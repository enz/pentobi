//-----------------------------------------------------------------------------
/** @file libpentobi_base/ColorMap.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_COLOR_MAP_H
#define LIBPENTOBI_BASE_COLOR_MAP_H

#include <array>
#include "Color.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Container mapping a color to another element type.
    The elements must be default-constructible. This requirement is due to the
    fact that elements are stored in an array for efficient access by color
    index and arrays need default-constructible elements. */
template<typename T>
class ColorMap
{
public:
    ColorMap() = default;

    explicit ColorMap(const T& val);

    T& operator[](Color c);

    const T& operator[](Color c) const;

    void fill(const T& val);

private:
    array<T, Color::range> m_a;
};

template<typename T>
inline ColorMap<T>::ColorMap(const T& val)
{
    fill(val);
}

template<typename T>
inline T& ColorMap<T>::operator[](Color c)
{
    return m_a[c.to_int()];
}

template<typename T>
inline const T& ColorMap<T>::operator[](Color c) const
{
    return m_a[c.to_int()];
}

template<typename T>
void ColorMap<T>::fill(const T& val)
{
    m_a.fill(val);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_COLOR_MAP_H
