//-----------------------------------------------------------------------------
/** @file libboardgame_base/StringRep.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "StringRep.h"

#include <cstdio>
#include <iostream>
#include "StringUtil.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

bool StdStringRep::read(string::const_iterator begin,
                        string::const_iterator end, unsigned width,
                        unsigned height, unsigned& x, unsigned& y) const
{
    auto p = begin;
    while (p != end && isspace(*p) != 0)
        ++p;
    bool read_x = false;
    x = 0;
    int c;
    while (p != end && isalpha(*p) != 0)
    {
        c = tolower(*(p++));
        if (c < 'a' || c > 'z')
            return false;
        x = 26 * x + static_cast<unsigned>(c - 'a' + 1);
        if (x > width)
            return false;
        read_x = true;
    }
    if (! read_x)
        return false;
    --x;
    bool read_y = false;
    y = 0;
    while (p != end && isdigit(*p) != 0)
    {
        c = *(p++);
        y = 10 * y + static_cast<unsigned>((c - '0'));
        if (y > height)
            return false;
        read_y = true;
    }
    if (! read_y)
        return false;
    y = height - y;
    while (p != end)
        if (isspace(*(p++)) == 0)
            return false;
    return true;
}

void StdStringRep::write(
        ostream& out, unsigned x, unsigned y, [[maybe_unused]] unsigned width,
        unsigned height) const
{
    out << get_letter_coord(x) << (height - y);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
