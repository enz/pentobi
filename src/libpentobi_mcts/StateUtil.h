//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/StateUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_UTIL_H
#define LIBPENTOBI_MCTS_STATE_UTIL_H

#include "libpentobi_base/Board.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::Board;
using libpentobi_base::SymmetricPoints;

//-----------------------------------------------------------------------------

bool check_symmetry_broken(const Board& bd,
                           const SymmetricPoints& symmetric_points);

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_UTIL_H
