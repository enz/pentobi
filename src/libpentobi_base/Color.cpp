//-----------------------------------------------------------------------------
/** @file libpentobi_base/Color.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Color.h"

#include <sstream>
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::to_lower;

//-----------------------------------------------------------------------------

Color::Color(const string& s)
{
    istringstream in(s);
    in >> *this;
    if (! in)
        throw InvalidString("Invalid color string '" + s + "'");
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const Color& c)
{
    out << (c.to_int() + 1);
    return out;
}

istream& operator>>(istream& in, Color& c)
{
    string s;
    in >> s;
    if (in)
    {
        s = to_lower(s);
        if (s == "1" || s == "b" || s == "black")
        {
            c = Color(0);
            return in;
        }
        else if (s == "2" || s == "w" || s == "white")
        {
            c = Color(1);
            return in;
        }
        else if (s == "3")
        {
            c = Color(2);
            return in;
        }
        else if (s == "4")
        {
            c = Color(3);
            return in;
        }
    }
    in.setstate(ios::failbit);
    return in;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
