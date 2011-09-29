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

    explicit FullGrid(const Geometry<P>& geometry);

    FullGrid(const Geometry<P>& geometry, const T& val);

    void init(const Geometry<P>& geometry);

    const Geometry<P>& get_geometry() const;

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
    const Geometry<P>* m_geometry;

    T m_a[Point::range];

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<class P, typename T>
inline FullGrid<P, T>::FullGrid()
{
#if LIBBOARDGAME_DEBUG
    m_geometry = 0;
#endif
}

template<class P, typename T>
inline FullGrid<P, T>::FullGrid(const Geometry<P>& geometry)
{
    init(geometry);
}

template<class P, typename T>
inline FullGrid<P, T>::FullGrid(const Geometry<P>& geometry, const T& val)
{
    init(geometry);
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
    LIBBOARDGAME_ASSERT(m_geometry->get_size() == grid.m_geometry->get_size());
    for (typename Geometry<P>::Iterator i(*m_geometry); i; ++i)
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
    for (typename Geometry<P>::Iterator i(*m_geometry); i; ++i)
        m_a[i->to_int()] = val;
}

template<class P, typename T>
const Geometry<P>& FullGrid<P, T>::get_geometry() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return *m_geometry;
}

template<class P, typename T>
inline T& FullGrid<P, T>::get_onboard(const Point& p)
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_onboard(m_geometry->get_size()));
    return operator[](p);
}

template<class P, typename T>
inline const T& FullGrid<P, T>::get_onboard(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_onboard(m_geometry->get_size()));
    return operator[](p);
}

template<class P, typename T>
void FullGrid<P, T>::init(const Geometry<P>& geometry)
{
    m_geometry = &geometry;
}

template<class P, typename T>
bool FullGrid<P, T>::is_initialized() const
{
    return m_geometry != 0;
}

template<class P, typename T>
ostream& FullGrid<P, T>::write(ostream& out) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    ostringstream buffer;
    size_t max_len = 0;
    for (typename Geometry<P>::Iterator i(*m_geometry); i; ++i)
    {
        buffer.str("");
        buffer << m_a[i->to_int()];
        max_len = max(max_len, buffer.str().length());
    }
    unsigned int sz = m_geometry->get_size();
    for (int y = sz - 1; y >= 0; --y)
    {
        for (int x = 0; x < sz; ++x)
        {
            out << setw(int(max_len)) << m_a[Point(x, y).to_int()];
            if (x < sz - 1)
                out << ' ';
        }
        out << '\n';
    }
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_FULL_GRID_H
