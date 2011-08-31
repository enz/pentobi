//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_H
#define LIBPENTOBI_MCTS_STATE_H

#include "libpentobi_base/ColorMap.h"
#include "libboardgame_mcts/PlayerMove.h"
#include "libboardgame_mcts/Tree.h"
#include "libboardgame_mcts/ValueType.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Unused.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/MoveMarker.h"
#include "libpentobi_base/PieceValueHeuristic.h"
#include "libpentobi_base/PointList.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::ColorMove;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveMarker;
using libpentobi_base::MovePoints;
using libpentobi_base::PieceValueHeuristic;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::SymmetricPoints;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libboardgame_mcts::PlayerMove;
using libboardgame_mcts::ValueType;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

/** Constant data shared between the search states. */
struct SharedConst
{
    const Board& board;

    /** The color to play at the root of the search. */
    const Color& to_play;

    bool detect_symmetry;

    bool avoid_symmetric_draw;

    /** Maximum value to modify the win/loss result by the score. */
    ValueType score_modification;

    PieceValueHeuristic piece_value;

    SharedConst(const Board& bd, const Color& to_play);
};

//-----------------------------------------------------------------------------

class State
{
public:
    /** Constructor.
        @param shared_const (@ref libboardgame_doc_storesref) */
    State(const Board& bd, const SharedConst& shared_const);

    State(const State& state);

    ~State() throw();

    /** Play a move in the in-tree phase of the search. */
    void play(const Move& mv);

    void start_search();

    void start_simulation(size_t n);

    void gen_children(libboardgame_mcts::Tree<Move>::NodeExpander& expander);

    void start_playout();

    /** Generate and play a playout move.
        @return @c false if end of game was reached, and no move was played */
    bool gen_and_play_playout_move();

    array<ValueType, 4> evaluate_playout();

    array<ValueType, 4> evaluate_terminal();

    /** Get number of moves in the current simulation. */
    unsigned int get_nu_moves() const;

    /** Get move in the current simulation. */
    PlayerMove<Move> get_move(unsigned int n) const;

    /** Do not update RAVE values for n'th move of the current simulation. */
    bool skip_rave(Move mv) const;

    void dump(ostream& out) const;

    void write_playout_info(ostream& out) const;

    ValueType get_mean_score() const;

private:
    /** Flag set to true at the beginning of the playout or node expansion.
        In the in-tree phase, the state simplay follows the moves that the
        MCTS search plays but does not need to update expensive data like
        move lists. This flag is set to true once the extended update should
        begin, which means at the call to gen_children() (expansion of the last
        node in the in-tree phase) or at the beginning of the playout. (Note
        that the search does not necessarily always expand a node, if the expand
        threshold is greater than one). */
    bool m_extended_update;

    unsigned int m_nu_moves_initial;

    unsigned int m_nu_passes;

    int m_max_local;

    const SharedConst& m_shared_const;

    /** Incrementally updated lists of legal moves for both colors.
        Only the move list for the color to play van be used in any given
        position, the other color is not updated immediately after a move. */
    ColorMap<vector<Move>> m_moves;

    /** The last move by each color.
        Used for updating the move lists and only defined if m_extended_update
        is true. */
    ColorMap<Move> m_last_move;

    ColorMap<bool> m_is_move_list_initialized;

    ColorMap<bool> m_has_moves;

    /** Moves that are a local response to the last move.
        These moves occupy at least one of the corner points of the last
        piece played. */
    vector<Move> m_local_moves;

    MoveMarker m_local_move_marker;

    /** Local variable during update.
        Reused for efficiency. */
    vector<Move> m_tmp_moves;

    /** Local variable during update.
        Reused for efficiency. */
    MoveMarker m_marker;

    PointList m_local_points;

    Grid<int> m_local_points_marker;

    SymmetricPoints m_symmetric_points;

    Board m_bd;

    RandomGenerator m_random;

    /** Precomputed State::m_score_modification / BoardConst::max_score. */
    ValueType m_score_modification_factor;

    ValueType m_score_sum;

    /** Number of simulations of this state in the current search.
        Currently identical to Search::get_nu_simulations() but could be
        different in the future if Search is multi-threaded. */
    size_t m_nu_simulations;

    bool m_check_symmetric_draw;

    bool m_is_symmetry_broken;

    /** Not implemented. */
    State& operator=(const State&);

    void check_local_move(int nu_local, Move mv);

    void clear_local_moves();

    void clear_local_points();

    void init_local_points();

    void init_move_list(Color c);

    void init_symmetry_info();

    bool is_forbidden(Color c, const MovePoints& points, int& nu_local) const;

    void update_move_list(Color c);

    void update_symmetry_info(Move mv);
};

inline ValueType State::get_mean_score() const
{
    return m_score_sum / m_nu_simulations;
}

inline PlayerMove<Move> State::get_move(unsigned int n) const
{
    ColorMove mv = m_bd.get_move(m_nu_moves_initial + n);
    return PlayerMove<Move>(mv.color.to_int(), mv.move);
}

inline unsigned int State::get_nu_moves() const
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_moves() >= m_nu_moves_initial);
    return m_bd.get_nu_moves() - m_nu_moves_initial;
}

inline bool State::skip_rave(Move mv) const
{
    LIBBOARDGAME_UNUSED(mv);
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
