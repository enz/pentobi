//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_H
#define LIBPENTOBI_MCTS_STATE_H

#include "LocalValue.h"
#include "libboardgame_mcts/Float.h"
#include "libboardgame_mcts/PlayerMove.h"
#include "libboardgame_mcts/Tree.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/Unused.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/MoveMarker.h"
#include "libpentobi_base/PieceMap.h"
#include "libpentobi_base/PointList.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::Float;
using libboardgame_mcts::PlayerMove;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;
using libboardgame_util::Statistics;
using libboardgame_util::StatisticsBase;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::ColorMove;
using libpentobi_base::Variant;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::PieceMap;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::SymmetricPoints;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;

//-----------------------------------------------------------------------------

/** Constant data shared between the search states. */
struct SharedConst
{
    const Board* board;

    /** The color to play at the root of the search. */
    const Color& to_play;

    bool detect_symmetry;

    bool avoid_symmetric_draw;

    /** Maximum value to modify the win/loss result by the score. */
    Float score_modification;

    /** Lookup table for symmetric points (only used in Duo). */
    SymmetricPoints symmetric_points;

    /** Precomputed information if move is forbidden at the start position (and
        therefore in all positions in the search). */
    ColorMap<MoveMarker> is_forbidden_at_root;

    /** Precomputed lists of considered pieces depending on the move number. */
    array<PieceMap<bool>,Board::max_game_moves> is_piece_considered;

    /** Precomputed lists of considered pieces if all pieces are enforced to be
        considered (because using the restricted set of pieces would generate
        no moves). */
    PieceMap<bool> is_piece_considered_all;

    SharedConst(const Color& to_play);
};

//-----------------------------------------------------------------------------

/** A state of a simulation.
    This class contains modifiable data used in a simulation. In multi-threaded
    search (not yet implemented), each thread uses its own instance of this
    class. */
class State
{
public:
    /** Constructor.
        @param initial_variant Game variant to initialize the internal
        board with (may avoid unnecessary BoardConst creation for game variant
        that is never used)
        @param shared_const (@ref libboardgame_doc_storesref) */
    State(Variant initial_variant, const SharedConst& shared_const);

    State(const State& state);

    ~State() throw();

    /** Play a move in the in-tree phase of the search. */
    void play(Move mv);

    unsigned get_to_play() const;

    void start_search();

    void start_simulation(size_t n);

    void gen_children(libboardgame_mcts::Tree<Move>::NodeExpander& expander,
                      Float init_val);

    void start_playout();

    /** Generate and play a playout move.
        @return @c false if end of game was reached, and no move was played */
    bool gen_and_play_playout_move(Move last_good_reply);

    array<Float,4> evaluate_playout();

    array<Float,4> evaluate_terminal();

    /** Get number of moves in the current simulation. */
    unsigned get_nu_moves() const;

    /** Get move in the current simulation. */
    PlayerMove<Move> get_move(unsigned n) const;

    /** Do not update RAVE values for n'th move of the current simulation. */
    bool skip_rave(Move mv) const;

    void dump(ostream& out) const;

    void write_info(ostream& out) const;

private:
    struct MoveFeatures
    {
        /** Only used on Classic and Trigon boards. */
        unsigned dist_to_center;

        /** Does the move touch a piece of the same player? */
        bool connect;

        /** Heuristic value of the move expressed in score points. */
        Float heuristic;
    };

    /** Flag set to true at the beginning of the playout or node expansion.
        In the in-tree phase, the state simplay follows the moves that the
        MCTS search plays but does not need to update expensive data like
        move lists. This flag is set to true once the extended update should
        begin, which means at the call to gen_children() (expansion of the last
        node in the in-tree phase) or at the beginning of the playout. (Note
        that the search does not necessarily always expand a node, if the expand
        threshold is greater than one). */
    bool m_extended_update;

    bool m_has_connect_move;

    unsigned m_nu_moves_initial;

    Color::IntType m_nu_passes;

    unsigned m_max_local_value;

    unsigned short m_max_playable_piece_size;

    unsigned short m_max_playable_piece_size_local;

    /** Maximum of Features::heuristic for all moves. */
    Float m_max_heuristic;

    unsigned m_min_dist_to_center;

    const SharedConst& m_shared_const;

    Board m_bd;

    const MoveInfo* m_move_info_array;

    /** Incrementally updated lists of legal moves for both colors.
        Only the move list for the color to play van be used in any given
        position, the other color is not updated immediately after a move. */
    ColorMap<MoveList> m_moves;

    ColorMap<const PieceMap<bool>*> m_is_piece_considered;

    ArrayList<MoveFeatures, Move::range> m_features;

    /** The last move by each color.
        Used for updating the move lists and only defined if m_extended_update
        is true. */
    ColorMap<Move> m_last_move;

    ColorMap<bool> m_is_move_list_initialized;

    ColorMap<bool> m_has_moves;

    /** Moves that are a local response to the last move.
        These moves occupy at least one of the corner points of the last
        piece played. */
    MoveList m_local_moves;

    /** Local variable during initializing and updating of move lists.
        Reused for efficiency. */
    MoveMarker m_marker;

    LocalValue m_local_value;

    RandomGenerator m_random;

    /** Precomputed State::m_score_modification / BoardConst::max_score. */
    Float m_score_modification_factor;

    Statistics<> m_stat_score;

    /** Number of simulations of this state in the current search.
        Currently identical to Search::get_nu_simulations() but could be
        different in the future if Search is multi-threaded. */
    size_t m_nu_simulations;

    size_t m_nu_playout_moves;

    size_t m_nu_last_good_reply_moves;

    bool m_check_symmetric_draw;

    bool m_is_symmetry_broken;

    /** Enforce all pieces to be considered for the rest of the simulation. */
    bool m_consider_all_pieces;

    /** Distance to center heuristic. */
    Grid<unsigned> m_dist_to_center;

    /** Not implemented. */
    State& operator=(const State&);

    void add_moves(Point p, Color c);

    void add_moves(Point p, Color c, Piece piece, unsigned adj_status);

    void check_local(unsigned local_value, Move mv, unsigned short piece_size);

    void compute_features();

    /** Equivalent to but faster than Board::get_move_info() */
    const MoveInfo& get_move_info(Move move) const;

    const PieceMap<bool>& get_pieces_considered() const;

    void init_move_list_with_local(Color c);

    void init_move_list_without_local(Color c);

    void play_pass();

    void play_nonpass(Move mv);

    bool check_move(Color c, const MovePoints& points,
                    unsigned& local_value);

    void update_move_list(Color c);

    bool check_symmetry_broken();

    void update_symmetry_broken(Move mv);
};

inline PlayerMove<Move> State::get_move(unsigned n) const
{
    ColorMove mv = m_bd.get_move(m_nu_moves_initial + n);
    return PlayerMove<Move>(mv.color.to_int(), mv.move);
}

inline const MoveInfo& State::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int()
                        < m_shared_const.board->get_board_const()
                          .get_nu_all_moves());
    return *(m_move_info_array + mv.to_int());
}

inline unsigned State::get_nu_moves() const
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_moves() >= m_nu_moves_initial);
    return m_bd.get_nu_moves() - m_nu_moves_initial;
}

inline unsigned State::get_to_play() const
{
    return m_bd.get_to_play().to_int();
}

inline bool State::skip_rave(Move mv) const
{
    LIBBOARDGAME_UNUSED(mv);
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
