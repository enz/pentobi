//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Arguments.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Arguments.h"

#include <cctype>

namespace libboardgame_gtp {

//-----------------------------------------------------------------------------

void Arguments::check_size(unsigned n) const
{
    if (get_size() == n)
        return;
    if (n == 0)
        throw Failure("no arguments allowed");
    if (n == 1)
        throw Failure("command needs one argument");
    ostringstream msg;
    msg << "command needs " << n << " arguments";
    throw Failure(msg.str());
}

void Arguments::check_size_less_equal(unsigned n) const
{
    if (get_size() <= n)
        return;
    if (n == 1)
        throw Failure("command needs at most one argument");
    ostringstream msg;
    msg << "command needs at most " << n << " arguments";
    throw Failure(msg.str());
}

string Arguments::get_tolower(unsigned i) const
{
    auto value = get<string>(i);
    for (auto& c : value)
        c = static_cast<char>(tolower(c));
    return value;
}

string Arguments::get_tolower() const
{
    check_size(1);
    return get_tolower(0);
}

string_view Arguments::get_remaining_line(unsigned i) const
{
    if (i < get_size())
        return m_line.get_trimmed_line_after_elem(m_line.get_idx_name() + i
                                                  + 1);
    ostringstream msg;
    msg << "missing argument " << (i + 1);
    throw Failure(msg.str());
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
