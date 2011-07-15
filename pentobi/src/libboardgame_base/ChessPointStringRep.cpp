//-----------------------------------------------------------------------------
/** @file ChessPointStringRep.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ChessPointStringRep.h"

#include <iostream>
#include <boost/algorithm/string/case_conv.hpp>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

using boost::algorithm::to_lower;

//-----------------------------------------------------------------------------

bool ChessPointStringRep::read(istream& in, unsigned int max_size,
                               unsigned int& x, unsigned int& y)
{
    string s;
    in >> s;
    size_t len = s.size();
    if (len >= 2 && len <= 3 && isalpha(s[0]) && isdigit(s[1])
        && (len == 2 || isdigit(s[2])))
    {
        to_lower(s);
        x = s[0] - 'a';
        if (x < max_size)
        {
            y = s[1] - '1';
            if (len == 3)
                y = (y + 1) * 10 + (s[2] - '1');
            if (y < max_size)
                return true;
        }
    }
    return false;
}

void ChessPointStringRep::write(ostream& out, unsigned int x, unsigned int y)
{
    LIBBOARDGAME_ASSERT(x <= 25);
    out << char('a' + x);
    int i = y;
    if (i >= 19)
    {
        out << '2';
        i -= 20;
    }
    else if (i >= 9)
    {
        out << '1';
        i -= 10;
    }
    out << char('1' + i);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
