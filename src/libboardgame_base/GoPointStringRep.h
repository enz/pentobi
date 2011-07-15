//-----------------------------------------------------------------------------
/** @file GoPointStringRep.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GO_POINT_STRING_REP_H
#define LIBBOARDGAME_BASE_GO_POINT_STRING_REP_H

#include <iosfwd>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Go-style string representation of points.
    A1 is at the lower left corner, the letter I is traditionally not used for
    x coordinates if the board size is greater than eight to avoid confusion
    with the letter J. */
struct GoPointStringRep
{
    static const unsigned int max_size = 25;

    static bool read(istream& in, unsigned int max_size, unsigned int& x,
                     unsigned int& y);

    static void write(ostream& out, unsigned int x, unsigned int y);
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GO_POINT_STRING_REP_H
