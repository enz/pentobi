//-----------------------------------------------------------------------------
/** @file libboardgame_base/CoordPoint.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_COORD_POINT_H
#define LIBBOARDGAME_BASE_COORD_POINT_H

#include <limits>
#include <iosfwd>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** %Point stored as x,y coordinates. */
struct CoordPoint
{
    int x;

    int y;

    static bool is_onboard(int x, int y, unsigned width, unsigned height);

    static CoordPoint null();

    CoordPoint() = default;

    CoordPoint(int x, int y);

    CoordPoint(unsigned x, unsigned y);

    bool operator==(CoordPoint p) const;

    bool operator!=(CoordPoint p) const;

    bool operator<(CoordPoint p) const;

    CoordPoint operator+(CoordPoint p) const;

    CoordPoint operator-(CoordPoint p) const;

    CoordPoint& operator+=(CoordPoint p);

    CoordPoint& operator-=(CoordPoint p);

    bool is_null() const;

    bool is_onboard(unsigned width, unsigned height) const;
};

inline CoordPoint::CoordPoint(int x, int y)
{
    this->x = x;
    this->y = y;
}

inline CoordPoint::CoordPoint(unsigned x, unsigned y)
{
    LIBBOARDGAME_ASSERT(x < numeric_limits<int>::max());
    LIBBOARDGAME_ASSERT(y < numeric_limits<int>::max());
    this->x = static_cast<int>(x);
    this->y = static_cast<int>(y);
}

inline bool CoordPoint::operator==(CoordPoint p) const
{
    return x == p.x && y == p.y;
}

inline bool CoordPoint::operator<(CoordPoint p) const
{
    if (y != p.y)
        return y < p.y;
    return x < p.x;
}

inline bool CoordPoint::operator!=(CoordPoint p) const
{
    return ! operator==(p);
}

inline CoordPoint CoordPoint::operator+(CoordPoint p) const
{
    return {x + p.x, y + p.y};
}

inline CoordPoint& CoordPoint::operator+=(CoordPoint p)
{
    *this = *this + p;
    return *this;
}

inline CoordPoint CoordPoint::operator-(CoordPoint p) const
{
    return {x - p.x, y - p.y};
}

inline CoordPoint& CoordPoint::operator-=(CoordPoint p)
{
    *this = *this - p;
    return *this;
}

inline CoordPoint CoordPoint::null()
{
    return {numeric_limits<int>::max(), numeric_limits<int>::max()};
}

inline bool CoordPoint::is_onboard(int x, int y, unsigned width,
                                   unsigned height)
{
    return x >= 0 && x < static_cast<int>(width)
            && y >= 0 && y < static_cast<int>(height);
}

inline bool CoordPoint::is_onboard(unsigned width, unsigned height) const
{
    return is_onboard(x, y, width, height);
}

inline bool CoordPoint::is_null() const
{
    return x == numeric_limits<int>::max();
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, CoordPoint p);

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_COORD_POINT_H
