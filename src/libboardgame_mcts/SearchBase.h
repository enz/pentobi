//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/SearchBase.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_SEARCH_BASE_H
#define LIBBOARDGAME_MCTS_SEARCH_BASE_H

#include <algorithm>
#include <array>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include "Atomic.h"
#include "BiasTerm.h"
#include "LastGoodReply.h"
#include "PlayerMove.h"
#include "Tree.h"
#include "TreeUtil.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/Barrier.h"
#include "libboardgame_util/BitMarker.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/IntervalChecker.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/StringUtil.h"
#include "libboardgame_util/TimeIntervalChecker.h"
#include "libboardgame_util/Timer.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_mcts::tree_util::find_node;
using libboardgame_util::get_abort;
using libboardgame_util::get_log;
using libboardgame_util::log;
using libboardgame_util::time_to_string;
using libboardgame_util::to_string;
using libboardgame_util::Barrier;
using libboardgame_util::BitMarker;
using libboardgame_util::Exception;
using libboardgame_util::IntervalChecker;
using libboardgame_util::StatisticsBase;
using libboardgame_util::StatisticsDirtyLockFree;
using libboardgame_util::StatisticsExt;
using libboardgame_util::Timer;
using libboardgame_util::TimeIntervalChecker;
using libboardgame_util::TimeSource;

//-----------------------------------------------------------------------------

/** Default optional compile-time parameters for Search.
    See description of class Search for more information. */
struct SearchParamConstDefault
{
    /** The floating type used for mean values and counts.
        The default type is @c float for a reduced node size and performance
        gains (especially on 32-bit systems). However, using @c float sets a
        practical limit on the number of simulations before the count and mean
        values go into saturation. This maximum is given by 2^d-1 with d being
        the digits in the mantissa (=23 for IEEE 754 float's). The search will
        terminate when this number is reached. For longer searches, the code
        should be compiled with floating type @c double. */
    typedef float Float;

    /** The maximum number of players. */
    static const PlayerInt max_players = 2;

    /** Use RAVE. */
    static const bool rave = false;

    /** Do not update RAVE values if the same move was played first by the
        other player.
        This improves RAVE performance in Go, where moves at the same point
        indicate that something was captured there, so the position has
        changed so significantly that the RAVE value of this move should not be
        updated to earlier positions. Setting it to true causes a slight
        slow-down in the update of the RAVE values. */
    static const bool rave_check_same = false;

    /** Enable distance weighting of RAVE updates.
        The weight decreases linearly from the start to the end of a
        simulation. The distance weight is applied in addition to the normal
        RAVE weight. */
    static const bool rave_dist_weighting = false;

    /** Enable Last-Good-Reply heuristic.
        @see LastGoodReply */
    static const bool use_last_good_reply = false;

    /** Use virtual loss in multi-threaded mode.
        See Chaslot et al.: Parallel Monte-Carlo Tree Search. 2008. */
    static const bool virtual_loss = false;
};

//-----------------------------------------------------------------------------

/** Game-independent Monte-Carlo tree search.
    Game-dependent functionality is added by implementing some pure virtual
    functions and by template parameters.

    RAVE (see @ref libboardgame_doc_rave) is implemented differently from
    the algorithm described in the original paper: RAVE values are not stored
    separately in the nodes but added to the normal values with a certain
    (constant) weight and up to a maximum visit count of the parent node. This
    saves memory in the tree and speeds up move selection in the in-tree phase.
    It is weaker than the original RAVE at a low number of simulations but
    seems to be equally good or even better at a high number of simulations.

    @tparam S The game-dependent state of a simulation. The state provides
    functions for move generation, evaluation of terminal positions, etc. The
    state should be thread-safe to support multiple states if multi-threading
    is used.
    @tparam M The move type. The type must be convertible to an integer by
    providing M::to_int() and M::range.
    @tparam R Optional compile-time parameters, see SearchParamConstDefault */
template<class S, class M, class R = SearchParamConstDefault>
class SearchBase
{
public:
    typedef S State;

    typedef M Move;

    typedef R SearchParamConst;

    typedef typename SearchParamConst::Float Float;

    typedef libboardgame_mcts::Node<M, Float> Node;

    typedef libboardgame_mcts::ChildIterator<Node> ChildIterator;

    typedef libboardgame_mcts::Tree<Node> Tree;

    typedef libboardgame_mcts::PlayerMove<M> PlayerMove;

    static const PlayerInt max_players = SearchParamConst::max_players;

    typedef array<StatisticsDirtyLockFree<Float>, max_players> RootStat;

    /** Constructor.
        @param nu_threads
        @param memory The memory to be used for (all) the search trees. If
        zero, a default value will be used. */
    SearchBase(unsigned nu_threads, size_t memory);

    virtual ~SearchBase();


    /** @name Pure virtual functions */
    /** @{ */

    /** Create a new game-specific state to be used in a thread of the
        search. */
    virtual unique_ptr<State> create_state() = 0;

    /** Get string representation of a move.
        This function is not a member function of the move type M because
        the move type may be a lightweight class that needs a context to
        be converted to a string. */
    virtual string get_move_string(Move mv) const = 0;

    /** Get the current number of players. */
    virtual PlayerInt get_nu_players() const = 0;

    /** Get player to play at root node of the search. */
    virtual PlayerInt get_player() const = 0;

    /** An evaluation value representing a 50% winning probability. */
    virtual Float get_tie_value() const = 0;

    /** @} */ // @name


    /** @name Virtual functions */
    /** @{ */

    /** Check if the position at the root is a follow-up position of the last
        search.
        In this function, the subclass can store the game state at the root of
        the search, compare it to the the one of the last search, check if
        the current state is a follow-up position and return the move sequence
        leading from the last position to the current one, so that the search
        can check if a subtree of the last search can be reused.
        This function will be called exactly once at the beginning of each
        search. The default implementation returns false.
        The information is also used for deciding whether to clear other
        caches from the last search (e.g. Last-Good-Reply heuristic). */
    virtual bool check_followup(vector<Move>& sequence);

    virtual string get_info() const;

    virtual string get_info_ext() const;

    /** Return the expected simulations per second.
        If the simulations per second vary a lot, it should return a value
        closer to the lower values. This value is used, for example, to
        determine an interval for checking expensive abort conditions in
        deterministic mode (in regular mode, the simulations per second will be
        measured and the interval will be adjusted automatically). That means
        that in deterministic mode, a pessimistic low value will cause more
        calls to the expensive function but an optimistic high value will
        delay aborting the search.
        The default implementation returns 100.
        @see set_deterministic() */
    virtual double expected_sim_per_sec() const;

    /** @} */ // @name


    /** @name Parameters */
    /** @{ */

    /** Interval in which a full move selection is performed in the in-tree
        phase at high parent counts.
        Takes advantage of the fact that at high parent counts the selected
        move is almost always the same as at the last visit of the node,
        so it will speed up the in-tree phase if we do a full move selection
        only at a certain intervals and play the move from the sequence of
        the last simulation otherwise.
        The default value is 1, which means that full move selection is always
        used.
        @see set_full_select_min. */
    void set_full_select_interval(unsigned n);

    /** The minimum node count to do a full move selection only in a certain
        interval.
        See set_full_select_interval(). */
    void set_full_select_min(Float n);

    /** Minimum count of a node to be expanded. */
    void set_expand_threshold(Float n);

    Float get_expand_threshold() const;

    /** Increase of the expand threshold per in-tree move played. */
    void set_expand_threshold_inc(Float n);

    Float get_expand_threshold_inc() const;

    /** Constant used in UCT bias term.
        @see BiasTerm */
    void set_bias_term_constant(Float c);

    Float get_bias_term_constant() const;

    /** Reuse the subtree from the previous search if the current position is
        a follow-up position of the previous one. */
    void set_reuse_subtree(bool enable);

    bool get_reuse_subtree() const;

    /** Reuse the tree from the previous search if the current position is
        the same position as the previous one. */
    void set_reuse_tree(bool enable);

    bool get_reuse_tree() const;

    void set_prune_full_tree(bool enable);

    bool get_prune_full_tree() const;

    /** Maximum parent visit count for applying RAVE. */
    void set_rave_parent_max(Float value);

    Float get_rave_parent_max() const;

    /** Maximum child value count for applying RAVE. */
    void set_rave_child_max(Float value);

    Float get_rave_child_max() const;

    /** Weight used for adding RAVE values to the node value. */
    void set_rave_weight(Float value);

    Float get_rave_weight() const;

    /** The RAVE distance weight at the end of a simulation.
        If RAVE distance weighting is used, the RAVE distance weight decreases
        linearly from 1 at the current position to this value at the end of a
        simulation. */
    void set_rave_dist_final(Float value);

    Float get_rave_dist_final() const;

    /** Value to start the tree pruning with.
        This value should be above typical count initializations if prior
        knowledge initialization is used. */
    void set_prune_count_start(Float n);

    Float get_prune_count_start() const;

    /** Total size of the trees in bytes. */
    void set_tree_memory(size_t memory);

    size_t get_tree_memory() const;

    /** Set deterministic mode.
        Note that using a fixed number of simulations instead of a time limit
        is not enough to make the search fully deterministic because the
        interval in which expensive abort conditions are checked (e.g. if the
        best move cannot change anymore) is adjusted dynamically depending on
        the number of simulations per second. In deterministic mode, a
        fixed interval is used. */
    void set_deterministic();

    /** @} */ // @name


    /** Run a search.
        @param[out] mv
        @param max_count Number of simulations to run. The search might return
        earlier if the best move cannot change anymore or if the count of the
        root node was initialized from an init tree
        @param min_simulations
        @param max_time Maximum search time. Only used if max_count is zero
        @param time_source Time source for time measurement
        @param always_search Always call the search, even if extracting the
        subtree to reuse was aborted due to max_time or util::get_abort(). If
        true, this will call a search with the partially extracted subtree,
        and the search will return immediately (because it also checks max_time
        and get_abort()). This flag should be true for regular searches because
        even a partially extracted subtree can be used for move generation, and
        false for pondering searches because here we don't need a search
        result but want to keep the full tree for reuse in a future search.
        @return @c false if no move could be generated because the position is
        a terminal position. */
    bool search(Move& mv, Float max_count, Float min_simulations,
                double max_time, TimeSource& time_source,
                bool always_search = true);

    const Tree& get_tree() const;

    string dump() const;

    /** Number of simulations in the current search in all threads. */
    size_t get_nu_simulations() const;

    /** Select the move to play.
        Uses select_child_final() on the root node. */
    bool select_move(Move& mv,
                     const vector<Move>* exclude_moves = nullptr) const;

    /** Select the best child of a node after the search.
        Selects child with highest visit count; the value is used as a
        tie-breaker for equal counts (important at very low number of
        simulations, e.g. all children have count 1 or 0). */
    const Node* select_child_final(const Node& node,
                            const vector<Move>* exclude_moves = nullptr) const;

    State& get_state(unsigned thread_id);

    const State& get_state(unsigned thread_id) const;

    /** Set a callback function that informs the caller about the
        estimated time left.
        The callback function will be called about every 0.1s. The arguments
        of the callback function are: elapsed time, estimated remaining time. */
    void set_callback(function<void(double, double)> callback);

    /** Get mean evaluation for all players at root node. */
    const RootStat& get_root_val() const;

    /** Create the threads used in the search.
        This cannot be done in the constructor because it uses the virtual
        function create_state(). This function will automatically be called
        before a search if the threads have not been constructed yet, but it
        is advisable to explicitely call it in the constructor of the subclass
        to save some time at the first move generation where the game clock
        might already be running. */
    void create_threads();

protected:
    struct Simulation
    {
        vector<const Node*> nodes;

        vector<const Node*> last_nodes;

        vector<PlayerMove> moves;

        array<Float, max_players> eval;
    };

    virtual void on_start_search(bool is_followup);

    virtual void on_simulation_finished(size_t n, const State& state,
                                        const Simulation& simulation);

    /** Time source for current search.
        Only valid during a search. */
    TimeSource& get_time_source();

private:
    class AssertionHandler
        : public libboardgame_util::AssertionHandler
    {
    public:
        AssertionHandler(const SearchBase& search);

        ~AssertionHandler();

        void run() override;

    private:
        const SearchBase& m_search;
    };

    /** Thread-specific search state. */
    struct ThreadState
    {
        unsigned thread_id;

        unique_ptr<State> state;

        unsigned full_select_counter;

        /** Was the search in this thread terminated because the search tree
            was full? */
        bool is_out_of_mem;

        /** Number of simulations in the current search in this thread. */
        size_t nu_simulations;

        Simulation simulation;

        StatisticsExt<> stat_len;

        /** Statistics about in-tree length where the full child select was
            skipped.
            @see Search::set_full_select_interval() */
        StatisticsExt<> stat_fs_len;

        StatisticsExt<> stat_in_tree_len;

        /** Local variable for update_rave().
            Stores if a move was played for each player.
            Reused for efficiency. */
        array<BitMarker<Move>, max_players> was_played;

        /** Local variable for update_rave().
            Stores the first time a move was played for each player.
            Elements are only defined if was_played is true.
            Reused for efficiency. */
        array<array<unsigned, Move::range>, max_players> first_play;

        ~ThreadState();
    };

    /** Thread in the parallel search.
        The thread waits for a call to start_search(), then runs the given
        search loop function (see Search::search_loop()) with the
        thread-specific search state. After start_search(),
        wait_search_finished() needs to called before calling start_search()
        again or destructing this object. */
    class Thread
    {
    public:
        typedef function<void(ThreadState&)> SearchFunc;

        ThreadState thread_state;

        Thread(SearchFunc& search_func);

        ~Thread();

        void run();

        void start_search();

        void wait_search_finished();

    private:
        SearchFunc m_search_func;

        bool m_quit;

        bool m_start_search_flag;

        bool m_search_finished_flag;

        Barrier m_thread_ready;

        mutex m_start_search_mutex;

        mutex m_search_finished_mutex;

        condition_variable m_start_search_cond;

        condition_variable m_search_finished_cond;

        unique_lock<mutex> m_search_finished_lock;

        thread m_thread;

        void thread_main();
    };

    unsigned m_nu_threads;

    /** See set_full_select_interval(). */
    unsigned m_full_select_interval;

    /** See set_full_select_min(). */
    Float m_full_select_min;

    Float m_expand_threshold;

    Float m_expand_threshold_inc;

    bool m_deterministic;

    bool m_reuse_subtree;

    bool m_reuse_tree;

    bool m_prune_full_tree;

    /** Player to play at the root node of the search. */
    PlayerInt m_player;

    /** Cached return value of get_nu_players() that stays constant during
        a search. */
    PlayerInt m_nu_players;

    /** Time of last search. */
    double m_last_time;

    Float m_prune_count_start;

    Float m_rave_parent_max;

    Float m_rave_child_max;

    Float m_rave_weight;

    Float m_rave_dist_final;

    /** Minimum simulations to perform in the current search.
        This does not include the count of simulations reused from a subtree of
        a previous search. */
    Float m_min_simulations;

    /** Maximum simulations of current search.
        This include the count of simulations reused from a subtree of a
        previous search. */
    Float m_max_count;

    /** Maximum count that can be exactly expressed in floating point type. */
    Float m_max_float_count;

    size_t m_tree_memory;

    size_t m_max_nodes;

    /** Maximum time of current search. */
    double m_max_time;

    TimeSource* m_time_source;

    BiasTerm<Float> m_bias_term;

    Timer m_timer;

    vector<unique_ptr<Thread>> m_threads;

    Tree m_tmp_tree;

    AssertionHandler m_assertion_handler;


    /** @name Members that are used concurrently by all threads during the
        lock-free multi-threaded search */
    /** @{ */

    Tree m_tree;

    /** See get_root_val(). */
    RootStat m_root_val;

    /** Current position value estimate for prior knowledge initialization.
        Derived form root values of last search and updated with root values
        of current search. */
    RootStat m_init_val;

    LastGoodReply<M, max_players> m_last_good_reply;

    /** See get_nu_simulations(). */
    LIBBOARDGAME_MCTS_ATOMIC(size_t) m_nu_simulations;

    /** Mutex used in log_thread() */
    mutable mutex m_log_mutex;

    /** @} */ // @name


    function<void(double, double)> m_callback;

    vector<Move> m_followup_sequence;

    static size_t get_max_nodes(size_t memory);

    bool check_abort(const ThreadState& thread_state) const;

    bool check_abort_expensive(ThreadState& thread_state) const;

    void check_create_threads();

    bool check_move_cannot_change(Float count, Float remaining) const;

    bool expand_node(ThreadState& thread_state, const Node& node,
                     const Node*& best_child);

    void log_thread(const ThreadState& thread_state, const string& s) const;

    void playout(ThreadState& thread_state);

    void play_in_tree(ThreadState& thread_state);

    bool prune(TimeSource& time_source, double time, double max_time,
               Float prune_min_count, Float& new_prune_min_count);

    void restore_root_from_children(Tree& tree, const Node& root);

    void search_loop(ThreadState& thread_state);

    const Node* select_child(const Node& node);

    void update_last_good_reply(ThreadState& thread_state);

    void update_rave(ThreadState& thread_state);

    void update_values(ThreadState& thread_state);
};


template<class S, class M, class R>
SearchBase<S, M, R>::ThreadState::~ThreadState()
{
}

template<class S, class M, class R>
SearchBase<S, M, R>::Thread::Thread(SearchFunc& search_func)
    : m_search_func(search_func),
      m_quit(false),
      m_start_search_flag(false),
      m_search_finished_flag(false),
      m_thread_ready(2),
      m_search_finished_lock(m_search_finished_mutex, defer_lock)
{
}

template<class S, class M, class R>
SearchBase<S, M, R>::Thread::~Thread()
{
    if (! m_thread.joinable())
        return;
    m_quit = true;
    {
        lock_guard<mutex> lock(m_start_search_mutex);
        m_start_search_flag = true;
    }
    m_start_search_cond.notify_one();
    m_thread.join();
}

template<class S, class M, class R>
void SearchBase<S, M, R>::Thread::run()
{
    m_thread = thread(bind(&Thread::thread_main, this));
    m_thread_ready.wait();
}

template<class S, class M, class R>
void SearchBase<S, M, R>::Thread::start_search()
{
    LIBBOARDGAME_ASSERT(m_thread.joinable());
    m_search_finished_lock.lock();
    {
        lock_guard<mutex> lock(m_start_search_mutex);
        m_start_search_flag = true;
    }
    m_start_search_cond.notify_one();
}

template<class S, class M, class R>
void SearchBase<S, M, R>::Thread::thread_main()
{
    //log() << "Start thread " << thread_state.thread_id << '\n';
    unique_lock<mutex> lock(m_start_search_mutex);
    m_thread_ready.wait();
    while (true)
    {
        while (! m_start_search_flag)
            m_start_search_cond.wait(lock);
        m_start_search_flag = false;
        if (m_quit)
            break;
        m_search_func(thread_state);
        {
            lock_guard<mutex> lock(m_search_finished_mutex);
            m_search_finished_flag = true;
        }
        m_search_finished_cond.notify_one();
    }
    //log() << "Finish thread " << thread_state.thread_id << '\n';
}

template<class S, class M, class R>
void SearchBase<S, M, R>::Thread::wait_search_finished()
{
    LIBBOARDGAME_ASSERT(m_thread.joinable());
    while (! m_search_finished_flag)
        m_search_finished_cond.wait(m_search_finished_lock);
    m_search_finished_flag = false;
    m_search_finished_lock.unlock();
}


template<class S, class M, class R>
SearchBase<S, M, R>::AssertionHandler::AssertionHandler(const SearchBase& search)
    : m_search(search)
{
}

template<class S, class M, class R>
SearchBase<S, M, R>::AssertionHandler::~AssertionHandler()
{
}

template<class S, class M, class R>
void SearchBase<S, M, R>::AssertionHandler::run()
{
    log(m_search.dump());
}


template<class S, class M, class R>
SearchBase<S, M, R>::SearchBase(unsigned nu_threads, size_t memory)
    : m_nu_threads(nu_threads),
      m_full_select_interval(1),
      m_full_select_min(numeric_limits<Float>::max()),
      m_expand_threshold(0),
      m_expand_threshold_inc(0),
      m_deterministic(false),
      m_reuse_subtree(true),
      m_reuse_tree(false),
      m_prune_full_tree(true),
      m_prune_count_start(16),
      m_rave_parent_max(50000),
      m_rave_child_max(2000),
      m_rave_weight(0.3f),
      m_rave_dist_final(0),
      m_tree_memory(memory == 0 ? 256000000 : memory),
      m_max_nodes(get_max_nodes(m_tree_memory)),
      m_bias_term(0),
      m_tmp_tree(m_max_nodes, m_nu_threads),
      m_assertion_handler(*this),
      m_tree(m_max_nodes, m_nu_threads)
{
    static_assert(numeric_limits<Float>::radix == 2, "");
    m_max_float_count = (size_t(1) << numeric_limits<Float>::digits) - 1;
}

template<class S, class M, class R>
SearchBase<S, M, R>::~SearchBase()
{
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::check_abort(const ThreadState& thread_state) const
{
    if (m_max_count > 0 && m_tree.get_root().get_visit_count() >= m_max_count)
    {
        log_thread(thread_state, "Maximum count reached");
        return true;
    }
    return false;
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::check_abort_expensive(ThreadState& thread_state) const
{
    if (get_abort())
    {
        log_thread(thread_state, "Search aborted");
        return true;
    }
    auto& root = m_tree.get_root();
    if (root.get_nu_children() == 1)
    {
        log_thread(thread_state, "Root has only one child");
        return true;
    }
    auto count = root.get_visit_count();
    if (count >= m_max_float_count)
    {
        log_thread(thread_state,
                   "Maximum count supported by floating type reached");
        return true;
    }
    auto time = m_timer();
    if (! m_deterministic && time < 0.1)
        // Simulations per second might be inaccurate for very small times
        return false;
    double simulations_per_sec;
    if (time == 0)
        simulations_per_sec = expected_sim_per_sec();
    else
    {
        size_t nu_simulations =
            LIBBOARDGAME_MCTS_ATOMIC_LOAD(m_nu_simulations,
                                          memory_order_relaxed);
        simulations_per_sec = double(nu_simulations) / time;
    }
    double remaining_time;
    Float remaining_simulations;
    if (m_max_count == 0)
    {
        // Search uses time limit
        if (time > m_max_time)
        {
            log_thread(thread_state, "Maximum time reached");
            return true;
        }
        remaining_time = m_max_time - time;
        remaining_simulations = Float(remaining_time * simulations_per_sec);
    }
    else
    {
        // Search uses count limit
        remaining_simulations = m_max_count - count;
        remaining_time = remaining_simulations / simulations_per_sec;
    }
    if (thread_state.thread_id == 0 && m_callback)
        m_callback(time, remaining_time);
    if (count + remaining_simulations > m_max_float_count)
        remaining_simulations = m_max_float_count - count;
    if (check_move_cannot_change(count, remaining_simulations))
    {
        log_thread(thread_state, "Move cannot change anymore");
        return true;
    }
    return false;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::check_create_threads()
{
    if (m_nu_threads != m_threads.size())
        create_threads();
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::check_followup(vector<Move>& sequence)
{
    LIBBOARDGAME_UNUSED(sequence);
    return false;
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::check_move_cannot_change(Float count,
                                               Float remaining) const
{
    if (remaining > count)
        return false;
    Float max_count = 0;
    Float second_max_count = 0;
    for (ChildIterator i(m_tree, m_tree.get_root()); i; ++i)
    {
        Float count = i->get_visit_count();
        if (count > max_count)
        {
            second_max_count = max_count;
            max_count = count;
        }
    }
    return (max_count > second_max_count + remaining);
}

template<class S, class M, class R>
void SearchBase<S, M, R>::create_threads()
{
#ifdef LIBBOARDGAME_MCTS_SINGLE_THREAD
    if (m_nu_threads > 1)
        throw Exception("libboardgame_mcts::Search was compiled"
                        " without support for multithreading");
#endif
    log("Creating ", m_nu_threads, " threads");
    m_threads.clear();
    auto search_func =
        static_cast<typename Thread::SearchFunc>(
                          bind(&SearchBase::search_loop, this, placeholders::_1));
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        unique_ptr<Thread> t(new Thread(search_func));
        auto& thread_state = t->thread_state;
        thread_state.thread_id = i;
        thread_state.state = create_state();
        for (PlayerInt j = 0; j < max_players; ++j)
            thread_state.was_played[j].clear();
        if (i > 0)
            t->run();
        m_threads.push_back(move(t));
    }
}

template<class S, class M, class R>
string SearchBase<S, M, R>::dump() const
{
    ostringstream s;
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        s << "Thread state " << i << ":\n"
          << get_state(i).dump();
    }
    return s.str();
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::expand_node(ThreadState& thread_state,
                                      const Node& node,
                                      const Node*& best_child)
{
    auto& state = *thread_state.state;
    typename Tree::NodeExpander expander(thread_state.thread_id, m_tree, node);
    state.gen_children(expander, m_init_val[state.get_to_play()].get_mean());
    if (! expander.is_tree_full())
    {
        expander.link_children();
        best_child = expander.get_best_child();
        return true;
    }
    return false;
}

template<class S, class M, class R>
double SearchBase<S, M, R>::expected_sim_per_sec() const
{
    return 100.0;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_bias_term_constant() const -> Float
{
    return m_bias_term.get_bias_term_constant();
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_expand_threshold() const -> Float
{
    return m_expand_threshold;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_expand_threshold_inc() const -> Float
{
    return m_expand_threshold_inc;
}

template<class S, class M, class R>
size_t SearchBase<S, M, R>::get_max_nodes(size_t memory)
{
    // Memory is used for 2 trees (m_tree and m_tmp_tree)
    size_t max_nodes = memory / sizeof(Node) / 2;
    // It doesn't make sense to set max_nodes higher than what can be accessed
    // with NodeIdx
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIdx>::max()));
    log("Search tree size: 2 x ", max_nodes, " nodes");
    return max_nodes;
}

template<class S, class M, class R>
inline size_t SearchBase<S, M, R>::get_nu_simulations() const
{
    return m_nu_simulations;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_root_val() const -> const RootStat&
{
    return m_root_val;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_prune_count_start() const -> Float
{
    return m_prune_count_start;
}

template<class S, class M, class R>
inline bool SearchBase<S, M, R>::get_prune_full_tree() const
{
    return m_prune_full_tree;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_rave_dist_final() const -> Float
{
    return m_rave_dist_final;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_rave_parent_max() const -> Float
{
    return m_rave_parent_max;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_rave_child_max() const -> Float
{
    return m_rave_child_max;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_rave_weight() const -> Float
{
    return m_rave_weight;
}

template<class S, class M, class R>
inline bool SearchBase<S, M, R>::get_reuse_subtree() const
{
    return m_reuse_subtree;
}

template<class S, class M, class R>
inline bool SearchBase<S, M, R>::get_reuse_tree() const
{
    return m_reuse_tree;
}

template<class S, class M, class R>
inline S& SearchBase<S, M, R>::get_state(unsigned thread_id)
{
    LIBBOARDGAME_ASSERT(thread_id < m_threads.size());
    return *m_threads[thread_id]->thread_state.state;
}

template<class S, class M, class R>
inline const S& SearchBase<S, M, R>::get_state(unsigned thread_id) const
{
    LIBBOARDGAME_ASSERT(thread_id < m_threads.size());
    return *m_threads[thread_id]->thread_state.state;
}

template<class S, class M, class R>
inline TimeSource& SearchBase<S, M, R>::get_time_source()
{
    LIBBOARDGAME_ASSERT(m_time_source != 0);
    return *m_time_source;
}

template<class S, class M, class R>
size_t SearchBase<S, M, R>::get_tree_memory() const
{
    return m_tree_memory;
}

template<class S, class M, class R>
inline auto SearchBase<S, M, R>::get_tree() const -> const Tree&
{
    return m_tree;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::log_thread(const ThreadState& thread_state,
                                 const string& s) const
{
    lock_guard<mutex> lock(m_log_mutex);
    log('[', thread_state.thread_id, "] ", s);
}

template<class S, class M, class R>
void SearchBase<S, M, R>::on_simulation_finished(size_t n, const State& state,
                                                 const Simulation& simulation)
{
    LIBBOARDGAME_UNUSED(n);
    LIBBOARDGAME_UNUSED(state);
    LIBBOARDGAME_UNUSED(simulation);
    // Default implementation does nothing
}

template<class S, class M, class R>
void SearchBase<S, M, R>::on_start_search(bool is_followup)
{
    // Default implementation does nothing
    LIBBOARDGAME_UNUSED(is_followup);
}

template<class S, class M, class R>
void SearchBase<S, M, R>::playout(ThreadState& thread_state)
{
    auto& state = *thread_state.state;
    auto& simulation = thread_state.simulation;
    state.start_playout();
    while (true)
    {
        Move lgr1 = Move::null();
        Move lgr2 = Move::null();
        if (SearchParamConst::use_last_good_reply)
        {
            auto& moves = simulation.moves;
            auto nu_moves = moves.size();
            if (nu_moves > 0)
            {
                Move last_mv = moves[nu_moves - 1].move;
                Move second_last_mv = Move::null();
                if (nu_moves > 1)
                    second_last_mv = moves[nu_moves - 2].move;
                m_last_good_reply.get(state.get_to_play(), last_mv,
                                      second_last_mv, lgr1, lgr2);
            }
        }
        PlayerMove move;
        if (! state.gen_playout_move(lgr1, lgr2, move))
            break;
        simulation.moves.push_back(move);
        state.play_playout(move.move);
    }
}

template<class S, class M, class R>
void SearchBase<S, M, R>::play_in_tree(ThreadState& thread_state)
{
    auto& state = *thread_state.state;
    auto& simulation = thread_state.simulation;
    auto& root = m_tree.get_root();
    auto node = &root;
    m_tree.inc_visit_count(*node);
    simulation.nodes.push_back(node);
    Float expand_threshold = m_expand_threshold;
    if (thread_state.full_select_counter > 0)
    {
        // Don't do a full move select but play same sequence as last time
        // as long as the node count is above m_full_select_min.
        --thread_state.full_select_counter;
        unsigned depth = 0;
        while (node->has_children())
        {
            if (node->get_visit_count() <= m_full_select_min
                    || depth + 1 >= simulation.last_nodes.size())
                break;
            node = simulation.last_nodes[depth + 1];
            m_tree.inc_visit_count(*node);
#ifndef LIBBOARDGAME_MCTS_SINGLE_THREAD
            if (SearchParamConst::virtual_loss && m_nu_threads > 0)
                m_tree.add_value(*node, 0);
#endif
            simulation.nodes.push_back(node);
            Move mv = node->get_move();
            simulation.moves.push_back(PlayerMove(state.get_to_play(), mv));
            state.play_in_tree(mv);
            ++depth;
            expand_threshold += m_expand_threshold_inc;
        }
    }
    else
        thread_state.full_select_counter = m_full_select_interval;
    thread_state.stat_fs_len.add(double(simulation.moves.size()));
    while (node->has_children())
    {
        node = select_child(*node);
        m_tree.inc_visit_count(*node);
#ifndef LIBBOARDGAME_MCTS_SINGLE_THREAD
        if (SearchParamConst::virtual_loss && m_nu_threads > 0)
            m_tree.add_value(*node, 0);
#endif
        simulation.nodes.push_back(node);
        Move mv = node->get_move();
        simulation.moves.push_back(PlayerMove(state.get_to_play(), mv));
        state.play_in_tree(mv);
        expand_threshold += m_expand_threshold_inc;
    }
    state.finish_in_tree();
    if (node->get_visit_count() > expand_threshold)
    {
        if (! expand_node(thread_state, *node, node))
            thread_state.is_out_of_mem = true;
        else if (node != nullptr)
        {
            simulation.nodes.push_back(node);
            Move mv = node->get_move();
            simulation.moves.push_back(PlayerMove(state.get_to_play(), mv));
            state.play_expanded_child(mv);
        }
    }
    thread_state.stat_in_tree_len.add(double(simulation.moves.size()));
}

template<class S, class M, class R>
string SearchBase<S, M, R>::get_info() const
{
    auto& root = m_tree.get_root();
    if (m_threads.empty())
        return string();
    auto& thread_state = m_threads[0]->thread_state;
    ostringstream s;
    if (thread_state.nu_simulations == 0)
    {
        s << "No simulations in thread 0\n";
        return s.str();
    }
    s << fixed << setprecision(2) << "Val: " << root.get_value()
      << setprecision(0) << ", ValCnt: " << root.get_value_count()
      << ", VstCnt: " << root.get_visit_count()
      << ", Sim: " << m_nu_simulations;
    auto child = select_child_final(root);
    if (child != nullptr && root.get_visit_count() > 0)
        s << setprecision(1) << ", Chld: "
          << (100 * child->get_visit_count() / root.get_visit_count())
          << '%';
    s << "\nNds: " << m_tree.get_nu_nodes()
      << ", Tm: " << time_to_string(m_last_time)
      << setprecision(0) << ", Sim/s: "
      << (double(m_nu_simulations) / m_last_time)
      << ", Len: " << thread_state.stat_len.to_string(true, 1, true)
      << "\nDp: " << thread_state.stat_in_tree_len.to_string(true, 1, true)
      << ", FS: " << thread_state.stat_fs_len.to_string(true, 1, true)
      << "\n";
    return s.str();
}

template<class S, class M, class R>
string SearchBase<S, M, R>::get_info_ext() const
{
    return string();
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::prune(TimeSource& time_source, double time,
                            double max_time, Float prune_min_count,
                            Float& new_prune_min_count)
{
    Timer timer(time_source);
    TimeIntervalChecker interval_checker(time_source, max_time);
    if (m_deterministic)
        interval_checker.set_deterministic(1000000);
    log("Pruning count ", prune_min_count, " (at tm ", time, ")");
    m_tmp_tree.clear(m_tree.get_root().get_value());
    if (! m_tree.copy_subtree(m_tmp_tree, m_tmp_tree.get_root(),
                              m_tree.get_root(), prune_min_count, true,
                              &interval_checker))
    {
        log("Pruning aborted");
        return false;
    }
    int percent = int(m_tmp_tree.get_nu_nodes() * 100 / m_tree.get_nu_nodes());
    log("Pruned size: ", m_tmp_tree.get_nu_nodes(), " (", percent, "%, tm=",
        timer(), ")");
    m_tree.swap(m_tmp_tree);
    if (percent > 50)
    {
        if (prune_min_count >= 0.5 * numeric_limits<Float>::max())
            return false;
        new_prune_min_count = prune_min_count * 2;
        return true;
    }
    else
    {
        new_prune_min_count = prune_min_count;
        return true;
    }
}

/** Restore the value and count of a root node from its children.
    The value of a root node has a different meaning than than values of inner
    nodes (position vs. move value) so after reusing a subtree, we need to
    restore it from the children. We use only the child with the highest count
    to avoid backing up many values of unvisited children that have only a
    value and count from prior knowledge initialization. */
template<class S, class M, class R>
void SearchBase<S, M, R>::restore_root_from_children(Tree& tree, const Node& root)
{
    const Node* best_child = nullptr;
    Float max_count = 0;
    for (ChildIterator i(tree, root); i; ++i)
        if (i->get_visit_count() > max_count)
        {
            best_child = &(*i);
            max_count = i->get_visit_count();
        }
    if (best_child == nullptr)
        tree.init_root_value(get_tie_value(), 0);
    else
        tree.init_root_value(best_child->get_value(),
                             best_child->get_value_count());
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::search(Move& mv, Float max_count,
                                 Float min_simulations, double max_time,
                                 TimeSource& time_source, bool always_search)
{
    check_create_threads();
    bool is_followup = check_followup(m_followup_sequence);
    on_start_search(is_followup);
    if (max_count > 0)
        // A fixed number of simulations means that no time limit is used, but
        // max_time is still used at some places in the code, so we set it to
        // infinity
        max_time = numeric_limits<double>::max();
    m_nu_players = get_nu_players();
    bool clear_tree = true;
    bool is_same = false;
    if (is_followup && m_followup_sequence.empty())
    {
        is_same = true;
        is_followup = false;
    }
    for (PlayerInt i = 0; i < m_nu_players; ++i)
    {
        m_init_val[i].clear();
        m_init_val[i].add(get_tie_value());
    }
    if (is_same || (is_followup && m_followup_sequence.size() <= m_nu_players))
        for (PlayerInt i = 0; i < m_nu_players; ++i)
            if (m_root_val[i].get_count() > 0)
                m_init_val[i] = m_root_val[i];
    if ((m_reuse_subtree && is_followup) || (m_reuse_tree && is_same))
    {
        size_t tree_nodes = m_tree.get_nu_nodes();
        if (m_followup_sequence.empty())
        {
            if (tree_nodes > 1)
                log("Reusing all ", tree_nodes, "nodes (count=",
                    m_tree.get_root().get_visit_count(), ")");
        }
        else
        {
            Timer timer(time_source);
            m_tmp_tree.clear(get_tie_value());
            auto node = find_node(m_tree, m_followup_sequence);
            if (node != nullptr)
            {
                TimeIntervalChecker interval_checker(time_source, max_time);
                if (m_deterministic)
                    interval_checker.set_deterministic(1000000);
                bool aborted =
                    ! m_tree.extract_subtree(m_tmp_tree, *node, true,
                                             &interval_checker);
                auto& tmp_tree_root = m_tmp_tree.get_root();
                if (! is_same)
                    restore_root_from_children(m_tmp_tree, tmp_tree_root);
                if (aborted && ! always_search)
                    return false;
                size_t tmp_tree_nodes = m_tmp_tree.get_nu_nodes();
                if (tree_nodes > 1 && tmp_tree_nodes > 1)
                {
                    double time = timer();
                    double reuse = double(tmp_tree_nodes) / double(tree_nodes);
                    double percent = 100 * reuse;
                    log("Reusing ", tmp_tree_nodes, " nodes (", std::fixed,
                        setprecision(1), percent, "% tm=", setprecision(4),
                        time, ")");
                    m_tree.swap(m_tmp_tree);
                    clear_tree = false;
                    max_time -= time;
                    if (max_time < 0)
                        max_time = 0;
                }
            }
        }
    }
    if (clear_tree)
        m_tree.clear(get_tie_value());

    m_timer.reset(time_source);
    m_time_source = &time_source;
    m_player = get_player();
    for (PlayerInt i = 0; i < m_nu_players; ++i)
        m_root_val[i].clear();
    if (SearchParamConst::use_last_good_reply && ! is_followup)
        m_last_good_reply.init(m_nu_players);
    for (unsigned i = 0; i < m_threads.size(); ++i)
    {
        auto& thread_state = m_threads[i]->thread_state;
        thread_state.nu_simulations = 0;
        thread_state.full_select_counter = 0;
        thread_state.stat_len.clear();
        thread_state.stat_in_tree_len.clear();
        thread_state.stat_fs_len.clear();
        thread_state.state->start_search();
    }
    m_max_count = max_count;
    m_min_simulations = min_simulations;
    m_max_time = max_time;
    m_nu_simulations = 0;
    Float prune_min_count = get_prune_count_start();

    // Don't use multi-threading for very short searches (less than 0.5s).
    // There are too many lost updates at the beginning (e.g. if all threads
    // expand the root node and only the children of the last thread are used)
    auto reused_count = m_tree.get_root().get_visit_count();
    unsigned nu_threads = m_nu_threads;
    if (max_time < 0.5
        || (max_count > 0
            && (max_count - reused_count) / expected_sim_per_sec() < 0.5))
    {
        log("Using single-threading for very short search");
        nu_threads = 1;
    }

    auto& thread_state_0 = m_threads[0]->thread_state;
    auto& root = m_tree.get_root();
    if (! root.has_children())
    {
        const Node* best_child;
        thread_state_0.state->start_simulation(0);
        expand_node(thread_state_0, root, best_child);
    }

    while (true)
    {
        for (unsigned i = 1; i < nu_threads; ++i)
            m_threads[i]->start_search();
        search_loop(thread_state_0);
        for (unsigned i = 1; i < nu_threads; ++i)
            m_threads[i]->wait_search_finished();
        bool is_out_of_mem = false;
        for (unsigned i = 0; i < nu_threads; ++i)
            if (m_threads[i]->thread_state.is_out_of_mem)
            {
                is_out_of_mem = true;
                break;
            }
        if (! is_out_of_mem)
            break;
        if (! m_prune_full_tree)
        {
            log("Maximum tree size reached");
            break;
        }
        double time = m_timer();
        if (! prune(time_source, time, max_time - time, prune_min_count,
                    prune_min_count))
        {
            log("Aborting search because pruning failed.");
            break;
        }
    }

    m_last_time = m_timer();
    log(get_info());
    bool result = select_move(mv);
    m_time_source = nullptr;
    return result;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::search_loop(ThreadState& thread_state)
{
    auto& state = *thread_state.state;
    auto& simulation = thread_state.simulation;
    simulation.last_nodes.clear();
    double time_interval = 0.1;
    if (m_max_count == 0 && m_max_time < 1)
        time_interval = 0.1 * m_max_time;
    IntervalChecker expensive_abort_checker(*m_time_source, time_interval,
                                           bind(&SearchBase::check_abort_expensive,
                                                this, ref(thread_state)));
    if (m_deterministic)
    {
        unsigned interval =
            static_cast<unsigned>(max(1.0, expected_sim_per_sec() / 5.0));
        expensive_abort_checker.set_deterministic(interval);
    }
    while (true)
    {
        thread_state.is_out_of_mem = false;
        auto root_count = m_tree.get_root().get_visit_count();
        auto nu_simulations =
            LIBBOARDGAME_MCTS_ATOMIC_FETCH_ADD(m_nu_simulations, 1);
        if (root_count > 0 && nu_simulations > m_min_simulations
            && (check_abort(thread_state) || expensive_abort_checker()))
        {
            LIBBOARDGAME_MCTS_ATOMIC_FETCH_ADD(m_nu_simulations, -1);
            break;
        }
        ++thread_state.nu_simulations;
        simulation.nodes.clear();
        simulation.moves.clear();
        state.start_simulation(nu_simulations);
        play_in_tree(thread_state);
        if (thread_state.is_out_of_mem)
            return;
        playout(thread_state);
        state.evaluate_playout(simulation.eval);
        thread_state.stat_len.add(double(simulation.moves.size()));
        update_values(thread_state);
        if (SearchParamConst::rave)
            update_rave(thread_state);
        if (SearchParamConst::use_last_good_reply)
            update_last_good_reply(thread_state);
        on_simulation_finished(nu_simulations, state, simulation);
        simulation.last_nodes = simulation.nodes;
    }
}

template<class S, class M, class R>
auto SearchBase<S, M, R>::select_child(const Node& node) -> const Node*
{
    const Node* best_child = nullptr;
    Float best_value = -numeric_limits<Float>::max();
    ChildIterator i(m_tree, node);
    LIBBOARDGAME_ASSERT(i);
    m_bias_term.start_iteration(node.get_visit_count());
    auto bias_upper_limit = m_bias_term.get(0);
    Float limit = -numeric_limits<Float>::max();
    do
    {
        auto value = i->get_value();
        if (value < limit)
            continue;
        value += m_bias_term.get(i->get_value_count());
        if (value > best_value)
        {
            best_value = value;
            best_child = &(*i);
            limit = best_value - bias_upper_limit;
        }
    }
    while (++i);
    return best_child;
}

template<class S, class M, class R>
auto SearchBase<S, M, R>::select_child_final(const Node& node,
                       const vector<Move>* exclude_moves) const -> const Node*
{
    // Select the child with the highest visit count, use value as tie breaker
    const Node* result = nullptr;
    Float max_count = -1;
    Float max_count_value = -numeric_limits<Float>::max();
    for (ChildIterator i(m_tree, node); i; ++i)
    {
        Float count = i->get_visit_count();
        if (count > max_count
            || (count == max_count && i->get_value() > max_count_value))
        {
            if (exclude_moves != nullptr
                && find(exclude_moves->begin(), exclude_moves->end(),
                        i->get_move()) != exclude_moves->end())
                continue;
            max_count = count;
            max_count_value = i->get_value();
            result = &(*i);
        }
    }
    return result;
}

template<class S, class M, class R>
bool SearchBase<S, M, R>::select_move(Move& mv, const vector<Move>* exclude_moves)
    const
{
    auto child = select_child_final(m_tree.get_root(), exclude_moves);
    if (child != nullptr)
    {
        mv = child->get_move();
        return true;
    }
    else
        return false;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_bias_term_constant(Float c)
{
    m_bias_term.set_bias_term_constant(c);
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_callback(function<void(double, double)> callback)
{
    m_callback = callback;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_deterministic()
{
    m_deterministic = true;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_expand_threshold(Float n)
{
    m_expand_threshold = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_expand_threshold_inc(Float n)
{
    m_expand_threshold_inc = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_full_select_interval(unsigned n)
{
    m_full_select_interval = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_full_select_min(Float n)
{
    m_full_select_min = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_prune_count_start(Float n)
{
    m_prune_count_start = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_prune_full_tree(bool enable)
{
    m_prune_full_tree = enable;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_rave_dist_final(Float v)
{
    m_rave_dist_final = v;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_rave_parent_max(Float n)
{
    m_rave_parent_max = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_rave_child_max(Float n)
{
    m_rave_child_max = n;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_rave_weight(Float v)
{
    m_rave_weight = v;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_reuse_subtree(bool enable)
{
    m_reuse_subtree = enable;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_reuse_tree(bool enable)
{
    m_reuse_tree = enable;
}

template<class S, class M, class R>
void SearchBase<S, M, R>::set_tree_memory(size_t memory)
{
    m_tree_memory = memory;
    m_max_nodes = get_max_nodes(memory);
    m_tree.set_max_nodes(m_max_nodes);
    m_tmp_tree.set_max_nodes(m_max_nodes);
}

template<class S, class M, class R>
void SearchBase<S, M, R>::update_last_good_reply(ThreadState& thread_state)
{
    const auto& simulation = thread_state.simulation;
    auto& eval = simulation.eval;
    auto max_eval = eval[0];
    for (PlayerInt i = 1; i < m_nu_players; ++i)
        max_eval = max(eval[i], max_eval);
    array<bool,max_players> is_winner;
    for (PlayerInt i = 0; i < m_nu_players; ++i)
        // Note: this handles a draw as a win. Without additional information
        // we cannot make a good decision how to handle draws and some
        // experiments in Blokus Duo showed (with low confidence) that treating
        // them as a win for both players is slighly better than treating them
        // as a loss for both.
        is_winner[i] = (eval[i] == max_eval);
    auto& moves = simulation.moves;
    auto nu_moves = moves.size();
    if (nu_moves < 2)
        return;
    Move last_mv = moves[0].move;
    Move second_last_mv = Move::null();
    for (size_t i = 1; i < nu_moves; ++i)
    {
        PlayerMove reply = moves[i];
        PlayerInt player = reply.player;
        Move mv = reply.move;
        if (is_winner[player])
            m_last_good_reply.store(player, last_mv, second_last_mv, mv);
        else
            m_last_good_reply.forget(player, last_mv, second_last_mv, mv);
        second_last_mv = last_mv;
        last_mv = mv;
    }
}

template<class S, class M, class R>
void SearchBase<S, M, R>::update_rave(ThreadState& thread_state)
{
    const auto& state = *thread_state.state;
    auto& moves = thread_state.simulation.moves;
    auto nu_moves = static_cast<unsigned>(moves.size());
    if (nu_moves == 0)
        return;
    auto& was_played = thread_state.was_played;
    auto& first_play = thread_state.first_play;
    auto& nodes = thread_state.simulation.nodes;
    unsigned nu_nodes = static_cast<unsigned>(nodes.size());
    unsigned i = nu_moves - 1;
    LIBBOARDGAME_ASSERT(nu_nodes > 1);

    // Fill was_played and first_play with information from playout moves
    for ( ; i >= nu_nodes - 1; --i)
    {
        auto mv = moves[i];
        if (! state.skip_rave(mv.move))
        {
            was_played[mv.player].set(mv.move);
            first_play[mv.player][mv.move.to_int()] = i;
        }
    }

    // Add RAVE values to children of nodes of current simulation
    while (true)
    {
        const auto node = nodes[i];
        if (node->get_visit_count() > m_rave_parent_max)
            break;
        auto mv = moves[i];
        auto player = mv.player;
        Float dist_weight_factor;
        if (SearchParamConst::rave_dist_weighting)
            dist_weight_factor = (1 - m_rave_dist_final) / Float(nu_moves - i);
        ChildIterator it(m_tree, *node);
        LIBBOARDGAME_ASSERT(it);
        do
        {
            auto mv = it->get_move();
            if (! was_played[player][mv]
                    || it->get_value_count() > m_rave_child_max)
                continue;
            auto first = first_play[player][mv.to_int()];
            LIBBOARDGAME_ASSERT(first > i);
            if (SearchParamConst::rave_check_same)
            {
                bool other_played_same = false;
                for (PlayerInt j = 0; j < m_nu_players; ++j)
                    if (j != player && was_played[j][mv])
                    {
                        auto first_other = first_play[j][mv.to_int()];
                        if (first_other >= i && first_other <= first)
                        {
                            other_played_same = true;
                            break;
                        }
                    }
                if (other_played_same)
                    continue;
            }
            Float weight = m_rave_weight;
            if (SearchParamConst::rave_dist_weighting)
                weight *= 1 - Float(first - i) * dist_weight_factor;
            m_tree.add_value(*it, thread_state.simulation.eval[player], weight);
        }
        while (++it);
        if (i == 0)
            break;
        if (! state.skip_rave(mv.move))
        {
            was_played[player].set(mv.move);
            first_play[player][mv.move.to_int()] = i;
        }
        --i;
    }

    // Reset was_played
    while (true)
    {
        ++i;
        if (i >= nu_moves)
            break;
        auto mv = moves[i];
        was_played[mv.player].clear_word(mv.move);
    }
}

template<class S, class M, class R>
void SearchBase<S, M, R>::update_values(ThreadState& thread_state)
{
    const auto& simulation = thread_state.simulation;
    auto& nodes = simulation.nodes;
    auto& eval = simulation.eval;
    m_tree.add_value(m_tree.get_root(), eval[m_player]);
    unsigned nu_nodes = static_cast<unsigned>(nodes.size());
    for (unsigned i = 1; i < nu_nodes; ++i)
    {
        auto& node = *nodes[i];
        auto mv = simulation.moves[i - 1];
        m_tree.add_value(node, eval[mv.player]);
#ifndef LIBBOARDGAME_MCTS_SINGLE_THREAD
        if (SearchParamConst::virtual_loss && m_nu_threads > 0)
            m_tree.remove_value(node, 0);
#endif
    }
    for (PlayerInt i = 0; i < m_nu_players; ++i)
    {
        m_root_val[i].add(eval[i]);
        m_init_val[i].add(eval[i]);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_SEARCH_BASE_H
