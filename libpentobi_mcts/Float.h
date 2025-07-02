//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Float.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_FLOAT_H
#define LIBPENTOBI_MCTS_FLOAT_H

#include <type_traits>

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

#ifdef LIBPENTOBI_MCTS_FLOAT_TYPE
using Float = LIBPENTOBI_MCTS_FLOAT_TYPE;
#else
using Float = float;
#endif

static_assert(std::is_floating_point_v<Float>);

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_FLOAT_H
