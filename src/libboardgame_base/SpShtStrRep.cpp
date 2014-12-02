//-----------------------------------------------------------------------------
/** @file libboardgame_base/SpShtStrRep.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SpShtStrRep.h"

#include <cstdio>
#include <iostream>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/StringUtil.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_base {

using libboardgame_util::get_letter_coord;

//-----------------------------------------------------------------------------

bool SpShtStrRep::read(istream& in, unsigned width, unsigned height,
                       unsigned& x, unsigned& y)
{
    int c;
    while (true)
    {
        c = in.peek();
        if (c == EOF || ! isspace(c))
            break;
        in.get();
    }
    bool read_x = false;
    x = 0;
    while (true)
    {
        c = in.peek();
        if (c == EOF || ! isalpha(c))
            break;
        c = tolower(in.get());
        if (c < 'a' || c > 'z')
            return false;
        x = 26 * x + (c - 'a' + 1);
        read_x = true;
    }
    if (! read_x)
        return false;
    --x;
    if (x >= width)
        return false;
    c = in.peek();
    if (c < '0' || c > '9')
        return false;
    in >> y;
    if (! in || y > height + 1)
        return false;
    y = height - y;
    c = in.peek();
    if (c == EOF)
    {
        in.clear();
        return true;
    }
    if (isspace(c))
        return true;
    return false;
}

void SpShtStrRep::write(ostream& out, unsigned x, unsigned y, unsigned width,
                        unsigned height)
{
    LIBBOARDGAME_UNUSED(width);
    out << get_letter_coord(x) << (height - y);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
