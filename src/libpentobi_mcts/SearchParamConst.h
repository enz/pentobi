//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/SearchParamConst.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_SEARCH_PARAM_CONST_H
#define LIBPENTOBI_MCTS_SEARCH_PARAM_CONST_H

#include "Float.h"
#include "libpentobi_base/Board.h"
#include "libboardgame_mcts/PlayerMove.h"

namespace libpentobi_mcts {

using libboardgame_mcts::PlayerInt;
using libpentobi_base::Board;
using libpentobi_base::Color;

//-----------------------------------------------------------------------------

/** Optional compile-time parameters for libboardgame_mcts::Search.
    See libboardgame_mcts::SearchParamConstDefault for the meaning of the
    members. */
struct SearchParamConst
{
    using Float = libpentobi_mcts::Float;


    static constexpr PlayerInt max_players = 6;

    /** The maximum number of moves in a simulation.
        This needs to include pass moves because in the in-tree phase pass
        moves (Move::null()) are used. The game ends after all colors have
        passed in a row. Therefore, the maximum number of moves is reached in
        case that a piece move is followed by (Color::range-1) pass moves and
        an extra Color::range pass moves at the end. */
    static constexpr unsigned max_moves =
            Color::range * (Color::range * Board::max_pieces + 1);

#ifdef LIBBOARDGAME_MCTS_SINGLE_THREAD
    static constexpr bool multithread = false;
#else
    static constexpr bool multithread = true;
#endif

    static constexpr bool rave = true;

    static constexpr bool rave_dist_weighting = true;

    static constexpr bool use_lgr = true;

#ifdef PENTOBI_LOW_RESOURCES
    static constexpr size_t lgr_hash_table_size = (1 << 20);
#else
    static constexpr size_t lgr_hash_table_size = (1 << 21);
#endif

    static constexpr bool virtual_loss = true;

    static constexpr bool use_unlikely_change = true;

    static constexpr Float child_min_count = 3;

    static constexpr Float max_move_prior = 1;

    static constexpr Float tie_value = 0.5f;

    static constexpr Float prune_count_start = 16;

    static constexpr Float expansion_threshold = 1;

    static constexpr Float expansion_threshold_inc = 0.5f;

    static constexpr double expected_sim_per_sec = 100;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_SEARCH_PARAM_CONST_H
