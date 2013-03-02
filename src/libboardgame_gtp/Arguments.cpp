//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Arguments.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Arguments.h"

#include <cctype>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

void Arguments::check_size(size_t n) const
{
    if (get_size() == n)
        return;
    if (n == 0)
        throw Failure("no arguments allowed");
    else if (n == 1)
        throw Failure("command needs one argument");
    else
    {
        ostringstream msg;
        msg << "command needs " << n << " arguments";
        throw Failure(msg.str());
    }
}

void Arguments::check_size_less_equal(size_t n) const
{
    if (get_size() <= n)
        return;
    if (n == 1)
        throw Failure("command needs at most one argument");
    else
    {
        ostringstream msg;
        msg << "command needs at most " << n << " arguments";
        throw Failure(msg.str());
    }
}

CmdLineRange Arguments::get(size_t i) const
{
    if (i < get_size())
        return m_line.get_element(m_line.get_idx_name() + i + 1);
    ostringstream msg;
    msg << "missing argument " << (i + 1);
    throw Failure(msg.str());
}

string Arguments::get_tolower(size_t i) const
{
    string value = get(i);
    for (auto& c : value)
        c = static_cast<char>(tolower(c));
    return value;
}

string Arguments::get_tolower() const
{
    check_size(1);
    return get_tolower(0);
}

string Arguments::get_remaining_arg(size_t i) const
{
    size_t size = get_size();
    if (size == i + 1)
        return "";
    else if (size == i + 2)
        return get(i + 1);
    else
        return get_remaining_line(i);
}

CmdLineRange Arguments::get_remaining_line(size_t i) const
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
