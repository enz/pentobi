//-----------------------------------------------------------------------------
/** @file libpentobi_base/Point.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINT_H
#define LIBPENTOBI_BASE_POINT_H

#include "libboardgame_base/Point.h"
#include "libboardgame_base/SpShtStrRep.h"

//-----------------------------------------------------------------------------

namespace libpentobi_base {

using namespace std;
using libboardgame_base::SpShtStrRep;

//-----------------------------------------------------------------------------

/** Point (coordinate of on-board field) for Blokus game variants.
    Supports RectGeometry up to size 20 and TrigonGeometry up to edge size
    9. */
typedef libboardgame_base::Point<35 * 18, unsigned short, SpShtStrRep> Point;

//-----------------------------------------------------------------------------

class WritePoint
{
public:
    WritePoint(Point p, unsigned width)
      : m_p(p),
        m_width(width)
    { }

    ostream& write(ostream& out) const
    {
        m_p.write(out, m_width);
        return out;
    }

private:
    Point m_p;

    unsigned m_width;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

namespace std {

inline ostream& operator<<(ostream& out,
                           const libpentobi_base::WritePoint& write)
{
    write.write(out);
    return out;
}

} // namespace std

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_POINT_H
