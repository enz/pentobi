//-----------------------------------------------------------------------------
/** @file libboardgame_base/MarkerWithCount.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_MARKER_WITH_COUNT_H
#define LIBBOARDGAME_BASE_MARKER_WITH_COUNT_H

#include "Marker.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** A point marker that also keeps track of the number of marked points.
    @tparam P An instantiation of boardgame::Point */
template<class P>
class MarkerWithCount
    : private Marker<P>
{
public:
    typedef P Point;

    MarkerWithCount();

    void clear();

    void set(Point p);

    bool toggle(Point p);

    int count() const;

private:
    int m_count;
};

template<class P>
inline MarkerWithCount<P>::MarkerWithCount()
{
    m_count = 0;
}

template<class P>
inline void MarkerWithCount<P>::clear()
{
    Marker<P>::clear();
    m_count = 0;
}

template<class P>
inline int MarkerWithCount<P>::count() const
{
    return m_count;
}

template<class P>
inline void MarkerWithCount<P>::set(Point p)
{
    Marker<P>::set(p);
    ++m_count;
}

template<class P>
inline bool MarkerWithCount<P>::toggle(Point p)
{
    if (Marker<P>::toggle(p))
    {
        ++m_count;
        return true;
    }
    else
    {
        --m_count;
        return false;
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_MARKER_WITH_COUNT_H
