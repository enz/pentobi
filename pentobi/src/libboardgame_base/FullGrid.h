//-----------------------------------------------------------------------------
/** @file libboardgame_base/FullGrid.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_FULL_GRID_H
#define LIBBOARDGAME_BASE_FULL_GRID_H

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
class FullGrid
{
public:
    typedef P Point;

    FullGrid();

    explicit FullGrid(unsigned int sz);

    FullGrid(unsigned int sz, const T& val);

    void init(unsigned int sz);

    unsigned int get_size() const;

    T& operator[](const Point& p);

    const T& operator[](const Point& p) const;

    T& get_onboard(const Point& p);

    const T& get_onboard(const Point& p) const;

    /** Fill all on-board points with a value. */
    void fill_onboard(const T& val);

    /** Fill all on-board and off-board points with a value. */
    void fill_all(const T& val);

    bool equals_onboard(const FullGrid& grid) const;

    ostream& write(ostream& out) const;

private:
    static const unsigned int value_uninitialized = Point::max_size + 1;

    unsigned int m_sz;

    T m_a[Point::range];

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<class P, typename T>
inline FullGrid<P, T>::FullGrid()
{
#if LIBBOARDGAME_DEBUG
    m_sz = value_uninitialized;
#endif
}

template<class P, typename T>
inline FullGrid<P, T>::FullGrid(unsigned int sz)
{
    init(sz);
}

template<class P, typename T>
inline FullGrid<P, T>::FullGrid(unsigned int sz, const T& val)
{
    init(sz);
    fill_all(val);
}

template<class P, typename T>
inline T& FullGrid<P, T>::operator[](const Point& p)
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_a[p.to_int_not_null()];
}

template<class P, typename T>
inline const T& FullGrid<P, T>::operator[](const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_a[p.to_int_not_null()];
}

template<class P, typename T>
bool FullGrid<P, T>::equals_onboard(const FullGrid& grid) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(grid.is_initialized());
    LIBBOARDGAME_ASSERT(m_sz == grid.get_size());
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
        if (m_a[i->to_int()] != grid.m_a[i->to_int()])
            return false;
    return true;
}

template<class P, typename T>
inline void FullGrid<P, T>::fill_all(const T& val)
{
    for (unsigned int i = 0; i < Point::range; ++i)
        m_a[i] = val;
}

template<class P, typename T>
inline void FullGrid<P, T>::fill_onboard(const T& val)
{
    LIBBOARDGAME_ASSERT(is_initialized());
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
        m_a[i->to_int()] = val;
}

template<class P, typename T>
inline T& FullGrid<P, T>::get_onboard(const Point& p)
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return operator[](p);
}

template<class P, typename T>
inline const T& FullGrid<P, T>::get_onboard(const Point& p) const
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return operator[](p);
}

template<class P, typename T>
unsigned int FullGrid<P, T>::get_size() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_sz;
}

template<class P, typename T>
void FullGrid<P, T>::init(unsigned int sz)
{
    LIBBOARDGAME_ASSERT(sz >= 1);
    LIBBOARDGAME_ASSERT(sz <= Point::max_size);
    m_sz = sz;
}

template<class P, typename T>
bool FullGrid<P, T>::is_initialized() const
{
    return m_sz < value_uninitialized;
}

template<class P, typename T>
ostream& FullGrid<P, T>::write(ostream& out) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    ostringstream buffer;
    size_t max_len = 0;
    for (typename Geometry<P>::Iterator i(m_sz); i; ++i)
    {
        buffer.str("");
        buffer << m_a[i->to_int()];
        max_len = max(max_len, buffer.str().length());
    }
    for (int y = m_sz - 1; y >= 0; --y)
    {
        for (int x = 0; x < m_sz; ++x)
        {
            out << setw(int(max_len)) << m_a[Point(x, y).to_int()];
            if (x < m_sz - 1)
                out << ' ';
        }
        out << '\n';
    }
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_FULL_GRID_H
