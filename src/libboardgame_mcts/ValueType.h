//-----------------------------------------------------------------------------
/** @file ValueType.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_VALUE_TYPE_H
#define LIBBOARDGAME_MCTS_VALUE_TYPE_H

#include <limits>

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** @typedef ValueType
    The floating type used for mean values and counts in the Monte-Carlo tree
    search.
    The default type is @c float for a reduced node size and performance gains
    (especially on 32-bit systems). However, using @c float sets a practical
    limit on the number of simulations before the count and mean values go into
    saturation. This maximum is given by 2^d-1 with d being the digits in the
    mantissa (=23 for IEEE 754 float's). The search will terminate when this
    number is reached. For longer searches, the code should be compiled with
    LIBBOARDGAME_MCTS_VALUE_TYPE_DOUBLE set to @c double. */

#ifdef LIBBOARDGAME_MCTS_VALUE_TYPE
typedef LIBBOARDGAME_MCTS_VALUE_TYPE ValueType;
#else
typedef float ValueType;
#endif

static_assert(! numeric_limits<ValueType>::is_exact,
              "libboardgame_mcts::ValueType is not a floating type");

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_VALUE_TYPE_H
