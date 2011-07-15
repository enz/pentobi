//-----------------------------------------------------------------------------
/** @file libpentobi_base/Color.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Color.h"

#include <sstream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>

namespace libpentobi_base {

using namespace std;
using boost::algorithm::to_lower;
using boost::format;

//-----------------------------------------------------------------------------

Color::Color(const string& s)
{
    istringstream in(s);
    in >> *this;
    if (! in)
        throw InvalidString(str(format("Invalid color string '%1%'") % s));
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const Color& c)
{
    out << "Color" << (c.to_int() + 1);
    return out;
}

istream& operator>>(istream& in, Color& c)
{
    string s;
    in >> s;
    if (in)
    {
        to_lower(s);
        if (s == "color1" || s == "b" || s == "black")
        {
            c = Color(0);
            return in;
        }
        else if (s == "color2" || s == "w" || s == "white")
        {
            c = Color(1);
            return in;
        }
        else if (s == "color3")
        {
            c = Color(2);
            return in;
        }
        else if (s == "color4")
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
