//-----------------------------------------------------------------------------
/** @file libpentobi_base/ColorMove.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_COLOR_MOVE_H
#define LIBPENTOBI_BASE_COLOR_MOVE_H

#include "Move.h"
#include "libpentobi_base/Color.h"

namespace libpentobi_base {

using namespace std;
using libpentobi_base::Color;

//-----------------------------------------------------------------------------

struct ColorMove
{
    Color color;

    Move move;

    static ColorMove null();

    ColorMove();

    ColorMove(Color c, Move mv);

    bool operator==(const ColorMove& mv) const;

    bool operator!=(const ColorMove& mv) const;

    bool is_null() const;

    bool is_pass() const;
};

inline ColorMove::ColorMove()
{
}

inline ColorMove::ColorMove(Color c, Move mv)
    : color(c),
      move(mv)
{
}

inline bool ColorMove::operator==(const ColorMove& mv) const
{
    // Test move first, color may be uninitialized if move is null
    return move == mv.move && color == mv.color;
}

inline bool ColorMove::operator!=(const ColorMove& mv) const
{
    return ! operator==(mv);
}

inline bool ColorMove::is_null() const
{
    return move.is_null();
}

inline bool ColorMove::is_pass() const
{
    return move.is_pass();
}

inline ColorMove ColorMove::null()
{
    return ColorMove(Color(), Move::null());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_COLOR_MOVE_H
