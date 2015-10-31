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
typedef LIBPENTOBI_MCTS_FLOAT_TYPE Float;
#else
typedef float Float;
#endif

static_assert(std::is_floating_point<Float>::value, "");

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_FLOAT_H
