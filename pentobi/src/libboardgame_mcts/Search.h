//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Search.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_SEARCH_H
#define LIBBOARDGAME_MCTS_SEARCH_H

#include <algorithm>
#include <array>
#include <functional>
#include <boost/format.hpp>
#include "ChildIterator.h"
#include "PlayerMove.h"
#include "Tree.h"
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
using libboardgame_util::get_abort;
using libboardgame_util::log;
using libboardgame_util::IntervalChecker;
using libboardgame_util::Parameters;
using libboardgame_util::StatisticsExt;
using libboardgame_util::Timer;
using libboardgame_util::TimeIntervalChecker;
using libboardgame_util::TimeSource;
using libboardgame_util::string_util::time_to_string;
using libboardgame_util::string_util::to_string;

//-----------------------------------------------------------------------------

/** Monte-Carlo tree search. */
template<class S, class M, class E, unsigned int P>
class Search
{
public:
    typedef S State;

    typedef M Move;

    typedef E ExplorationTerm;

    static const unsigned int nu_players = P;

    typedef libboardgame_mcts::Node<M> Node;

    typedef libboardgame_mcts::ChildIterator<M> ChildIterator;

    typedef libboardgame_mcts::Tree<M> Tree;

    typedef libboardgame_mcts::PlayerMove<M> PlayerMove;

    static const bool log_move_selection = false;

    Search(const State& state);

    virtual ~Search() throw();

    /** Get string representation of a move. */
    virtual string get_move_string(Move mv) const = 0;

    virtual unsigned int get_player() const = 0;

    /** Get a copy of the search parameters that determine the reusability of
        (a part of) the tree between searches.
        Usually, one should not try to use a subtree from the last search as an
        init tree for the next search (should the current state be a follow-up
        state of the last search) if any of those parameters were changed. */
    virtual Parameters get_reuse_param() const;

    virtual void write_info(ostream& out) const;

    virtual void write_info_ext(ostream& out) const;

    void set_expand_threshold(ValueType n);

    ValueType get_expand_threshold() const;

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

    void set_rave_equivalence(ValueType value);

    ValueType get_rave_equivalence() const;

    void set_unexplored_value(ValueType value);

    ValueType get_unexplored_value() const;

    void set_weight_rave_updates(bool enable);

    bool get_weight_rave_updates() const;

    /** See get_reuse_param() */
    const Parameters& get_last_reuse_param() const;

    /** Value to start the tree pruning with.
        This value should be above typical count initializations if prior
        knowledge initialization is used. */
    void set_prune_count_start(ValueType n);

    ValueType get_prune_count_start() const;

    /** Total size of the trees in bytes. */
    void set_tree_memory(size_t n);

    size_t get_tree_memory() const;

    /** Run a search.
        @param[out] mv
        @param max_count Number of simulations to run. The search might return
        earlier, if the best move cannot change anymore, or if the count of the
        root node was initialized from an init tree
        @param min_simulations
        @param max_time Maximum search time. Only used if max_count is zero
        @param time_source Time source for time measurement
        @param init_tree A tree to initialize the search tree with. The caller
        is reponsible that the state at the root of the init tree is the same
        as the current game state.
        @param clear_tree Whether to clear the tree at the start of the
        search. If this parameter is set to false, then the caller is
        reponsible that the state at the root of the current tree is the same
        as the current game state.
        @return @c false, if no move could be generated. This can happen if the
        root node was not expanded, because the position is a terminal
        position or because the search was immediately aborted. */
    bool search(Move& mv, ValueType max_count, size_t min_simulations,
                double max_time, TimeSource& time_source, Tree* init_tree = 0,
                bool clear_tree = true);

    /** Get the tree for temporary operations.
        The tree is internally used by this class, but can also be used by
        external users or subclasses. The tree is not guaranteed to be cleared
        or to keep its state between calls to search(). */
    Tree& get_tmp_tree();

    const Tree& get_tree() const;

    /** Clear tree of last search.
        Has the eeffect that no subtree can be reused for the next search. */
    void clear_tree();

    void dump(ostream& out) const;

    size_t get_nu_simulations() const;

    /** Select the move to play.
        Uses select_child_final() on the root node. */
    bool select_move(Move& mv, vector<Move>* exclude_moves = 0) const;

    /** Select the best child of a node after the search.
        Selects child with highest visit count; the value is used as a
        tie-breaker for equal counts (important at very low number of
        simulations, e.g. all children have count 1 or 0). */
    const Node* select_child_final(const Node& node,
                                   vector<Move>* exclude_moves = 0) const;

    State& get_state();

    const State& get_state() const;

    ExplorationTerm& get_exploration_term();

    /** Set a callback function that informs the caller about the
        estimated time left.
        The callback function will be called about every 0.1s. The arguments
        of the callback function are: elapsed time, estimated remaining time. */
    void set_callback(function<void(double, double)> callback);

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

    ValueType m_expand_threshold;

    bool m_prune_full_tree;

    bool m_rave;

    bool m_rave_check_same;

    bool m_weight_rave_updates;

    /** Player to play at the root node of the search. */
    unsigned int m_player;

    /** Time of last search. */
    double m_last_time;

    ValueType m_unexplored_value;

    ValueType m_prune_count_start;

    ValueType m_rave_equivalence;

    /** Maximum simulations of current search. */
    ValueType m_max_count;

    size_t m_tree_memory;

    /** Number simulations of current search. */
    size_t m_nu_simulations;

    /** Maximum time of current search. */
    double m_max_time;

    TimeSource* m_time_source;

    Timer m_timer;

    State m_state;

    Simulation m_simulation;

    /** Local variable for update_rave_values().
        Stores the first time a move was played for each player.
        Reused for efficiency. */
    array<array<size_t, Move::range>, nu_players> m_first_play;

    ExplorationTerm m_exploration_term;

    Tree m_tree;

    Tree m_tmp_tree;

    AssertionHandler m_assertion_handler;

    /** See get_reuse_param() */
    Parameters m_last_reuse_param;

    StatisticsExt m_stat_len;

    StatisticsExt m_stat_in_tree_len;

    function<void(double, double)> m_callback;

    bool check_abort() const;

    bool check_abort_expensive() const;

    bool check_move_cannot_change(ValueType count, ValueType remaining) const;

    bool expand_node(const Node& node, const Node*& best_child);

    LIBBOARDGAME_FLATTEN void playout();

    void play_in_tree(bool& is_out_of_memory, bool& is_terminal);

    ValueType prune(TimeSource& time_source, double time, double max_time,
                    ValueType prune_min_count);

    const Node* select_child(const Node& node);

    void update_rave_values(const array<ValueType, nu_players>& eval);

    void update_rave_values(const array<ValueType, nu_players>&, size_t i,
                            unsigned int player);

    void update_values(const array<ValueType, nu_players>& eval);
};

template<class S, class M, class E, unsigned int P>
Search<S, M, E, P>::AssertionHandler::AssertionHandler(const Search& search)
    : m_search(search)
{
}

template<class S, class M, class E, unsigned int P>
Search<S, M, E, P>::AssertionHandler::~AssertionHandler() throw()
{
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::AssertionHandler::run()
{
    m_search.dump(log());
}

template<class S, class M, class E, unsigned int P>
Search<S, M, E, P>::Simulation::~Simulation() throw()
{
}

template<class S, class M, class E, unsigned int P>
Search<S, M, E, P>::Search(const State& state)
    : m_expand_threshold(0),
      m_prune_full_tree(true),
      m_rave(false),
      m_rave_check_same(false),
      m_weight_rave_updates(true),
      m_unexplored_value(numeric_limits<ValueType>::max()),
      m_prune_count_start(16),
      m_rave_equivalence(1000),
      m_tree_memory(256000000),
      m_state(state),
      m_tree(1),
      m_tmp_tree(1),
      m_assertion_handler(*this)
{
    set_tree_memory(m_tree_memory);
    for (unsigned int i = 0; i < nu_players; ++i)
        m_first_play[i].fill(numeric_limits<size_t>::max());
}

template<class S, class M, class E, unsigned int P>
Search<S, M, E, P>::~Search() throw()
{
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::check_abort() const
{
    if (get_abort())
    {
        log("Search aborted");
        return true;
    }
    ValueType count = m_tree.get_root().get_count();
    static_assert(numeric_limits<ValueType>::radix == 2,
                  "libboardgame_mcts::ValueType must have radix 2");
    if (count == (size_t(1) << numeric_limits<ValueType>::digits) - 1)
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

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::check_abort_expensive() const
{
    ValueType count = m_tree.get_root().get_count();
    if (count == 0)
        return false;
    double time = m_timer();
    if (time < 0.1)
        // Simulations per second might be inaccurate for very small times
        return false;
    double simulations_per_sec = double(m_nu_simulations) / time;
    if (m_max_count == 0)
    {
        if (time > m_max_time)
        {
            log("Maximum time reached");
            return true;
        }
        double remaining_time = m_max_time - time;
        if (m_callback)
            m_callback(time, remaining_time);
        double remaining_simulations = remaining_time * simulations_per_sec;
        if (check_move_cannot_change(count, remaining_simulations))
        {
            log("Move cannot change anymore");
            return true;
        }
    }
    else
    {
        ValueType remaining_simulations = m_max_count - count;
        if (m_callback)
        {
            double remaining_time = remaining_simulations / simulations_per_sec;
            m_callback(time, remaining_time);
        }
        if (check_move_cannot_change(count, remaining_simulations))
        {
            log("Move cannot change anymore");
            return true;
        }
    }
    return false;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::check_move_cannot_change(ValueType count,
                                               ValueType remaining) const
{
    if (remaining > count)
        return false;
    ValueType max_count = 0;
    ValueType second_max_count = 0;
    for (ChildIterator i(m_tree.get_root()); i; ++i)
    {
        ValueType count = i->get_count();
        if (count > max_count)
        {
            second_max_count = max_count;
            max_count = count;
        }
    }
    return (max_count > second_max_count + remaining);
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::dump(ostream& out) const
{
    m_state.dump(out);
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::expand_node(const Node& node, const Node*& best_child)
{
    typename Tree::NodeExpander expander(m_tree, node);
    m_state.gen_children(expander);
    if (! expander.is_tree_full())
    {
        expander.link_children();
        best_child = expander.get_best_child();
        return true;
    }
    return false;
}

template<class S, class M, class E, unsigned int P>
typename Search<S, M, E, P>::ExplorationTerm&
Search<S, M, E, P>::get_exploration_term()
{
    return m_exploration_term;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::clear_tree()
{
    m_tree.clear();
}

template<class S, class M, class E, unsigned int P>
ValueType Search<S, M, E, P>::get_expand_threshold() const
{
    return m_expand_threshold;
}

template<class S, class M, class E, unsigned int P>
const Parameters& Search<S, M, E, P>::get_last_reuse_param() const
{
    return m_last_reuse_param;
}

template<class S, class M, class E, unsigned int P>
size_t Search<S, M, E, P>::get_nu_simulations() const
{
    return m_nu_simulations;
}

template<class S, class M, class E, unsigned int P>
Parameters Search<S, M, E, P>::get_reuse_param() const
{
    Parameters p;
    p.create<bool>("rave", m_rave);
    p.create<bool>("weight_rave_updates", m_weight_rave_updates);
    p.create<ValueType>("rave_equivalence", m_rave_equivalence);
    p.create<ValueType>("unexplored_value", m_unexplored_value);
    return p;
}

template<class S, class M, class E, unsigned int P>
ValueType Search<S, M, E, P>::get_prune_count_start() const
{
    return m_prune_count_start;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::get_prune_full_tree() const
{
    return m_prune_full_tree;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::get_rave() const
{
    return m_rave;
}

template<class S, class M, class E, unsigned int P>
ValueType Search<S, M, E, P>::get_rave_equivalence() const
{
    return m_rave_equivalence;
}

template<class S, class M, class E, unsigned int P>
inline typename Search<S, M, E, P>::State& Search<S, M, E, P>::get_state()
{
    return m_state;
}

template<class S, class M, class E, unsigned int P>
inline const typename Search<S, M, E, P>::State& Search<S, M, E, P>::get_state()
    const
{
    return m_state;
}

template<class S, class M, class E, unsigned int P>
inline TimeSource& Search<S, M, E, P>::get_time_source()
{
    LIBBOARDGAME_ASSERT(m_time_source != 0);
    return *m_time_source;
}

template<class S, class M, class E, unsigned int P>
size_t Search<S, M, E, P>::get_tree_memory() const
{
    return m_tree_memory;
}

template<class S, class M, class E, unsigned int P>
inline typename Search<S, M, E, P>::Tree& Search<S, M, E, P>::get_tmp_tree()
{
    return m_tmp_tree;
}

template<class S, class M, class E, unsigned int P>
inline const typename Search<S, M, E, P>::Tree& Search<S, M, E, P>::get_tree()
    const
{
    return m_tree;
}

template<class S, class M, class E, unsigned int P>
ValueType Search<S, M, E, P>::get_unexplored_value() const
{
    return m_unexplored_value;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::get_weight_rave_updates() const
{
    return m_weight_rave_updates;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::on_search_iteration(size_t n,
                                          const Simulation& simulation)
{
    LIBBOARDGAME_UNUSED(n);
    LIBBOARDGAME_UNUSED(simulation);
    // Default implementation does nothing
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::on_start_search()
{
    // Default implementation does nothing
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::playout()
{
    m_state.start_playout();
    while (true)
        if (! m_state.gen_and_play_playout_move())
            break;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::play_in_tree(bool& is_out_of_memory, bool& is_terminal)
{
    const Node& root = m_tree.get_root();
    const Node* node = &root;
    bool done = false;
    is_out_of_memory = false;
    is_terminal = false;
    while (! done)
    {
        if (node->has_children())
            node = select_child(*node);
        else if (node->get_visit_count() >= m_expand_threshold || node == &root)
        {
            if (! expand_node(*node, node))
            {
                is_out_of_memory = true;
                return;
            }
            if (node == 0)
            {
                is_terminal = true;
                return;
            }
            done = true;
        }
        else
            return;
        m_simulation.m_nodes.push_back(node);
        m_state.play(node->get_move());
    }
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::write_info(ostream& out) const
{
    const Node& root = m_tree.get_root();
    ValueType count = root.get_count();
    if (m_nu_simulations == 0 || count == 0)
        return;
    out << format(
               "Val: %.2f, Cnt: %.0f, Sim: %i, Nds: %i, Tm: %s, Sim/s: %.0f\n"
               "Len: %s, Dp: %s\n")
        % root.get_value() % count % m_nu_simulations % m_tree.get_nu_nodes()
        % time_to_string(m_last_time)
        % (double(m_nu_simulations) / m_last_time)
        % m_stat_len.to_string(true, 1, true)
        % m_stat_in_tree_len.to_string(true, 1, true);
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::write_info_ext(ostream& out) const
{
    LIBBOARDGAME_UNUSED(out);
}

template<class S, class M, class E, unsigned int P>
ValueType Search<S, M, E, P>::prune(TimeSource& time_source, double time,
                                 double max_time, ValueType prune_min_count)
{
    Timer timer(time_source);
    TimeIntervalChecker interval_checker(time_source, max_time);
    log(format("Pruning count %1% (at time %2%)") % prune_min_count % time);
    m_tmp_tree.clear();
    if (! m_tree.copy_subtree(m_tmp_tree, m_tmp_tree.get_root(),
                              m_tree.get_root(), prune_min_count, true,
                              &interval_checker))
    {
        log("Pruning aborted");
        return prune_min_count;
    }
    int percent = int(m_tmp_tree.get_nu_nodes() * 100 / m_tree.get_nu_nodes());
    log(format("Pruned size: %1% (%2%%%, time=%3%)")
               % m_tmp_tree.get_nu_nodes() % percent % timer());
    m_tree.swap(m_tmp_tree);
    if (percent > 50)
        return prune_min_count * 2;
    else
        return prune_min_count;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::search(Move& mv, ValueType max_count,
                             size_t min_simulations, double max_time,
                             TimeSource& time_source, Tree* init_tree,
                             bool clear_tree)
{
    m_last_reuse_param = get_reuse_param();
    m_timer.reset(time_source);
    m_time_source = &time_source;
    on_start_search();
    m_player = get_player();
    m_stat_len.clear();
    m_stat_in_tree_len.clear();
    if (init_tree != 0)
        m_tree.swap(*init_tree);
    else if (clear_tree)
        m_tree.clear();
    m_state.start_search();
    m_max_count = max_count;
    m_nu_simulations = 0;
    ValueType prune_min_count = get_prune_count_start();
    m_max_time = max_time;
    double time_interval;
    if (max_count > 0)
        time_interval = 0.1;
    else
        time_interval = max_time > 1 ? 0.1 : 0.1 * max_time;
    IntervalChecker expensive_abort_checker(time_source, time_interval,
                                   bind(&Search::check_abort_expensive, this));
    while (m_tree.get_root().get_count() == 0
           || m_nu_simulations < min_simulations
           || (! check_abort() && ! expensive_abort_checker()))
    {
        m_simulation.m_nodes.clear();
        m_simulation.m_nodes.push_back(&m_tree.get_root());
        m_state.start_simulation(m_nu_simulations);
        bool is_out_of_memory;
        bool is_terminal;
        play_in_tree(is_out_of_memory, is_terminal);
        if (is_out_of_memory)
        {
            if (m_prune_full_tree)
            {
                double time = m_timer();
                prune_min_count =
                    prune(time_source, time, max_time - time, prune_min_count);
                continue;
            }
            else
            {
                log("Maximum tree size reached");
                break;
            }
        }
        m_stat_in_tree_len.add(double(m_state.get_nu_moves()));
        array<ValueType, nu_players> eval;
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
        on_search_iteration(m_nu_simulations, m_simulation);
        ++m_nu_simulations;
    }
    m_last_time = m_timer();
    write_info(log());
    bool result = select_move(mv);
    m_time_source = 0;
    return result;
}

template<class S, class M, class E, unsigned int P>
const Node<M>* Search<S, M, E, P>::select_child(const Node& node)
{
    if (log_move_selection)
    {
        log() << "Search::select_child:\n"
              << "c=" << node.get_count() << '\n';
        if (node.get_count() > 0)
            log() << "v=" << node.get_value() << '\n';
    }
    LIBBOARDGAME_ASSERT(node.has_children());
    const Node* best_child = 0;
    ValueType best_value = -numeric_limits<ValueType>::max();
    m_exploration_term.begin_select_child(node);
    if (m_rave)
    {
        ValueType beta = sqrt(m_rave_equivalence
                              / (3 * node.get_count() + m_rave_equivalence));
        if (log_move_selection)
            log() << "beta=" << beta << '\n';
        for (ChildIterator i(node); i; ++i)
        {
            if (log_move_selection)
                log() << get_move_string(i->get_move())
                      << " | c=" << i->get_count()
                      << " rc=" << i->get_rave_count();
            ValueType value;
            if (i->get_count() > 0)
            {
                ValueType child_value = i->get_value();
                if (log_move_selection)
                    log() << " v=" << child_value;
                if (i->get_rave_count() > 0)
                {
                    if (log_move_selection)
                        log() << " r=" << i->get_rave_value();
                    value =
                        beta * i->get_rave_value() + (1 - beta) * child_value;
                }
                else
                    value = child_value;
            }
            else if (i->get_rave_count() > 0)
            {
                if (log_move_selection)
                    log() << " r=" << i->get_rave_value();
                value = i->get_rave_value();
            }
            else
                value = m_unexplored_value;
            ValueType exploration_term = m_exploration_term.get(*i);
            if (log_move_selection)
                log() << " e=" << exploration_term;
            value += exploration_term;
            if (log_move_selection)
                log() << " | " << value << '\n';
            if (value > best_value)
            {
                best_value = value;
                best_child = &(*i);
            }
        }
    }
    else
    {
        for (ChildIterator i(node); i; ++i)
        {
            ValueType value;
            if (i->get_count() > 0)
                value = i->get_value();
            else
                value = m_unexplored_value;
            value += m_exploration_term.get(*i);
            if (value > best_value)
            {
                best_value = value;
                best_child = &(*i);
            }
        }
    }
    if (log_move_selection)
        log() << "Selected: " << get_move_string(best_child->get_move())
              << '\n';
    return best_child;
}

template<class S, class M, class E, unsigned int P>
const Node<M>* Search<S, M, E, P>::select_child_final(const Node& node,
                                             vector<Move>* exclude_moves) const
{
    const Node* result = 0;
    ValueType max_count = -1;
    ValueType max_count_value = numeric_limits<ValueType>::max();
    for (ChildIterator i(node); i; ++i)
    {
        ValueType count = i->get_visit_count();
        if (count > max_count
            || (count == max_count && count > 0 && max_count > 0
                && i->get_value() < max_count_value))
        {
            if (exclude_moves != 0
                && find(exclude_moves->begin(), exclude_moves->end(),
                        i->get_move()) != exclude_moves->end())
                continue;
            max_count = count;
            if (count > 0)
                max_count_value = i->get_value();
            result = &(*i);
        }
    }
    return result;
}

template<class S, class M, class E, unsigned int P>
bool Search<S, M, E, P>::select_move(Move& mv, vector<Move>* exclude_moves)
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

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_callback(function<void(double, double)> callback)
{
    m_callback = callback;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_expand_threshold(ValueType n)
{
    m_expand_threshold = n;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_prune_count_start(ValueType n)
{
    m_prune_count_start = n;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_prune_full_tree(bool enable)
{
    m_prune_full_tree = enable;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_rave(bool enable)
{
    m_rave = enable;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_rave_check_same(bool enable)
{
    m_rave_check_same = enable;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_rave_equivalence(ValueType n)
{
    m_rave_equivalence = n;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_tree_memory(size_t n)
{
    m_tree_memory = n;
    size_t max_nodes = n / sizeof(Node) / 2;
    m_tree.set_max_nodes(max_nodes);
    m_tmp_tree.set_max_nodes(max_nodes);
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_unexplored_value(ValueType value)
{
    m_unexplored_value = value;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::set_weight_rave_updates(bool enable)
{
    m_weight_rave_updates = enable;
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::update_rave_values(
                                       const array<ValueType, nu_players>& eval)
{
    unsigned int nu_moves = m_state.get_nu_moves();
    if (nu_moves == 0)
        return;
    vector<const Node*>& nodes = m_simulation.m_nodes;
    size_t nu_nodes = nodes.size();
    unsigned int i = nu_moves - 1;
    for ( ; i >= nu_nodes; --i)
    {
        LIBBOARDGAME_ASSERT(i < nu_moves);
        PlayerMove mv = m_state.get_move(i);
        size_t& first = m_first_play[mv.player][mv.move.to_int()];
        if (i < first)
            first = i;
    }
    while (true)
    {
        LIBBOARDGAME_ASSERT(i < nu_moves);
        LIBBOARDGAME_ASSERT(i < nu_nodes);
        PlayerMove mv = m_state.get_move(i);
        size_t& first = m_first_play[mv.player][mv.move.to_int()];
        if (i < first)
            first = i;
        update_rave_values(eval, i, mv.player);
        if (i == 0)
            break;
        --i;
    }
    // Reset m_first_play
    for (unsigned int i = 0; i < nu_moves; ++i)
    {
        PlayerMove mv = m_state.get_move(i);
        m_first_play[mv.player][mv.move.to_int()] =
            numeric_limits<size_t>::max();
    }
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::update_rave_values(
                             const array<ValueType, nu_players>& eval, size_t i,
                             unsigned int player)
{
    LIBBOARDGAME_ASSERT(i < m_simulation.m_nodes.size());
    const Node* node = m_simulation.m_nodes[i];
    if (! node->has_children())
        return;
    size_t len = m_state.get_nu_moves();
    ValueType weight_factor = 1 / ValueType(len - i);
    for (ChildIterator it(*node); it; ++it)
    {
        Move mv = it->get_move();
        if (! m_state.skip_rave(mv))
        {
            int m = mv.to_int();
            size_t first = m_first_play[player][m];
            LIBBOARDGAME_ASSERT(first >= i);
            if (first == numeric_limits<size_t>::max())
                continue;
            if (m_rave_check_same)
            {
                for (unsigned int i = 0; i < nu_players; ++i)
                    if (i != player)
                    {
                        size_t first_other = m_first_play[i][m];
                        if (first_other >= i && first_other <= first)
                            continue;
                    }
            }
            ValueType weight;
            if (m_weight_rave_updates)
                weight = 2 - ValueType(first - i) * weight_factor;
            else
                weight = 1;
            m_tree.add_rave_value(*it, eval[player], weight);
        }
    }
}

template<class S, class M, class E, unsigned int P>
void Search<S, M, E, P>::update_values(const array<ValueType, nu_players>& eval)
{
    m_tree.add_value(m_tree.get_root(), eval[m_player]);
    m_tree.inc_visit_count(m_tree.get_root());
    for (size_t i = 1; i < m_simulation.m_nodes.size(); ++i)
    {
        const Node& node = *m_simulation.m_nodes[i];
        PlayerMove mv = m_state.get_move(i - 1);
        m_tree.add_value(node, eval[mv.player]);
        m_tree.inc_visit_count(node);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_SEARCH_H
