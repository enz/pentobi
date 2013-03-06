//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Search.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_SEARCH_H
#define LIBBOARDGAME_MCTS_SEARCH_H

#include <algorithm>
#include <array>
#include <functional>
#include "BiasTerm.h"
#include "LastGoodReply.h"
#include "PlayerMove.h"
#include "Tree.h"
#include "TreeUtil.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/Barrier.h"
#include "libboardgame_util/BitMarker.h"
#include "libboardgame_util/FmtSaver.h"
#include "libboardgame_util/IntervalChecker.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/StringUtil.h"
#include "libboardgame_util/TimeIntervalChecker.h"
#include "libboardgame_util/Timer.h"
#include "libboardgame_util/Unused.h"

#ifdef USE_BOOST_THREAD
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#else
#include <condition_variable>
#include <mutex>
#include <thread>
#endif

namespace libboardgame_mcts {

using namespace std;
using libboardgame_mcts::tree_util::find_node;
using libboardgame_util::get_abort;
using libboardgame_util::log;
using libboardgame_util::time_to_string;
using libboardgame_util::to_string;
using libboardgame_util::Barrier;
using libboardgame_util::BitMarker;
using libboardgame_util::FmtSaver;
using libboardgame_util::IntervalChecker;
using libboardgame_util::StatisticsBase;
using libboardgame_util::StatisticsDirtyLockFree;
using libboardgame_util::StatisticsExt;
using libboardgame_util::Timer;
using libboardgame_util::TimeIntervalChecker;
using libboardgame_util::TimeSource;

#ifdef USE_BOOST_THREAD
using boost::condition_variable;
using boost::lock_guard;
using boost::mutex;
using boost::thread;
using boost::unique_lock;
#endif

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
class Search
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

    static const bool log_move_selection = false;

    /** Constructor.
        @param nu_threads
        @param memory The memory to be used for (all) the search trees. If
        zero, a default value will be used. */
    Search(unsigned nu_threads, size_t memory);

    virtual ~Search() throw();


    /** @name Pure virtual functions */
    // @{

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

    // @} // @name


    /** @name Virtual functions */
    // @{

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

    virtual void write_info(ostream& out) const;

    virtual void write_info_ext(ostream& out) const;

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

    // @} // @name


    /** @name Parameters */
    // @{

    /** Minimum count of a node to be expanded. */
    void set_expand_threshold(Float n);

    Float get_expand_threshold() const;

    /** Increase of the expand threshold per in-tree move played. */
    void set_expand_threshold_incr(Float n);

    Float get_expand_threshold_incr() const;

    /** Constant used in UCT bias term. */
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

    /** Maximum parent count for applying RAVE. */
    void set_rave_max_parent_count(Float value);

    Float get_rave_max_parent_count() const;

    /** Maximum child count for applying RAVE. */
    void set_rave_max_child_count(Float value);

    Float get_rave_max_child_count() const;

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

    // @} // @name


    /** Run a search.
        @param[out] mv
        @param max_count Number of simulations to run. The search might return
        earlier, if the best move cannot change anymore, or if the count of the
        root node was initialized from an init tree
        @param min_simulations
        @param max_time Maximum search time. Only used if max_count is zero
        @param time_source Time source for time measurement
        @param always_search Always call the search, even if extracting the
        subtree to reuse was aborted due to max_time or util::get_abort(). If
        true, this will call a search with the partially extracted subtree,
        and the search will return immediately (because it also checks max_time
        and get_abort(). This flag should be true for regular searches, because
        even a partially extracted subtree can be used for move generation, and
        false for pondering searches, because here we don't need a search
        result, but want to keep the full tree for reuse in a future
        searches.
        @return @c false, if no move could be generated. This can happen if the
        root node was not expanded, because the position is a terminal
        position or because the search was immediately aborted. */
    bool search(Move& mv, Float max_count, Float min_simulations,
                double max_time, TimeSource& time_source,
                bool always_search = true);

    const Tree& get_tree() const;

    void dump(ostream& out) const;

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

        array<Float, max_players> eval;
    };

    virtual void on_start_search();

    virtual void on_search_iteration(size_t n, const State& state,
                                     const Simulation& simulation);

    /** Time source for current search.
        Only valid during a search. */
    TimeSource& get_time_source();

private:
    class AssertionHandler
        : public libboardgame_util::AssertionHandler
    {
    public:
        AssertionHandler(const Search& search);

        ~AssertionHandler() throw();

        void run();

    private:
        const Search& m_search;
    };

    /** Thread-specific search state. */
    struct ThreadState
    {
        unsigned thread_id;

        unique_ptr<State> state;

        /** Was the search in this thread terminated because the search tree
            was full? */
        bool is_out_of_mem;

        /** Number of simulations in the current search in this thread. */
        size_t nu_simulations;

        Simulation simulation;

        StatisticsExt<> stat_len;

        StatisticsExt<> stat_in_tree_len;

        /** Local variable for update_rave_values().
            Stores if a move was played for each player.
            Reused for efficiency. */
        array<BitMarker<Move>,max_players> was_played;

        /** Local variable for update_rave_values().
            Stores the first time a move was played for each player.
            Elements are only defined if was_played is true.
            Reused for efficiency. */
        array<array<unsigned,Move::range>,max_players> first_play;
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

    Float m_expand_threshold;

    Float m_expand_threshold_incr;

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

    Float m_rave_max_parent_count;

    Float m_rave_max_child_count;

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
    // @{

    Tree m_tree;

    /** See get_root_val(). */
    RootStat m_root_val;

    /** Current position value estimate for prior knowledge initialization.
        Derived form root values of last search and updated with root values
        of current search. */
    RootStat m_init_val;

    LastGoodReply<S, M, max_players> m_last_good_reply;

    /** See get_nu_simulations(). */
    atomic<size_t> m_nu_simulations;

    // @} // @name


    function<void(double, double)> m_callback;

    vector<Move> m_followup_sequence;

    static size_t get_max_nodes(size_t memory);

    bool check_abort(const ThreadState& thread_state) const;

    bool check_abort_expensive(ThreadState& thread_state) const;

    void check_create_threads();

    bool check_move_cannot_change(Float count, Float remaining) const;

    bool expand_node(unsigned thread_id, const Node& node,
                     const Node*& best_child, Float init_val);

    void log_thread(const ThreadState& thread_state, const string& s) const;

    void playout(ThreadState& thread_state);

    void play_in_tree(ThreadState& thread_state, bool& is_terminal);

    bool prune(TimeSource& time_source, double time, double max_time,
               Float prune_min_count, Float& new_prune_min_count);

    void restore_root_from_children(Tree& tree, const Node& root);

    void search_loop(ThreadState& thread_state);

    const Node* select_child(const Node& node);

    void update_last_good_reply(ThreadState& thread_state);

    void update_rave_values(ThreadState& thread_state);

    void update_rave_values(ThreadState& thread_state, unsigned i,
                            PlayerInt player);

    void update_values(ThreadState& thread_state);
};


template<class S, class M, class R>
Search<S, M, R>::Thread::Thread(SearchFunc& search_func)
    : m_search_func(search_func),
      m_quit(false),
      m_start_search_flag(false),
      m_search_finished_flag(false),
      m_thread_ready(2),
      m_search_finished_lock(m_search_finished_mutex)
{
}

template<class S, class M, class R>
Search<S, M, R>::Thread::~Thread()
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
void Search<S, M, R>::Thread::run()
{
    m_thread = thread(bind(&Thread::thread_main, this));
    m_thread_ready.wait();
}

template<class S, class M, class R>
void Search<S, M, R>::Thread::start_search()
{
    LIBBOARDGAME_ASSERT(m_thread.joinable());
    {
        lock_guard<mutex> lock(m_start_search_mutex);
        m_start_search_flag = true;
    }
    m_start_search_cond.notify_one();
}

template<class S, class M, class R>
void Search<S, M, R>::Thread::thread_main()
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
void Search<S, M, R>::Thread::wait_search_finished()
{
    LIBBOARDGAME_ASSERT(m_thread.joinable());
    while (! m_search_finished_flag)
        m_search_finished_cond.wait(m_search_finished_lock);
    m_search_finished_flag = false;
}


template<class S, class M, class R>
Search<S, M, R>::AssertionHandler::AssertionHandler(const Search& search)
    : m_search(search)
{
}

template<class S, class M, class R>
Search<S, M, R>::AssertionHandler::~AssertionHandler() throw()
{
}

template<class S, class M, class R>
void Search<S, M, R>::AssertionHandler::run()
{
    m_search.dump(log());
}


template<class S, class M, class R>
Search<S, M, R>::Search(unsigned nu_threads, size_t memory)
    : m_nu_threads(nu_threads),
      m_expand_threshold(0),
      m_expand_threshold_incr(0),
      m_deterministic(false),
      m_reuse_subtree(true),
      m_reuse_tree(false),
      m_prune_full_tree(true),
      m_prune_count_start(16),
      m_rave_max_parent_count(50000),
      m_rave_max_child_count(500),
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
Search<S, M, R>::~Search() throw()
{
}

template<class S, class M, class R>
bool Search<S, M, R>::check_abort(const ThreadState& thread_state) const
{
    if (m_max_count > 0 && m_tree.get_root().get_visit_count() >= m_max_count)
    {
        log_thread(thread_state, "Maximum count reached");
        return true;
    }
    return false;
}

template<class S, class M, class R>
bool Search<S, M, R>::check_abort_expensive(ThreadState& thread_state) const
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
        size_t nu_simulations = m_nu_simulations.load(memory_order_relaxed);
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
void Search<S, M, R>::check_create_threads()
{
    if (m_nu_threads != m_threads.size())
        create_threads();
}

template<class S, class M, class R>
bool Search<S, M, R>::check_followup(vector<Move>& sequence)
{
    LIBBOARDGAME_UNUSED(sequence);
    return false;
}

template<class S, class M, class R>
bool Search<S, M, R>::check_move_cannot_change(Float count,
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
void Search<S, M, R>::create_threads()
{
    log() << "Creating " << m_nu_threads << " threads\n";
    m_threads.clear();
    auto search_func =
        static_cast<typename Thread::SearchFunc>(
                          bind(&Search::search_loop, this, placeholders::_1));
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
void Search<S, M, R>::dump(ostream& out) const
{
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        out << "Thread state " << i << ":\n";
        get_state(i).dump(out);
    }
}

template<class S, class M, class R>
bool Search<S, M, R>::expand_node(unsigned thread_id, const Node& node,
                                  const Node*& best_child, Float init_val)
{
    typename Tree::NodeExpander expander(thread_id, m_tree, node);
    get_state(thread_id).gen_children(expander, init_val);
    if (! expander.is_tree_full())
    {
        expander.link_children();
        best_child = expander.get_best_child();
        return true;
    }
    return false;
}

template<class S, class M, class R>
double Search<S, M, R>::expected_sim_per_sec() const
{
    return 100.0;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_bias_term_constant() const -> Float
{
    return m_bias_term.get_bias_term_constant();
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_expand_threshold() const -> Float
{
    return m_expand_threshold;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_expand_threshold_incr() const -> Float
{
    return m_expand_threshold_incr;
}

template<class S, class M, class R>
size_t Search<S, M, R>::get_max_nodes(size_t memory)
{
    // Memory is used for 2 trees (m_tree and m_tmp_tree)
    size_t max_nodes = memory / sizeof(Node) / 2;
    // It doesn't make sense to set max_nodes higher than what can be accessed
    // with NodeIndex
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIndex>::max()));
    log() << "Search tree size: 2 x " << max_nodes << " nodes\n";
    return max_nodes;
}

template<class S, class M, class R>
inline size_t Search<S, M, R>::get_nu_simulations() const
{
    return m_nu_simulations;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_root_val() const -> const RootStat&
{
    return m_root_val;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_prune_count_start() const -> Float
{
    return m_prune_count_start;
}

template<class S, class M, class R>
inline bool Search<S, M, R>::get_prune_full_tree() const
{
    return m_prune_full_tree;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_rave_dist_final() const -> Float
{
    return m_rave_dist_final;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_rave_max_parent_count() const -> Float
{
    return m_rave_max_parent_count;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_rave_max_child_count() const -> Float
{
    return m_rave_max_child_count;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_rave_weight() const -> Float
{
    return m_rave_weight;
}

template<class S, class M, class R>
inline bool Search<S, M, R>::get_reuse_subtree() const
{
    return m_reuse_subtree;
}

template<class S, class M, class R>
inline bool Search<S, M, R>::get_reuse_tree() const
{
    return m_reuse_tree;
}

template<class S, class M, class R>
inline S& Search<S, M, R>::get_state(unsigned thread_id)
{
    LIBBOARDGAME_ASSERT(thread_id < m_threads.size());
    return *m_threads[thread_id]->thread_state.state;
}

template<class S, class M, class R>
inline const S& Search<S, M, R>::get_state(unsigned thread_id) const
{
    LIBBOARDGAME_ASSERT(thread_id < m_threads.size());
    return *m_threads[thread_id]->thread_state.state;
}

template<class S, class M, class R>
inline TimeSource& Search<S, M, R>::get_time_source()
{
    LIBBOARDGAME_ASSERT(m_time_source != 0);
    return *m_time_source;
}

template<class S, class M, class R>
size_t Search<S, M, R>::get_tree_memory() const
{
    return m_tree_memory;
}

template<class S, class M, class R>
inline auto Search<S, M, R>::get_tree() const -> const Tree&
{
    return m_tree;
}

template<class S, class M, class R>
void Search<S, M, R>::log_thread(const ThreadState& thread_state,
                                 const string& s) const
{
    ostringstream o;
    o << "[" << thread_state.thread_id << "] " << s;
    log(o.str());
}

template<class S, class M, class R>
void Search<S, M, R>::on_search_iteration(size_t n, const State& state,
                                          const Simulation& simulation)
{
    LIBBOARDGAME_UNUSED(n);
    LIBBOARDGAME_UNUSED(state);
    LIBBOARDGAME_UNUSED(simulation);
    // Default implementation does nothing
}

template<class S, class M, class R>
void Search<S, M, R>::on_start_search()
{
    // Default implementation does nothing
}

template<class S, class M, class R>
void Search<S, M, R>::playout(ThreadState& thread_state)
{
    auto& state = *thread_state.state;
    state.start_playout();
    while (true)
    {
        Move last_good_reply_1 = Move::null();
        Move last_good_reply_2 = Move::null();
        if (SearchParamConst::use_last_good_reply)
        {
            unsigned nu_moves = state.get_nu_moves();
            if (nu_moves > 0)
            {
                Move last_mv = state.get_move(nu_moves - 1).move;
                Move second_last_mv = Move::null();
                if (nu_moves > 1)
                    second_last_mv = state.get_move(nu_moves - 2).move;
                m_last_good_reply.get(state.get_to_play(), last_mv,
                                      second_last_mv, last_good_reply_1,
                                      last_good_reply_2);
            }
        }
        if (! state.gen_and_play_playout_move(last_good_reply_1,
                                              last_good_reply_2))
            break;
    }
}

template<class S, class M, class R>
void Search<S, M, R>::play_in_tree(ThreadState& thread_state, bool& is_terminal)
{
    auto& state = *thread_state.state;
    auto& root = m_tree.get_root();
    auto node = &root;
    m_tree.inc_visit_count(*node);
    thread_state.simulation.nodes.push_back(node);
    is_terminal = false;
    Float expand_threshold = m_expand_threshold;
    while (node->has_children())
    {
        node = select_child(*node);
        m_tree.inc_visit_count(*node);
        thread_state.simulation.nodes.push_back(node);
        state.play_in_tree(node->get_move());
        expand_threshold += m_expand_threshold_incr;
    }
    state.finish_in_tree();
    if (node->get_visit_count() > expand_threshold || node == &root)
    {
        Float init_val = m_init_val[state.get_to_play()].get_mean();
        if (! expand_node(thread_state.thread_id, *node, node, init_val))
            thread_state.is_out_of_mem = true;
        else if (node == nullptr)
            is_terminal = true;
        else
        {
            thread_state.simulation.nodes.push_back(node);
            state.play_expanded_child(node->get_move());
        }
    }
}

template<class S, class M, class R>
void Search<S, M, R>::write_info(ostream& out) const
{
    auto& root = m_tree.get_root();
    if (m_threads.empty())
        return;
    auto& thread_state = m_threads[0]->thread_state;
    if (thread_state.nu_simulations == 0)
    {
        out << "No simulations in thread 0\n";
        return;
    }
    FmtSaver saver(out);
    out << fixed << setprecision(2) << "Val: " << root.get_value()
        << setprecision(0) << ", ValCnt: " << root.get_value_count()
        << ", VstCnt: " << root.get_visit_count()
        << ", Sim: " << m_nu_simulations;
    auto child = select_child_final(root);
    if (child != nullptr && root.get_visit_count() > 0)
        out << setprecision(1) << ", Chld: "
            << (100 * child->get_visit_count() / root.get_visit_count())
            << '%';
    out << ", Nds: " << m_tree.get_nu_nodes()
        << "\nTm: " << time_to_string(m_last_time)
        << setprecision(0) << ", Sim/s: "
        << (double(m_nu_simulations) / m_last_time)
        << ", Len: " << thread_state.stat_len.to_string(true, 1, true)
        << "\nDp: " << thread_state.stat_in_tree_len.to_string(true, 1, true)
        << "\n";
}

template<class S, class M, class R>
void Search<S, M, R>::write_info_ext(ostream& out) const
{
    LIBBOARDGAME_UNUSED(out);
}

template<class S, class M, class R>
bool Search<S, M, R>::prune(TimeSource& time_source, double time,
                            double max_time, Float prune_min_count,
                            Float& new_prune_min_count)
{
    Timer timer(time_source);
    TimeIntervalChecker interval_checker(time_source, max_time);
    if (m_deterministic)
        interval_checker.set_deterministic(1000000);
    log() << "Pruning count " << prune_min_count << " (at tm " << time << ")\n";
    m_tmp_tree.clear(m_tree.get_root().get_value());
    if (! m_tree.copy_subtree(m_tmp_tree, m_tmp_tree.get_root(),
                              m_tree.get_root(), prune_min_count, true,
                              &interval_checker))
    {
        log("Pruning aborted");
        return false;
    }
    int percent = int(m_tmp_tree.get_nu_nodes() * 100 / m_tree.get_nu_nodes());
    log() << "Pruned size: " << m_tmp_tree.get_nu_nodes() << " (" << percent
          << "%, tm=" << timer() << ")\n";
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
void Search<S, M, R>::restore_root_from_children(Tree& tree, const Node& root)
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
bool Search<S, M, R>::search(Move& mv, Float max_count, Float min_simulations,
                             double max_time, TimeSource& time_source,
                             bool always_search)
{
    check_create_threads();
    if (max_count > 0)
        // A fixed number of simulations means that no time limit is used, but
        // max_time is still used at some places in the code, so we set it to
        // infinity
        max_time = numeric_limits<double>::max();
    m_nu_players = get_nu_players();
    bool clear_tree = true;
    bool is_followup = check_followup(m_followup_sequence);
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
    if (((m_reuse_subtree && is_followup) || (m_reuse_tree && is_same)))
    {
        size_t tree_nodes = m_tree.get_nu_nodes();
        if (m_followup_sequence.empty())
        {
            if (tree_nodes > 1)
                log() << "Reusing all " << tree_nodes << "nodes (count="
                      << m_tree.get_root().get_visit_count() << ")\n";
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
                bool aborted = ! m_tree.extract_subtree(m_tmp_tree, *node, true,
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
                    {
                        FmtSaver saver(log());
                        log() << "Reusing " << tmp_tree_nodes << " nodes ("
                              << fixed << setprecision(1) << percent
                              << "% tm=" << setprecision(4) << time << ")\n";
                    }
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
    on_start_search();
    m_player = get_player();
    for (PlayerInt i = 0; i < m_nu_players; ++i)
        m_root_val[i].clear();
    if (SearchParamConst::use_last_good_reply && ! is_followup)
        m_last_good_reply.init(m_nu_players);
    for (unsigned i = 0; i < m_threads.size(); ++i)
    {
        auto& thread_state = m_threads[i]->thread_state;
        thread_state.nu_simulations = 0;
        thread_state.stat_len.clear();
        thread_state.stat_in_tree_len.clear();
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

    while (true)
    {
        for (unsigned i = 1; i < nu_threads; ++i)
            m_threads[i]->start_search();
        search_loop(m_threads[0]->thread_state);
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
    write_info(log());
    bool result = select_move(mv);
    m_time_source = nullptr;
    return result;
}

template<class S, class M, class R>
void Search<S, M, R>::search_loop(ThreadState& thread_state)
{
    auto& state = *thread_state.state;
    double time_interval = 0.1;
    if (m_max_count == 0 && m_max_time < 1)
        time_interval = 0.1 * m_max_time;
    IntervalChecker expensive_abort_checker(*m_time_source, time_interval,
                                            bind(&Search::check_abort_expensive,
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
        Float root_count = m_tree.get_root().get_visit_count();
        size_t nu_simulations = m_nu_simulations.fetch_add(1);
        if (root_count > 0 && nu_simulations > m_min_simulations
            && (check_abort(thread_state) || expensive_abort_checker()))
        {
            m_nu_simulations.fetch_add(-1);
            break;
        }
        ++thread_state.nu_simulations;
        auto& simulation = thread_state.simulation;
        simulation.nodes.clear();
        state.start_simulation(nu_simulations);
        bool is_terminal;
        play_in_tree(thread_state, is_terminal);
        if (thread_state.is_out_of_mem)
            return;
        thread_state.stat_in_tree_len.add(double(state.get_nu_moves()));
        if (! is_terminal)
        {
            playout(thread_state);
            simulation.eval = state.evaluate_playout();
        }
        else
            simulation.eval = state.evaluate_terminal();
        thread_state.stat_len.add(double(state.get_nu_moves()));
        update_values(thread_state);
        if (SearchParamConst::rave)
            update_rave_values(thread_state);
        if (SearchParamConst::use_last_good_reply)
            update_last_good_reply(thread_state);
        on_search_iteration(nu_simulations, *thread_state.state,
                            thread_state.simulation);
    }
}

template<class S, class M, class R>
auto Search<S, M, R>::select_child(const Node& node) -> const Node*
{
    LIBBOARDGAME_ASSERT(node.has_children());
    auto node_count = node.get_visit_count();
    if (log_move_selection)
        log() << "Search::select_child:\n"
              << "vc=" << node_count << '\n'
              << "v=" << node.get_value() << '\n';
    const Node* best_child = nullptr;
    Float best_value = -numeric_limits<Float>::max();
    m_bias_term.start_iteration(node_count);
    for (ChildIterator i(m_tree, node); i; ++i)
    {
        auto child_count = i->get_visit_count();
        auto bias = m_bias_term.get(child_count);
        Float value = i->get_value() + bias;
        if (log_move_selection)
            log() << get_move_string(i->get_move())
                  << " | vc=" << child_count << " v=" << i->get_value()
                  << " e=" << bias << " | " << value << '\n';
        if (value > best_value)
        {
            best_value = value;
            best_child = &(*i);
        }
    }
    if (log_move_selection)
        log() << "Selected: " << get_move_string(best_child->get_move())
              << '\n';
    return best_child;
}

template<class S, class M, class R>
auto Search<S, M, R>::select_child_final(const Node& node,
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
bool Search<S, M, R>::select_move(Move& mv, const vector<Move>* exclude_moves)
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
void Search<S, M, R>::set_bias_term_constant(Float c)
{
    m_bias_term.set_bias_term_constant(c);
}

template<class S, class M, class R>
void Search<S, M, R>::set_callback(function<void(double, double)> callback)
{
    m_callback = callback;
}

template<class S, class M, class R>
void Search<S, M, R>::set_expand_threshold(Float n)
{
    m_expand_threshold = n;
}

template<class S, class M, class R>
void Search<S, M, R>::set_expand_threshold_incr(Float n)
{
    m_expand_threshold_incr = n;
}

template<class S, class M, class R>
void Search<S, M, R>::set_deterministic()
{
    m_deterministic = true;
}

template<class S, class M, class R>
void Search<S, M, R>::set_prune_count_start(Float n)
{
    m_prune_count_start = n;
}

template<class S, class M, class R>
void Search<S, M, R>::set_prune_full_tree(bool enable)
{
    m_prune_full_tree = enable;
}

template<class S, class M, class R>
void Search<S, M, R>::set_rave_dist_final(Float v)
{
    m_rave_dist_final = v;
}

template<class S, class M, class R>
void Search<S, M, R>::set_rave_max_parent_count(Float n)
{
    m_rave_max_parent_count = n;
}

template<class S, class M, class R>
void Search<S, M, R>::set_rave_max_child_count(Float n)
{
    m_rave_max_child_count = n;
}

template<class S, class M, class R>
void Search<S, M, R>::set_rave_weight(Float v)
{
    m_rave_weight = v;
}

template<class S, class M, class R>
void Search<S, M, R>::set_reuse_subtree(bool enable)
{
    m_reuse_subtree = enable;
}

template<class S, class M, class R>
void Search<S, M, R>::set_reuse_tree(bool enable)
{
    m_reuse_tree = enable;
}

template<class S, class M, class R>
void Search<S, M, R>::set_tree_memory(size_t memory)
{
    m_tree_memory = memory;
    m_max_nodes = get_max_nodes(memory);
    m_tree.set_max_nodes(m_max_nodes);
    m_tmp_tree.set_max_nodes(m_max_nodes);
}

template<class S, class M, class R>
void Search<S, M, R>::update_last_good_reply(ThreadState& thread_state)
{
    const auto& state = *thread_state.state;
    const auto& eval = thread_state.simulation.eval;
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
    unsigned nu_moves = state.get_nu_moves();
    if (nu_moves >= 2)
    {
        // Iterate backwards to store first reply if move was played more than
        // once
        PlayerMove reply = state.get_move(nu_moves - 1);
        PlayerMove last = state.get_move(nu_moves - 2);
        PlayerMove second_last;
        if (nu_moves >= 3)
            for (unsigned i = nu_moves - 1; i >= 2; --i)
            {
                second_last = state.get_move(i - 2);
                if (is_winner[reply.player])
                    m_last_good_reply.store(reply.player, last.move,
                                            second_last.move, reply.move);
                else
                    m_last_good_reply.forget(reply.player, last.move,
                                             second_last.move, reply.move);
                reply = last;
                last = second_last;
            }
        if (is_winner[reply.player])
            m_last_good_reply.store(reply.player, last.move, reply.move);
        else
            m_last_good_reply.forget(reply.player, last.move, reply.move);
    }
}

template<class S, class M, class R>
void Search<S, M, R>::update_rave_values(ThreadState& thread_state)
{
    const auto& state = *thread_state.state;
    unsigned nu_moves = state.get_nu_moves();
    if (nu_moves == 0)
        return;
    auto& was_played = thread_state.was_played;
    auto& first_play = thread_state.first_play;
    auto& nodes = thread_state.simulation.nodes;
    unsigned nu_nodes = static_cast<unsigned>(nodes.size());
    unsigned i = nu_moves - 1;
    LIBBOARDGAME_ASSERT(nu_nodes > 1);
    for ( ; i >= nu_nodes - 1; --i)
    {
        auto mv = state.get_move(i);
        if (! state.skip_rave(mv.move))
        {
            was_played[mv.player].set(mv.move);
            first_play[mv.player][mv.move.to_int()] = i;
        }
    }
    while (true)
    {
        auto mv = state.get_move(i);
        update_rave_values(thread_state, i, mv.player);
        if (i == 0)
            break;
        if (! state.skip_rave(mv.move))
        {
            was_played[mv.player].set(mv.move);
            first_play[mv.player][mv.move.to_int()] = i;
        }
        --i;
    }
    // Reset was_played
    for (unsigned i = 1; i < nu_moves; ++i)
    {
        auto mv = state.get_move(i);
        was_played[mv.player].clear_word(mv.move);
    }
}

template<class S, class M, class R>
void Search<S, M, R>::update_rave_values(ThreadState& thread_state,
                                         unsigned i, PlayerInt player)
{
    auto& nodes = thread_state.simulation.nodes;
    LIBBOARDGAME_ASSERT(i < nodes.size());
    const auto node = nodes[i];
    LIBBOARDGAME_ASSERT(node->has_children());
    if (node->get_visit_count() > m_rave_max_parent_count)
        return;
    const auto& state = *thread_state.state;
    auto& was_played = thread_state.was_played;
    auto& first_play = thread_state.first_play;
    unsigned len = state.get_nu_moves();
    Float dist_weight_factor = (1 - m_rave_dist_final) / Float(len - i);
    for (ChildIterator it(m_tree, *node); it; ++it)
    {
        auto mv = it->get_move();
        if (! was_played[player][mv]
            || it->get_visit_count() > m_rave_max_child_count)
            continue;
        unsigned first = first_play[player][mv.to_int()];
        LIBBOARDGAME_ASSERT(first > i);
        if (SearchParamConst::rave_check_same)
        {
            bool other_played_same = false;
            for (PlayerInt j = 0; j < m_nu_players; ++j)
                if (j != player && was_played[j][mv])
                {
                    unsigned first_other = first_play[j][mv.to_int()];
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
}

template<class S, class M, class R>
void Search<S, M, R>::update_values(ThreadState& thread_state)
{
    const auto& state = *thread_state.state;
    auto& nodes = thread_state.simulation.nodes;
    const auto& eval = thread_state.simulation.eval;
    m_tree.add_value(m_tree.get_root(), eval[m_player]);
    unsigned nu_nodes = static_cast<unsigned>(nodes.size());
    for (unsigned i = 1; i < nu_nodes; ++i)
    {
        auto& node = *nodes[i];
        auto mv = state.get_move(i - 1);
        m_tree.add_value(node, eval[mv.player]);
    }
    for (PlayerInt i = 0; i < m_nu_players; ++i)
    {
        m_root_val[i].add(eval[i]);
        m_init_val[i].add(eval[i]);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_SEARCH_H
