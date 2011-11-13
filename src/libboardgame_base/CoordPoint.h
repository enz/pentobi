//-----------------------------------------------------------------------------
/** @file libboardgame_base/CoordPoint.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_COORD_POINT_H
#define LIBBOARDGAME_BASE_COORD_POINT_H

#include <limits>
#include <iosfwd>
#include <vector>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Point stored as x,y coordinates. */
struct CoordPoint
{
    int x;

    int y;

    static bool is_onboard(int x, int y, unsigned int width,
                           unsigned int height);

    static CoordPoint null();

    CoordPoint();

    CoordPoint(int x, int y);

    bool operator==(const CoordPoint& p) const;

    bool operator!=(const CoordPoint& p) const;

    bool operator<(const CoordPoint& p) const;

    CoordPoint operator+(const CoordPoint& p) const;

    bool is_null() const;

    bool is_onboard(unsigned int width, unsigned int height) const;

    /** Find the width and height of the bounding box. */
    template<typename T>
    static void find_width_height(T begin, T end, unsigned int& width,
                                  unsigned int& height);
};

inline CoordPoint::CoordPoint()
{
}

inline CoordPoint::CoordPoint(int x, int y)
{
    this->x = x;
    this->y = y;
}

inline bool CoordPoint::operator==(const CoordPoint& p) const
{
    return x == p.x && y == p.y;
}

inline bool CoordPoint::operator<(const CoordPoint& p) const
{
    if (y != p.y)
        return (y < p.y);
    return (x < p.x);
}

inline bool CoordPoint::operator!=(const CoordPoint& p) const
{
    return ! operator==(p);
}

inline CoordPoint CoordPoint::operator+(const CoordPoint& p) const
{
    return CoordPoint(x + p.x, y + p.y);
}

inline CoordPoint CoordPoint::null()
{
    return CoordPoint(numeric_limits<int>::max(), numeric_limits<int>::max());
}

template<typename T>
void CoordPoint::find_width_height(T begin, T end,
                                   unsigned int& width, unsigned int& height)
{
    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();
    for (auto i = begin; i != end; ++i)
    {
        if (i->x < min_x)
            min_x = i->x;
        if (i->x > max_x)
            max_x = i->x;
        if (i->y < min_y)
            min_y = i->y;
        if (i->y > max_y)
            max_y = i->y;
    }
    width = max_x - min_x + 1;
    height = max_y - min_y + 1;
}

inline bool CoordPoint::is_onboard(int x, int y, unsigned int width,
                                   unsigned int height)
{
    return (x >= 0 && x < static_cast<int>(width)
            && y >= 0 && y < static_cast<int>(height));
}

inline bool CoordPoint::is_onboard(unsigned int width,
                                   unsigned int height) const
{
    return is_onboard(x, y, width, height);
}

inline bool CoordPoint::is_null() const
{
    return x == numeric_limits<int>::max();
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const CoordPoint& p);

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_COORD_POINT_H
