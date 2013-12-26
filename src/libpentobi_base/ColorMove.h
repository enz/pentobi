//-----------------------------------------------------------------------------
/** @file libpentobi_base/ColorMove.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_COLOR_MOVE_H
#define LIBPENTOBI_BASE_COLOR_MOVE_H

#include "Move.h"
#include "libpentobi_base/Color.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

struct ColorMove
{
    Color color;

    Move move;

    /** Return a color move with a null move and an undefined color.
        Even if the color is logically not defined, it is still initialized
        (with Color(0)), such that this color move can be used in
        comparisons. If you are sure that the color is never used and don't
        want to initialize it for efficiency, use the default constructor
        and then assign only the move. */
    static ColorMove null();

    ColorMove() = default;

    ColorMove(Color c, Move mv);

    /** Equality operator.
        @pre move, color, mv.move, mv.color are initialized. */
    bool operator==(const ColorMove& mv) const;

    /** Inequality operator.
        @pre move, color, mv.move, mv.color are initialized. */
    bool operator!=(const ColorMove& mv) const;

    bool is_null() const;

    bool is_pass() const;

    bool is_regular() const;
};

inline ColorMove::ColorMove(Color c, Move mv)
    : color(c),
      move(mv)
{
}

inline bool ColorMove::operator==(const ColorMove& mv) const
{
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

inline bool ColorMove::is_regular() const
{
    return move.is_regular();
}

inline ColorMove ColorMove::null()
{
    return ColorMove(Color(0), Move::null());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_COLOR_MOVE_H
