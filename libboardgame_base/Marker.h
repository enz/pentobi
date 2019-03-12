//-----------------------------------------------------------------------------
/** @file libboardgame_base/Marker.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_MARKER_H
#define LIBBOARDGAME_BASE_MARKER_H

#include <algorithm>
#include <limits>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** %Marker to mark points on board with fast operation to clear all marks.
    This marker is typically used in recursive fills or other loops to
    remember what points have already been visited.
    @tparam P An instantiation of Point */
template<class P>
class Marker
{
public:
    using Point = P;


    Marker() { reset(); }

    void clear();

    /** Mark a point.
        @return true if the point was already marked. */
    bool set(Point p);

    bool operator[](Point p) const { return m_a[p.to_int()] == m_current; }

    /** Set up for overflow test (for testing purposes only).
        The function is equivalent to calling reset() and then clear()
        nu_clear times. It allows a faster implementation of a unit test case
        that tests if the overflow is handled correctly, if clear() is called
        more than numeric_limits<unsigned>::max() times. */
    void setup_for_overflow_test(unsigned nu_clear);

private:
    unsigned m_current;

    unsigned m_a[Point::range];

    void reset();
};


template<class P>
inline void Marker<P>::clear()
{
    if (--m_current == 0)
        reset();
}

template<class P>
inline void Marker<P>::setup_for_overflow_test(unsigned nu_clear)
{
    reset();
    m_current -= nu_clear;
}

template<class P>
inline void Marker<P>::reset()
{
    m_current = numeric_limits<unsigned>::max() - 1;
    fill(m_a, m_a + Point::range, numeric_limits<unsigned>::max());
}

template<class P>
inline bool Marker<P>::set(Point p)
{
    auto& a = m_a[p.to_int()];
    if (a == m_current)
        return true;
    a = m_current;
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_MARKER_H
