//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveMarker.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_MARKER_H
#define LIBPENTOBI_BASE_MOVE_MARKER_H

#include <array>
#include "Move.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class MoveMarker
{
public:
    MoveMarker()
    {
        clear();
    }

    bool operator[](Move mv) const
    {
        return m_a[mv.to_int()];
    }

    void set(Move mv)
    {
        m_a[mv.to_int()] = true;
    }

    void clear(Move mv)
    {
        m_a[mv.to_int()] = false;
    }

    template<class T>
    void set(const T& t)
    {
        for (Move mv : t)
            set(mv);
    }

    template<class T>
    void clear(const T& t)
    {
        for (Move mv : t)
            clear(mv);
    }

    void set()
    {
        m_a.fill(true);
    }

    void clear()
    {
        m_a.fill(false);
    }

private:
    array<bool, Move::range> m_a;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_MARKER_H
