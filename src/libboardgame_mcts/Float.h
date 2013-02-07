//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Float.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_FLOAT_H
#define LIBBOARDGAME_MCTS_FLOAT_H

#include <limits>

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** @typedef Float
    The floating type used for mean values and counts in the Monte-Carlo tree
    search.
    The default type is @c float for a reduced node size and performance gains
    (especially on 32-bit systems). However, using @c float sets a practical
    limit on the number of simulations before the count and mean values go into
    saturation. This maximum is given by 2^d-1 with d being the digits in the
    mantissa (=23 for IEEE 754 float's). The search will terminate when this
    number is reached. For longer searches, the code should be compiled with
    LIBBOARDGAME_MCTS_FLOAT_TYPE set to @c double. */

#ifdef LIBBOARDGAME_MCTS_FLOAT_TYPE
typedef LIBBOARDGAME_MCTS_FLOAT_TYPE Float;
#else
typedef float Float;
#endif

static_assert(! numeric_limits<Float>::is_exact, "");

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_FLOAT_H
