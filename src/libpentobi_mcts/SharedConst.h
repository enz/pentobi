//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/SharedConst.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_SHARED_CONST_H
#define LIBPENTOBI_MCTS_SHARED_CONST_H

#include "libpentobi_base/Board.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_util::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::MoveMarker;
using libpentobi_base::PieceMap;
using libpentobi_base::PrecompMoves;
using libpentobi_base::SymmetricPoints;

//-----------------------------------------------------------------------------

/** Constant data shared between the search states. */
struct SharedConst
{
    /** Precomputed moves additionally constrained by moves that are
        non-forbidden at root position. */
    ColorMap<PrecompMoves> precomp_moves;

    /** The game board.
        Contains the current position. */
    const Board* board;

    /** The color to play at the root of the search. */
    const Color& to_play;

    bool avoid_symmetric_draw;

    /** Lookup table for symmetric points (only used in Duo and Trigon). */
    SymmetricPoints symmetric_points;

    /** Precomputed information if move is forbidden at the start position (and
        therefore in all positions in the search). */
    ColorMap<MoveMarker> is_forbidden_at_root;

    /** Minimum total number of pieces on the board where all pieces are
        considered until the rest of the simulation. */
    unsigned min_move_all_considered;

    /** Precomputed lists of considered pieces depending on the total number
        of pieces on the board.
        Only initialized for numbers greater than or equal to the number in the
        root position and less than min_move_all_considered.
        Contains pointers to unique values such that the comparison of the
        lists can be done by comparing the pointers to the lists. */
    array<const PieceMap<bool>*, Board::max_game_moves> is_piece_considered;

    /** List of unique values for is_piece_considered. */
    ArrayList<PieceMap<bool>, Board::max_game_moves> is_piece_considered_list;

    /** Precomputed lists of considered pieces if all pieces are enforced to be
        considered (because using the restricted set of pieces would generate
        no moves). */
    PieceMap<bool> is_piece_considered_all;


    SharedConst(const Color& to_play);

    void init(bool is_followup);
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_SHARED_CONST_H
