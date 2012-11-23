//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Search.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_SEARCH_H
#define LIBBOARDGAME_MCTS_SEARCH_H

#include <algorithm>
#include <array>
#include <functional>
#include <boost/format.hpp>
#include "BiasTerm.h"
#include "ReplyTable.h"
#include "Tree.h"
#include "TreeUtil.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/IntervalChecker.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Parameters.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/StringUtil.h"
#include "libboardgame_util/TimeIntervalChecker.h"
#include "libboardgame_util/Timer.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_mcts {

using namespace std;
using boost::format;
using libboardgame_mcts::tree_util::find_node;
using libboardgame_util::get_abort;
using libboardgame_util::log;
using libboardgame_util::IntervalChecker;
using libboardgame_util::Parameters;
using libboardgame_util::StatisticsBase;
using libboardgame_util::StatisticsExt;
using libboardgame_util::Timer;
using libboardgame_util::TimeIntervalChecker;
using libboardgame_util::TimeSource;
using libboardgame_util::string_util::time_to_string;
using libboardgame_util::string_util::to_string;

//-----------------------------------------------------------------------------

/** Game-independent Monte-Carlo tree search.
    For best performance, the game-dependent functionality is added to this
    class by template parameters, like the type M representing a move (which
    must be convertible to an integer by providing M::to_int() and M::range)
    and the state S of a simulation that provides functions for move
    generation, evaluation of terminal positions, etc. (the state should be
    thread-safe to support multiple states if multi-threading will be
    implemented in the future). Additionally, you need to derive from this
    class and implement some pure virtual functions like get_move_string().
    @tparam S The state of a simulation
    @tparam M The move type
    @tparam P The maximum number of players */
template<class S, class M, unsigned P>
class Search
{
public:
    typedef S State;

    typedef M Move;

    static const unsigned max_players = P;

    typedef libboardgame_mcts::Node<M> Node;

    typedef libboardgame_mcts::ChildIterator<M> ChildIterator;

    typedef libboardgame_mcts::Tree<M> Tree;

    typedef libboardgame_mcts::PlayerMove<M> PlayerMove;

    static const bool log_move_selection = false;

    /** Constructor.
        @param state A prototype for the search state used by one thread.
        @param memory The memory to be used for (all) the search trees. If
        zero, a default value will be used. */
    Search(const State& state, size_t memory = 0);

    virtual ~Search() throw();


    /** @name Pure virtual functions */
    // @{

    /** Get string representation of a move.
        This function is not a member function of the move type M because
        the move type may be a lightweight class that needs a context to
        be converted to a string. */
    virtual string get_move_string(Move mv) const = 0;

    /** Get the current number of players. */
    virtual unsigned get_nu_players() const = 0;

    /** Get player to play at root node of the search. */
    virtual unsigned get_player() const = 0;

    /** An evaluation value representing a 50% winning probability. */
    virtual Float get_tie_value() const = 0;

    // @} // @name


    /** @name Virtual functions */
    // @{

    /** Get a copy of the search parameters that determine the reusability of
        (a part of) the tree between searches.
        Usually, one should not try to use a subtree from the last search as an
        init tree for the next search (should the current state be a follow-up
        state of the last search) if any of those parameters were changed.
        @see check_followup() */
    virtual Parameters get_reuse_param() const;

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
        caches from the last search (e.g. Last-Good-Reply heuristic).
        @see get_reuse_param() */
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

    void set_expand_threshold(Float n);

    Float get_expand_threshold() const;

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

    /** Use @ref libboardgame_doc_rave.
        @note The implementation of RAVE slightly differs from the one
        described in the original paper: The value of a move is computed
        as the weighted average of move value and RAVE value (without
        exploration terms) and then only one exploration term is added.
        The rationale is that the UCT-like exploration term based on the
        RAVE counts in the paper is probably not well-defined, since the
        RAVE count does not only depend on decisions made by the player in
        the current node, so it is not a UCB bandit problem. */
    void set_rave(bool enable);

    bool get_rave() const;

    /** Do not update RAVE values if the same move was played first by the
        other player.
        This improves RAVE performance in Go, where moves at the same point
        indicate that something was captured there, so the position has
        changed so significantly that the RAVE value of this move should not be
        updated to earlier positions. Default is false. */
    void set_rave_check_same(bool enable);

    /** Set the equivalence parameter in the @ref libboardgame_doc_rave
        formula. */
    void set_rave_equivalence(Float value);

    Float get_rave_equivalence() const;

    /** Enable weighting of @ref libboardgame_doc_rave updates.
        The weight decreases linearly from the start to the end of a
        simulation. */
    void set_weight_rave_updates(bool enable);

    bool get_weight_rave_updates() const;

    /** Enable Last-Good-Reply heuristic.
        Uses LGRF-1 to remember good reply moves and passes them to the
        gen_and_play_playout_move() function of the state, which needs to check
        if the reply is legal in the current position and decide when and if
        to use it for the playout move. Can only be use in games, in which the
        sequence of players does not change.
        See Baier, Drake: The Power of Forgetting: Improving the Last-Good-Reply
        Policy in Monte-Carlo Go. (2010)
        http://webdisk.lclark.edu/drake/publications/baier-drake-ieee-2010.pdf
        @see ReplyTable */
    void set_last_good_reply(bool enable);

    bool get_last_good_reply() const;

    /** See get_reuse_param() */
    const Parameters& get_last_reuse_param() const;

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
    bool search(Move& mv, Float max_count, size_t min_simulations,
                double max_time, TimeSource& time_source,
                bool always_search = true);

    const Tree& get_tree() const;

    void dump(ostream& out) const;

    size_t get_nu_simulations() const;

    /** Select the move to play.
        Uses select_child_final() on the root node. */
    bool select_move(Move& mv, const vector<Move>* exclude_moves = 0) const;

    /** Select the best child of a node after the search.
        Selects child with highest visit count; the value is used as a
        tie-breaker for equal counts (important at very low number of
        simulations, e.g. all children have count 1 or 0). */
    const Node* select_child_final(const Node& node,
                                   const vector<Move>* exclude_moves = 0) const;

    State& get_state();

    const State& get_state() const;

    /** Set a callback function that informs the caller about the
        estimated time left.
        The callback function will be called about every 0.1s. The arguments
        of the callback function are: elapsed time, estimated remaining time. */
    void set_callback(function<void(double, double)> callback);

    /** Get mean evaluation for all players at root node. */
    const array<StatisticsBase<Float>,P>& get_root_val() const;

    /** Get the value of the root position.
        If the count of the root node is less than the count of the child
        with the highest count, the value of this child will be returned,
        otherwise the value of the root node (this can happen because the root
        values are cleared when reusing a subtree from a previous search). */
    Float get_value() const;

protected:
    struct Simulation
    {
        vector<const Node*> m_nodes;

        ~Simulation() throw();
    };

    virtual void on_start_search();

    virtual void on_search_iteration(size_t n, const Simulation& simulation);

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

    Float m_expand_threshold;

    bool m_deterministic;

    bool m_reuse_subtree;

    bool m_reuse_tree;

    bool m_prune_full_tree;

    bool m_rave;

    bool m_rave_check_same;

    bool m_weight_rave_updates;

    bool m_use_last_good_reply;

    /** Player to play at the root node of the search. */
    unsigned m_player;

    /** Currently always 1 (multi-threaded search not yet implemented). */
    unsigned m_nu_threads;

    /** Time of last search. */
    double m_last_time;

    Float m_prune_count_start;

    Float m_rave_equivalence;

    /** Maximum simulations of current search. */
    Float m_max_count;

    /** Count of root node of reused subtree if its values had to be cleared.
        If a subtree (not the full tree) is reused because the current position
        is a follow-up position of the last search, the root values need to be
        cleared because the root value has a different meaning than inner node
        values (position value vs. move values). We remember the count of the
        root node before clearing for the max count abort condition. */
    Float m_reuse_count;

    size_t m_tree_memory;

    size_t m_max_nodes;

    /** Number simulations of current search. */
    size_t m_nu_simulations;

    /** Maximum time of current search. */
    double m_max_time;

    TimeSource* m_time_source;

    BiasTerm m_bias_term;

    Timer m_timer;

    /** Game dependent data per thread. */
    State m_state;

    Simulation m_simulation;

    /** Local variable for update_rave_values().
        Stores the first time a move was played for each player.
        Reused for efficiency. */
    array<array<unsigned,Move::range>,max_players> m_first_play;

    Tree m_tree;

    Tree m_tmp_tree;

    AssertionHandler m_assertion_handler;

    /** See get_reuse_param() */
    Parameters m_last_reuse_param;

    StatisticsExt<> m_stat_len;

    StatisticsExt<> m_stat_in_tree_len;

    /** See get_root_val(). */
    array<StatisticsBase<Float>,P> m_root_val;

    /** Current position value estimate for prior knowledge initialization.
        Derived form root values of last search and updated with root values
        of current search. */
    array<StatisticsBase<Float>,P> m_init_val;

    function<void(double, double)> m_callback;

    ReplyTable<S,M,P> m_reply_table;

    vector<Move> m_followup_sequence;

    static size_t get_max_nodes(size_t memory);

    bool check_abort() const;

    bool check_abort_expensive() const;

    bool check_move_cannot_change(Float count, Float remaining) const;

    bool expand_node(unsigned thread_id, const Node& node,
                     const Node*& best_child, Float init_val);

    LIBBOARDGAME_FLATTEN void playout();

    void play_in_tree(unsigned thread_id, bool& is_out_of_memory,
                      bool& is_terminal);

    bool prune(TimeSource& time_source, double time, double max_time,
               Float prune_min_count, Float& new_prune_min_count);

    const Node* select_child(const Node& node);

    void update_last_good_reply(const array<Float,max_players>& eval);

    void update_rave_values(const array<Float,max_players>& eval);

    void update_rave_values(const array<Float,max_players>&, unsigned i,
                            unsigned player);

    void update_values(const array<Float,max_players>& eval);
};

template<class S, class M, unsigned P>
Search<S,M,P>::AssertionHandler::AssertionHandler(const Search& search)
    : m_search(search)
{
}

template<class S, class M, unsigned P>
Search<S,M,P>::AssertionHandler::~AssertionHandler() throw()
{
}

template<class S, class M, unsigned P>
void Search<S,M,P>::AssertionHandler::run()
{
    m_search.dump(log());
}

template<class S, class M, unsigned P>
Search<S,M,P>::Simulation::~Simulation() throw()
{
}

template<class S, class M, unsigned P>
Search<S,M,P>::Search(const State& state, size_t memory)
    : m_expand_threshold(0),
      m_deterministic(false),
      m_reuse_subtree(true),
      m_reuse_tree(false),
      m_prune_full_tree(true),
      m_rave(false),
      m_rave_check_same(false),
      m_weight_rave_updates(true),
      m_use_last_good_reply(false),
      m_nu_threads(1),
      m_prune_count_start(16),
      m_rave_equivalence(1000),
      m_tree_memory(memory == 0 ? 256000000 : memory),
      m_max_nodes(get_max_nodes(m_tree_memory)),
      m_bias_term(0),
      m_state(state),
      m_tree(m_max_nodes, m_nu_threads),
      m_tmp_tree(m_max_nodes, m_nu_threads),
      m_assertion_handler(*this)
{
    for (unsigned i = 0; i < max_players; ++i)
        m_first_play[i].fill(numeric_limits<unsigned>::max());
}

template<class S, class M, unsigned P>
Search<S,M,P>::~Search() throw()
{
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::check_abort() const
{
    Float count = m_tree.get_root().get_count() + m_reuse_count;
    static_assert(numeric_limits<Float>::radix == 2,
                  "libboardgame_mcts::Float must have radix 2");
    if (count == (size_t(1) << numeric_limits<Float>::digits) - 1)
    {
        log("Maximum count supported by floating type reached");
        return true;
    }
    if (m_max_count > 0 && count >= m_max_count)
    {
        log("Maximum count reached");
        return true;
    }
    return false;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::check_abort_expensive() const
{
    if (get_abort())
    {
        log("Search aborted");
        return true;
    }
    Float count = m_tree.get_root().get_count() + m_reuse_count;
    double time = m_timer();
    if (! m_deterministic && time < 0.1)
        // Simulations per second might be inaccurate for very small times
        return false;
    double simulations_per_sec;
    if (time == 0)
        simulations_per_sec = expected_sim_per_sec();
    else
        simulations_per_sec = double(m_nu_simulations) / time;
    double remaining_time;
    Float remaining_simulations;
    if (m_max_count == 0)
    {
        if (time > m_max_time)
        {
            log("Maximum time reached");
            return true;
        }
        remaining_time = m_max_time - time;
        remaining_simulations = Float(remaining_time * simulations_per_sec);
    }
    else
    {
        remaining_simulations = m_max_count - count;
        remaining_time = remaining_simulations / simulations_per_sec;
    }
    if (m_callback)
        m_callback(time, remaining_time);
    if (check_move_cannot_change(count, remaining_simulations))
    {
        log("Move cannot change anymore");
        return true;
    }
    return false;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::check_move_cannot_change(Float count,
                                             Float remaining) const
{
    if (remaining > count)
        return false;
    Float max_count = 0;
    Float second_max_count = 0;
    for (ChildIterator i(m_tree, m_tree.get_root()); i; ++i)
    {
        Float count = i->get_count();
        if (count > max_count)
        {
            second_max_count = max_count;
            max_count = count;
        }
    }
    return (max_count > second_max_count + remaining);
}

template<class S, class M, unsigned P>
void Search<S,M,P>::dump(ostream& out) const
{
    m_state.dump(out);
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::expand_node(unsigned thread_id, const Node& node,
                                const Node*& best_child, Float init_val)
{
    typename Tree::NodeExpander expander(thread_id, m_tree, node);
    m_state.gen_children(expander, init_val);
    if (! expander.is_tree_full())
    {
        expander.link_children();
        best_child = expander.get_best_child();
        return true;
    }
    return false;
}

template<class S, class M, unsigned P>
double Search<S,M,P>::expected_sim_per_sec() const
{
    return 100.0;
}

template<class S, class M, unsigned P>
Float Search<S,M,P>::get_bias_term_constant() const
{
    return m_bias_term.get_bias_term_constant();
}

template<class S, class M, unsigned P>
Float Search<S,M,P>::get_expand_threshold() const
{
    return m_expand_threshold;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_last_good_reply() const
{
    return m_use_last_good_reply;
}

template<class S, class M, unsigned P>
const Parameters& Search<S,M,P>::get_last_reuse_param() const
{
    return m_last_reuse_param;
}

template<class S, class M, unsigned P>
size_t Search<S,M,P>::get_max_nodes(size_t memory)
{
    // Memory is used for 2 trees (m_tree and m_tmp_tree)
    size_t max_nodes = memory / sizeof(Node) / 2;
    // It doesn't make sense to set max_nodes higher than what can be accessed
    // with NodeIndex
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIndex>::max()));
    log(format("Search tree size: 2 x %1% nodes") % max_nodes);
    return max_nodes;
}

template<class S, class M, unsigned P>
size_t Search<S,M,P>::get_nu_simulations() const
{
    return m_nu_simulations;
}

template<class S, class M, unsigned P>
inline const array<StatisticsBase<Float>,P>& Search<S,M,P>::get_root_val() const
{
    return m_root_val;
}

template<class S, class M, unsigned P>
Parameters Search<S,M,P>::get_reuse_param() const
{
    Parameters p;
    p.create<bool>("rave", m_rave);
    p.create<bool>("weight_rave_updates", m_weight_rave_updates);
    p.create<Float>("rave_equivalence", m_rave_equivalence);
    return p;
}

template<class S, class M, unsigned P>
Float Search<S,M,P>::get_prune_count_start() const
{
    return m_prune_count_start;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_prune_full_tree() const
{
    return m_prune_full_tree;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_rave() const
{
    return m_rave;
}

template<class S, class M, unsigned P>
Float Search<S,M,P>::get_rave_equivalence() const
{
    return m_rave_equivalence;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_reuse_subtree() const
{
    return m_reuse_subtree;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_reuse_tree() const
{
    return m_reuse_tree;
}

template<class S, class M, unsigned P>
inline typename Search<S,M,P>::State& Search<S,M,P>::get_state()
{
    return m_state;
}

template<class S, class M, unsigned P>
inline const typename Search<S,M,P>::State& Search<S,M,P>::get_state()
    const
{
    return m_state;
}

template<class S, class M, unsigned P>
inline TimeSource& Search<S,M,P>::get_time_source()
{
    LIBBOARDGAME_ASSERT(m_time_source != 0);
    return *m_time_source;
}

template<class S, class M, unsigned P>
size_t Search<S,M,P>::get_tree_memory() const
{
    return m_tree_memory;
}

template<class S, class M, unsigned P>
inline const typename Search<S,M,P>::Tree& Search<S,M,P>::get_tree()
    const
{
    return m_tree;
}

template<class S, class M, unsigned P>
Float Search<S,M,P>::get_value() const
{
    const Node& root = m_tree.get_root();
    Float root_count = root.get_count();
    const Node* child = select_child_final(root);
    if (child != 0 && child->get_count() > root_count)
        return child->get_value();
    else if (root_count > 0)
        return root.get_value();
    else
        return get_tie_value();
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::get_weight_rave_updates() const
{
    return m_weight_rave_updates;
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::check_followup(vector<Move>& sequence)
{
    LIBBOARDGAME_UNUSED(sequence);
    return false;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::on_search_iteration(size_t n, const Simulation& simulation)
{
    LIBBOARDGAME_UNUSED(n);
    LIBBOARDGAME_UNUSED(simulation);
    // Default implementation does nothing
}

template<class S, class M, unsigned P>
void Search<S,M,P>::on_start_search()
{
    // Default implementation does nothing
}

template<class S, class M, unsigned P>
void Search<S,M,P>::playout()
{
    m_state.start_playout();
    while (true)
    {
        Move last_good_reply = Move::null();
        if (m_use_last_good_reply)
        {
            unsigned nu_moves = m_state.get_nu_moves();
            if (nu_moves > 0)
            {
                PlayerMove last_move = m_state.get_move(nu_moves - 1);
                last_good_reply = m_reply_table.get_reply(last_move);
            }
        }
        if (! m_state.gen_and_play_playout_move(last_good_reply))
            break;
    }
}

template<class S, class M, unsigned P>
void Search<S,M,P>::play_in_tree(unsigned thread_id, bool& is_out_of_memory,
                                 bool& is_terminal)
{
    const Node& root = m_tree.get_root();
    const Node* node = &root;
    is_out_of_memory = false;
    is_terminal = false;
    while (node->has_children())
    {
        node = select_child(*node);
        m_simulation.m_nodes.push_back(node);
        m_state.play_in_tree(node->get_move());
    }
    m_state.finish_in_tree();
    if (node->get_count() >= m_expand_threshold || node == &root)
    {
        Float init_val = m_init_val[m_state.get_to_play()].get_mean();
        if (! expand_node(thread_id, *node, node, init_val))
            is_out_of_memory = true;
        else if (node == 0)
            is_terminal = true;
        else
        {
            m_simulation.m_nodes.push_back(node);
            m_state.play_expanded_child(node->get_move());
        }
    }
}

template<class S, class M, unsigned P>
void Search<S,M,P>::write_info(ostream& out) const
{
    const Node& root = m_tree.get_root();
    Float count = root.get_count();
    if (m_nu_simulations == 0 || count == 0)
        return;
    out << format(
             "Val: %.2f, Cnt: %.0f, ReuseCnt: %.0f, Sim: %i, Nds: %i, Tm: %s\n"
             "Sim/s: %.0f, Len: %s, Dp: %s\n")
        % get_value() % count % m_reuse_count % m_nu_simulations
        % m_tree.get_nu_nodes() % time_to_string(m_last_time)
        % (double(m_nu_simulations) / m_last_time)
        % m_stat_len.to_string(true, 1, true)
        % m_stat_in_tree_len.to_string(true, 1, true);
}

template<class S, class M, unsigned P>
void Search<S,M,P>::write_info_ext(ostream& out) const
{
    LIBBOARDGAME_UNUSED(out);
}

template<class S, class M, unsigned P>
bool Search<S,M,P>::prune(TimeSource& time_source, double time,
                          double max_time, Float prune_min_count,
                          Float& new_prune_min_count)
{
    Timer timer(time_source);
    TimeIntervalChecker interval_checker(time_source, max_time);
    if (m_deterministic)
        interval_checker.set_deterministic(1000000);
    log(format("Pruning count %1% (at time %2%)") % prune_min_count % time);
    m_tmp_tree.clear(m_tree.get_root().get_value());
    if (! m_tree.copy_subtree(m_tmp_tree, m_tmp_tree.get_root(),
                              m_tree.get_root(), prune_min_count, true,
                              &interval_checker))
    {
        log("Pruning aborted");
        return false;
    }
    int percent = int(m_tmp_tree.get_nu_nodes() * 100 / m_tree.get_nu_nodes());
    log(format("Pruned size: %1% (%2%%%, time=%3%)")
               % m_tmp_tree.get_nu_nodes() % percent % timer());
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

template<class S, class M, unsigned P>
bool Search<S,M,P>::search(Move& mv, Float max_count, size_t min_simulations,
                           double max_time, TimeSource& time_source,
                           bool always_search)
{
    if (max_count > 0)
        // A fixed number of simulations means that no time limit is used, but
        // max_time is still used at some places in the code, so we set it to
        // infinity
        max_time = numeric_limits<double>::max();
    unsigned nu_players = get_nu_players();
    bool clear_tree = true;
    bool is_followup = check_followup(m_followup_sequence);
    bool is_same = false;
    if (is_followup && m_followup_sequence.empty())
    {
        is_same = true;
        is_followup = false;
    }
    for (unsigned i = 0; i < nu_players; ++i)
    {
        m_init_val[i].clear();
        m_init_val[i].add(get_tie_value());
    }
    if (is_same || (is_followup && m_followup_sequence.size() <= nu_players))
        for (unsigned i = 0; i < nu_players; ++i)
            if (m_root_val[i].get_count() > 0)
                m_init_val[i] = m_root_val[i];
    m_reuse_count = 0;
    if (((m_reuse_subtree && is_followup) || (m_reuse_tree && is_same))
        && get_reuse_param() == get_last_reuse_param())
    {
        size_t tree_nodes = m_tree.get_nu_nodes();
        if (m_followup_sequence.empty())
        {
            if (tree_nodes > 1)
                log(format("Reusing all %1% nodes (count=%2%)")
                    % tree_nodes % m_tree.get_root().get_count());
        }
        else
        {
            Timer timer(time_source);
            m_tmp_tree.clear(get_tie_value());
            const Node* node = find_node(m_tree, m_followup_sequence);
            if (node != 0)
            {
                TimeIntervalChecker interval_checker(time_source, max_time);
                if (m_deterministic)
                    interval_checker.set_deterministic(1000000);
                bool aborted = ! m_tree.extract_subtree(m_tmp_tree, *node, true,
                                                        &interval_checker);
                const Node& tmp_tree_root = m_tmp_tree.get_root();
                // The values of root nodes have a different meaning than the
                // values of inner nodes (position value vs. move values) so
                // we might have to discard them
                if (! is_same)
                {
                    m_reuse_count = tmp_tree_root.get_count();
                    m_tmp_tree.clear_root_value(get_tie_value());
                }
                if (aborted && ! always_search)
                    return false;
                size_t tmp_tree_nodes = m_tmp_tree.get_nu_nodes();
                if (tree_nodes > 1 && tmp_tree_nodes > 1)
                {
                    double time = timer();
                    double reuse = double(tmp_tree_nodes) / double(tree_nodes);
                    double percent = 100 * reuse;
                    log(format("Reusing %i nodes (%.1f%% count=%i time=%f)")
                        % tmp_tree_nodes % percent % tmp_tree_root.get_count()
                        % time);
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

    m_last_reuse_param = get_reuse_param();
    m_timer.reset(time_source);
    m_time_source = &time_source;
    on_start_search();
    // Currently always 0 (multi-threaded search not yet implemented)
    unsigned thread_id = 0;
    m_player = get_player();
    m_stat_len.clear();
    m_stat_in_tree_len.clear();
    for (unsigned i = 0; i < nu_players; ++i)
        m_root_val[i].clear();
    if (m_use_last_good_reply && ! is_followup)
        m_reply_table.init(nu_players);
    m_state.start_search();
    m_max_count = max_count;
    m_nu_simulations = 0;
    Float prune_min_count = get_prune_count_start();
    m_max_time = max_time;
    double time_interval;
    if (max_count > 0)
        time_interval = 0.1;
    else
        time_interval = max_time > 1 ? 0.1 : 0.1 * max_time;
    IntervalChecker expensive_abort_checker(time_source, time_interval,
                                   bind(&Search::check_abort_expensive, this));
    if (m_deterministic)
    {
        unsigned interval =
            static_cast<unsigned>(max(1.0, expected_sim_per_sec() / 5.0));
        expensive_abort_checker.set_deterministic(interval);
    }
    while (m_tree.get_root().get_count() == 0
           || m_nu_simulations < min_simulations
           || (! check_abort() && ! expensive_abort_checker()))
    {
        m_simulation.m_nodes.clear();
        m_simulation.m_nodes.push_back(&m_tree.get_root());
        m_state.start_simulation(m_nu_simulations);
        bool is_out_of_memory;
        bool is_terminal;
        play_in_tree(thread_id, is_out_of_memory, is_terminal);
        if (is_out_of_memory)
        {
            if (m_prune_full_tree)
            {
                double time = m_timer();
                if (! prune(time_source, time, max_time - time,
                            prune_min_count, prune_min_count))
                {
                    log("Aborting search because pruning failed.");
                    break;
                }
                continue;
            }
            else
            {
                log("Maximum tree size reached");
                break;
            }
        }
        m_stat_in_tree_len.add(double(m_state.get_nu_moves()));
        array<Float,max_players> eval;
        if (! is_terminal)
        {
            playout();
            eval = m_state.evaluate_playout();
        }
        else
            eval = m_state.evaluate_terminal();
        m_stat_len.add(double(m_state.get_nu_moves()));
        update_values(eval);
        if (m_rave)
            update_rave_values(eval);
        if (m_use_last_good_reply)
            update_last_good_reply(eval);
        on_search_iteration(m_nu_simulations, m_simulation);
        ++m_nu_simulations;
    }
    m_last_time = m_timer();
    write_info(log());
    bool result = select_move(mv);
    m_time_source = 0;
    return result;
}

template<class S, class M, unsigned P>
const Node<M>* Search<S,M,P>::select_child(const Node& node)
{
    LIBBOARDGAME_ASSERT(node.has_children());
    Float node_count = node.get_count();
    if (log_move_selection)
    {
        log() << "Search::select_child:\n"
              << "c=" << node_count << '\n'
              << "v=" << node.get_value() << '\n';
    }
    const Node* best_child = 0;
    Float best_value = -numeric_limits<Float>::max();
    m_bias_term.start_iteration(node_count);
    Float beta =
        sqrt(m_rave_equivalence / (3 * node_count + m_rave_equivalence));
    if (log_move_selection)
        log() << "beta=" << beta << '\n';
    for (ChildIterator i(m_tree, node); i; ++i)
    {
        if (log_move_selection)
            log() << get_move_string(i->get_move())
                  << " | c=" << i->get_count()
                  << " rc=" << i->get_rave_count();
        Float child_value = i->get_value();
        if (log_move_selection)
            log() << " v=" << child_value;
        Float value;
        if (m_rave)
        {
            if (log_move_selection)
                log() << " r=" << i->get_rave_value();
            value = beta * i->get_rave_value() + (1 - beta) * child_value;
        }
        else
            value = child_value;
        Float bias = m_bias_term.get(i->get_count());
        if (log_move_selection)
            log() << " e=" << bias;
        value += bias;
        if (log_move_selection)
            log() << " | " << value << '\n';
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

template<class S, class M, unsigned P>
const Node<M>* Search<S,M,P>::select_child_final(const Node& node,
                                       const vector<Move>* exclude_moves) const
{
    // Select the child with the highest visit count, use value as a tie breaker
    const Node* result = 0;
    Float max_count = -1;
    Float max_count_value = -numeric_limits<Float>::max();
    for (ChildIterator i(m_tree, node); i; ++i)
    {
        Float count = i->get_count();
        if (count > max_count
            || (count == max_count && i->get_value() > max_count_value))
        {
            if (exclude_moves != 0
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

template<class S, class M, unsigned P>
bool Search<S,M,P>::select_move(Move& mv, const vector<Move>* exclude_moves)
    const
{
    const Node* child = select_child_final(m_tree.get_root(), exclude_moves);
    if (child != 0)
    {
        mv = child->get_move();
        return true;
    }
    else
        return false;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_bias_term_constant(Float c)
{
    m_bias_term.set_bias_term_constant(c);
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_callback(function<void(double, double)> callback)
{
    m_callback = callback;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_expand_threshold(Float n)
{
    m_expand_threshold = n;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_deterministic()
{
    m_deterministic = true;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_last_good_reply(bool enable)
{
    m_use_last_good_reply = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_prune_count_start(Float n)
{
    m_prune_count_start = n;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_prune_full_tree(bool enable)
{
    m_prune_full_tree = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_rave(bool enable)
{
    m_rave = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_rave_check_same(bool enable)
{
    m_rave_check_same = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_rave_equivalence(Float n)
{
    m_rave_equivalence = n;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_reuse_subtree(bool enable)
{
    m_reuse_subtree = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_reuse_tree(bool enable)
{
    m_reuse_tree = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_tree_memory(size_t memory)
{
    m_tree_memory = memory;
    m_max_nodes = get_max_nodes(memory);
    m_tree.set_max_nodes(m_max_nodes);
    m_tmp_tree.set_max_nodes(m_max_nodes);
}

template<class S, class M, unsigned P>
void Search<S,M,P>::set_weight_rave_updates(bool enable)
{
    m_weight_rave_updates = enable;
}

template<class S, class M, unsigned P>
void Search<S,M,P>::update_last_good_reply(const array<Float,max_players>& eval)
{
    unsigned nu_players = get_nu_players();
    Float max_eval = eval[0];
    for (unsigned i = 1; i < nu_players; ++i)
        max_eval = max(eval[i], max_eval);
    array<bool,max_players> is_winner;
    for (unsigned i = 0; i < nu_players; ++i)
        // Note: this handles a draw as a win. Without additional information
        // we cannot make a good decision how to handle draws and some
        // experiments in Blokus Duo showed (with low confidence) that treating
        // them as a win for both players is slighly better than treating them
        // as a loss for both.
        is_winner[i] = (eval[i] == max_eval);
    unsigned nu_moves = m_state.get_nu_moves();
    // Iterate backwards to store 1st reply if a move was played more than once
    if (nu_moves > 1)
        for (unsigned i = nu_moves - 1; i > 0; --i)
        {
            PlayerMove reply = m_state.get_move(i);
            PlayerMove mv = m_state.get_move(i - 1);
            if (is_winner[reply.player])
                m_reply_table.store(mv, reply);
            else if (m_reply_table.get_reply(mv) == reply.move)
                m_reply_table.forget(mv, reply);
        }
}

template<class S, class M, unsigned P>
void Search<S,M,P>::update_rave_values(const array<Float,max_players>& eval)
{
    unsigned nu_moves = m_state.get_nu_moves();
    if (nu_moves == 0)
        return;
    vector<const Node*>& nodes = m_simulation.m_nodes;
    size_t nu_nodes = nodes.size();
    unsigned i = nu_moves - 1;
    for ( ; i >= nu_nodes; --i)
    {
        LIBBOARDGAME_ASSERT(i < nu_moves);
        PlayerMove mv = m_state.get_move(i);
        unsigned& first = m_first_play[mv.player][mv.move.to_int()];
        if (i < first)
            first = i;
    }
    while (true)
    {
        LIBBOARDGAME_ASSERT(i < nu_moves);
        LIBBOARDGAME_ASSERT(i < nu_nodes);
        PlayerMove mv = m_state.get_move(i);
        unsigned& first = m_first_play[mv.player][mv.move.to_int()];
        if (i < first)
            first = i;
        update_rave_values(eval, i, mv.player);
        if (i == 0)
            break;
        --i;
    }
    // Reset m_first_play
    for (unsigned i = 0; i < nu_moves; ++i)
    {
        PlayerMove mv = m_state.get_move(i);
        m_first_play[mv.player][mv.move.to_int()] =
            numeric_limits<unsigned>::max();
    }
}

template<class S, class M, unsigned P>
void Search<S,M,P>::update_rave_values(const array<Float,max_players>& eval,
                                       unsigned i, unsigned player)
{
    LIBBOARDGAME_ASSERT(i < m_simulation.m_nodes.size());
    const Node* node = m_simulation.m_nodes[i];
    if (! node->has_children())
        return;
    unsigned len = m_state.get_nu_moves();
    Float weight_factor = 1 / Float(len - i);
    for (ChildIterator it(m_tree, *node); it; ++it)
    {
        Move mv = it->get_move();
        if (! m_state.skip_rave(mv))
        {
            int m = mv.to_int();
            unsigned first = m_first_play[player][m];
            LIBBOARDGAME_ASSERT(first >= i);
            if (first == numeric_limits<unsigned>::max())
                continue;
            if (m_rave_check_same)
            {
                bool other_played_same = false;
                for (unsigned i = 0; i < max_players; ++i)
                    if (i != player)
                    {
                        unsigned first_other = m_first_play[i][m];
                        if (first_other >= i && first_other <= first)
                        {
                            other_played_same = true;
                            break;
                        }
                    }
                if (other_played_same)
                    continue;
            }
            Float weight;
            if (m_weight_rave_updates)
                // Weight decreases linearly from 2 at the start to 1 at the
                // end of a simulation. Being proportional to the relative move
                // distance (by dividing it by the length of the simulation),
                // is essential for a positive effect of rave weighting,
                // however the scaling to [2..1] could not be optimal for
                // different games and should be made configurable in the future
                weight = 2 - Float(first - i) * weight_factor;
            else
                weight = 1;
            m_tree.add_rave_value(*it, eval[player], weight);
        }
    }
}

template<class S, class M, unsigned P>
void Search<S,M,P>::update_values(const array<Float,max_players>& eval)
{
    m_tree.add_value(m_tree.get_root(), eval[m_player]);
    unsigned nu_nodes = static_cast<unsigned>(m_simulation.m_nodes.size());
    for (unsigned i = 1; i < nu_nodes; ++i)
    {
        const Node& node = *m_simulation.m_nodes[i];
        PlayerMove mv = m_state.get_move(i - 1);
        m_tree.add_value(node, eval[mv.player]);
    }
    unsigned nu_players = get_nu_players();
    for (unsigned i = 0; i < nu_players; ++i)
    {
        m_root_val[i].add(eval[i]);
        m_init_val[i].add(eval[i]);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_SEARCH_H
