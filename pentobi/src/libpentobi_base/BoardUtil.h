//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARDUTIL_H
#define LIBPENTOBI_BASE_BOARDUTIL_H

#include <iosfwd>
#include "Board.h"

namespace libpentobi_base {
namespace boardutil {

//-----------------------------------------------------------------------------

void dump(const Board& bd, ostream& out);

//-----------------------------------------------------------------------------

} // namespace boardutil
} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARDUTIL_H
