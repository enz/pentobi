//-----------------------------------------------------------------------------
/** @file libboardgame_base/Grid.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GRID_H
#define LIBBOARDGAME_BASE_GRID_H

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include "Geometry.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

template<class T>
string grid_to_string(const T& grid, const Geometry<typename T::Point>& geo)
{
    ostringstream buffer;
    size_t max_len = 0;
    for (auto p : geo)
    {
        buffer.str("");
        buffer << grid[p];
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
            auto p = geo.get_point(x, y);
            if (! p.is_null())
                buffer << setw(int(max_len)) << grid[p];
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

template<class P, typename T> class GridExt;

/** Elements assigned to on-board points.
    The elements must be default-constructible. This class is a POD if the
    element type is a POD.
    @tparam P An instantiation of Point (or compatible class)
    @tparam T The element type. */
template<class P, typename T>
class Grid
{
    friend class GridExt<P, T>; // for GridExt::copy_from(Grid)

public:
    using Point = P;

    using Geometry = libboardgame_base::Geometry<P>;


    T& operator[](const Point& p);

    const T& operator[](const Point& p) const;

    /** Fill all on-board points for a given geometry with a value. */
    void fill(const T& val, const Geometry& geo);

    /** Fill points with a value. */
    void fill_all(const T& val);

    string to_string(const Geometry& geo) const;

    void copy_from(const Grid& grid, const Geometry& geo);

    /** Specialized version for trivially copyable elements.
        Can be used instead of copy_from if the compiler is not smart enough to
        figure out that it can use memcpy.
        @pre std::is_trivially_copyable<T>::value */
    void memcpy_from(const Grid& grid, const Geometry& geo);

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
inline void Grid<P, T>::copy_from(const Grid& grid, const Geometry& geo)
{
    copy(grid.m_a, grid.m_a + geo.get_range(), m_a);
}

template<class P, typename T>
inline void Grid<P, T>::fill(const T& val, const Geometry& geo)
{
    std::fill(m_a, m_a + geo.get_range(), val);
}

template<class P, typename T>
inline void Grid<P, T>::fill_all(const T& val)
{
    std::fill(m_a, m_a + Point::range_onboard, val);
}

template<class P, typename T>
void Grid<P, T>::memcpy_from(const Grid& grid, const Geometry& geo)
{
    static_assert(is_trivially_copyable<T>::value);
    memcpy(&m_a, grid.m_a, geo.get_range() * sizeof(T));
}

template<class P, typename T>
string Grid<P, T>::to_string(const Geometry& geo) const
{
    return grid_to_string(*this, geo);
}

//-----------------------------------------------------------------------------

/** Like Grid, but allows Point::null() as index. */
template<class P, typename T>
class GridExt
{
public:
    using Point = P;

    using Geometry = libboardgame_base::Geometry<P>;


    T& operator[](const Point& p) { return m_a[p.to_int()]; }

    const T& operator[](const Point& p) const { return m_a[p.to_int()]; }

    /** Fill all on-board points for a given geometry with a value. */
    void fill(const T& val, const Geometry& geo);

    /** Fill points with a value. */
    void fill_all(const T& val);

    string to_string(const Geometry& geo) const;

    void copy_from(const Grid<P, T>& grid, const Geometry& geo);

    void copy_from(const GridExt& grid, const Geometry& geo);

private:
    T m_a[Point::range];
};


template<class P, typename T>
inline void GridExt<P, T>::fill(const T& val, const Geometry& geo)
{
    std::fill(m_a, m_a + geo.get_range(), val);
}

template<class P, typename T>
inline void GridExt<P, T>::fill_all(const T& val)
{
    std::fill(m_a, m_a + Point::range, val);
}

template<class P, typename T>
inline void GridExt<P, T>::copy_from(const Grid<P, T>& grid,
                                     const Geometry& geo)
{
    copy(grid.m_a, grid.m_a + geo.get_range(), m_a);
}

template<class P, typename T>
inline void GridExt<P, T>::copy_from(const GridExt& grid,
                                          const Geometry& geo)
{
    copy(grid.m_a, grid.m_a + geo.get_range(), m_a);
}

template<class P, typename T>
string GridExt<P, T>::to_string(const Geometry& geo) const
{
    return grid_to_string(*this, geo);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GRID_H
