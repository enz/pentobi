//-----------------------------------------------------------------------------
/** @file libboardgame_base/SpreadsheetStringRep.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SpreadsheetStringRep.h"

#include <cstdio>
#include <iostream>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_base {

using libboardgame_util::string_util::get_letter_coord;

//-----------------------------------------------------------------------------

bool SpreadsheetStringRep::read(istream& in, unsigned width, unsigned height,
                                unsigned& x, unsigned& y)
{
    int c;
    while (true)
    {
        c = in.peek();
        if (c == EOF || ! isspace(c))
            break;
        c = in.get();
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
    if (! in)
        return false;
    --y;
    if (y >= height)
        return false;
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

void SpreadsheetStringRep::write(ostream& out, unsigned x, unsigned y)
{
    out << get_letter_coord(x) << (y + 1);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
