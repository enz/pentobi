//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Float.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_FLOAT_H
#define LIBPENTOBI_MCTS_FLOAT_H

#include <limits>

namespace libpentobi_mcts {

using namespace std;

//-----------------------------------------------------------------------------

#ifdef LIBPENTOBI_MCTS_FLOAT_TYPE
typedef LIBPENTOBI_MCTS_FLOAT_TYPE Float;
#else
typedef float Float;
#endif

static_assert(! numeric_limits<Float>::is_exact, "");

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_FLOAT_H
