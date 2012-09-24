//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Response.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Response.h"

#include <iostream>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

ostringstream Response::s_dummy;

Response::~Response() throw()
{
}

void Response::write(ostream& out, string& buffer) const
{
    buffer = m_stream.str();
    bool was_newline = false;
    for (auto i = buffer.begin(); i != buffer.end(); ++i)
    {
        bool is_newline =(*i == '\n');
        if (is_newline && was_newline)
            out << ' ';
        out << *i;
        was_newline = is_newline;
    }
    if (! was_newline)
        out << '\n';
    out << '\n';
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
