//-----------------------------------------------------------------------------
/** @file libboardgame_base/Grid.h */
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
    The elements must be default-constructible.
    @tparam P An instantiation of boardgame::Point
    @tparam T The element type. */
template<class P, typename T>
class Grid
{
public:
    typedef P Point;

    Grid();

    explicit Grid(unsigned int sz);

    Grid(unsigned int sz, const T& val);

    bool operator==(const Grid& grid) const;

    void init(unsigned int sz);

    void init(unsigned int sz, const T& val);

    unsigned int get_size() const;

    T& operator[](const Point& p);

    const T& operator[](const Point& p) const;

    /** Fill all on-board points for a given board size with a value. */
    void fill(const T& val);

    ostream& write(ostream& out) const;

private:
    static const unsigned int value_uninitialized = Point::max_size + 1;

    unsigned int m_sz;

    T m_a[Point::range_onboard_end];

    bool is_initialized() const;
};

template<class P, typename T>
inline Grid<P, T>::Grid()
{
#if LIBBOARDGAME_DEBUG
    m_sz = value_uninitialized;
#endif
}

template<class P, typename T>
inline Grid<P, T>::Grid(unsigned int sz)
{
    init(sz);
}

template<class P, typename T>
inline Grid<P, T>::Grid(unsigned int sz, const T& val)
{
    init(sz, val);
}

template<class P, typename T>
bool Grid<P, T>::operator==(const Grid& grid) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(grid.is_initialized());
    LIBBOARDGAME_ASSERT(m_sz == grid.get_size());
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
        if (operator[](*i) != grid[*i])
            return false;
    return true;
}

template<class P, typename T>
inline T& Grid<P, T>::operator[](const Point& p)
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_a[p.to_int()];
}

template<class P, typename T>
inline const T& Grid<P, T>::operator[](const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_a[p.to_int()];
}

template<class P, typename T>
inline void Grid<P, T>::fill(const T& val)
{
    LIBBOARDGAME_ASSERT(is_initialized());
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
        operator[](*i) = val;
}

template<class P, typename T>
unsigned int Grid<P, T>::get_size() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_sz;
}

template<class P, typename T>
void Grid<P, T>::init(unsigned int sz)
{
    LIBBOARDGAME_ASSERT(sz >= 1);
    LIBBOARDGAME_ASSERT(sz <= Point::max_size);
    m_sz = sz;
}

template<class P, typename T>
inline void Grid<P, T>::init(unsigned int sz, const T& val)
{
    init(sz);
    fill(val);
}

template<class P, typename T>
bool Grid<P, T>::is_initialized() const
{
    return m_sz < value_uninitialized;
}

template<class P, typename T>
ostream& Grid<P, T>::write(ostream& out) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    ostringstream buffer;
    size_t max_len = 0;
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
    {
        buffer.str("");
        buffer << operator[](*i);
        max_len = max(max_len, buffer.str().length());
    }
    for (unsigned int y = m_sz - 1; ; --y)
    {
        for (unsigned int x = 0; x < m_sz; ++x)
        {
            out << setw(int(max_len)) << operator[](Point(x, y));
            if (x < m_sz - 1)
                out << ' ';
        }
        out << '\n';
        if (y == 0)
            break;
    }
    return out;
}

//-----------------------------------------------------------------------------

template<class P, typename T>
ostream& operator<<(ostream& out, const Grid<P, T>& grid)
{
    return grid.write(out);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GRID_H
