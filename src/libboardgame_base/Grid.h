//-----------------------------------------------------------------------------
/** @file libboardgame_base/Grid.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GRID_H
#define LIBBOARDGAME_BASE_GRID_H

#include <algorithm>
#include <iomanip>
#include <sstream>
#include "Point.h"
#include "Geometry.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Elements assigned to on-board points.
    The elements must be default-constructible. This class is a POD if the
    element type is a POD.
    @tparam P An instantiation of libboardgame_base::Point (or compatible
    class)
    @tparam T The element type. */
template<class P, typename T>
class Grid
{
public:
    typedef P Point;

    typedef libboardgame_base::Geometry<P> Geometry;

    T& operator[](const Point& p);

    const T& operator[](const Point& p) const;

    /** Fill all on-board points for a given geometry with a value. */
    void fill(const T& val, const Geometry& geo);

    /** Fill points with a value. */
    void fill_all(const T& val);

    string to_string(const Geometry& geo) const;

    void copy_from(const Grid& grid, const Geometry& geo);

private:
    T m_a[Point::range_onboard];
};

template<class P, typename T>
inline T& Grid<P, T>::operator[](const Point& p)
{
    LIBBOARDGAME_ASSERT(! p.is_null());
    return m_a[p.to_int()];
}

template<class P, typename T>
inline const T& Grid<P, T>::operator[](const Point& p) const
{
    LIBBOARDGAME_ASSERT(! p.is_null());
    return m_a[p.to_int()];
}

template<class P, typename T>
inline void Grid<P, T>::fill(const T& val, const Geometry& geo)
{
    std::fill(m_a + Point::begin_onboard, m_a + geo.get_range(), val);
}

template<class P, typename T>
inline void Grid<P, T>::fill_all(const T& val)
{
    std::fill(m_a + Point::begin_onboard, m_a + Point::range_onboard, val);
}

template<class P, typename T>
inline void Grid<P, T>::copy_from(const Grid& grid, const Geometry& geo)
{
    copy(grid.m_a + Point::begin_onboard, grid.m_a + geo.get_range(),
         m_a + Point::begin_onboard);
}

template<class P, typename T>
string Grid<P, T>::to_string(const Geometry& geo) const
{
    ostringstream buffer;
    size_t max_len = 0;
    for (Point p : geo)
    {
        buffer.str("");
        buffer << (*this)[p];
        max_len = max(max_len, buffer.str().length());
    }
    buffer.str("");
    auto width = geo.get_width();
    auto height = geo.get_height();
    string empty(max_len, ' ');
    for (unsigned y = 0; y < height; ++y)
    {
        for (unsigned x = 0; x < width; ++x)
        {
            Point p = geo.get_point(x, y);
            if (! p.is_null())
                buffer << setw(int(max_len)) << (*this)[p];
            else
                buffer << empty;
            if (x < width - 1)
                buffer << ' ';
        }
        buffer << '\n';
    }
    return buffer.str();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GRID_H
