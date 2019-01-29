//-----------------------------------------------------------------------------
/** @file libboardgame_util/Options.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_OPTIONS_H
#define LIBBOARDGAME_UTIL_OPTIONS_H

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include "StringUtil.h"
#include "libboardgame_util/Compiler.h"

namespace libboardgame_util {

using namespace std;

//----------------------------------------------------------------------------

class OptionError
    : public runtime_error
{
    using runtime_error::runtime_error;
};

//----------------------------------------------------------------------------

/** Parser for command line options.
    The syntax of options is similar to GNU getopt. Options start with "--"
    and an option name. Options have optional short (single-character) names
    that are used with a single "-" and can be combined if all but the last
    option have no value. A single "--" stops option parsing to support
    non-option arguments that start with "-". */
class Options
{
public:
    /** Create options from arguments to main().
        @param argc
        @param argv
        @param specs A string per option that describes the option. The
        description is the long name of the option, followed by and optional
        '|' and a character for the short name of the option, followed  by an
        optional ':' if the option needs a value.
        @throws OptionError on error */
    Options(int argc, const char** argv, const vector<string>& specs);

    /** Overloaded version for con-const character strings in argv.
        Needed because the portable signature of main is (int, char**).
        argv is not modified by this constructor.  */
    Options(int argc, char** argv, const vector<string>& specs);

    ~Options();

    /** Check if an option exists in the command line arguments.
        @param name The (long) option name.  */
    bool contains(const string& name) const;

    string get(const string& name) const;

    string get(const string& name, const string& default_value) const;

    string get(const string& name, const char* default_value) const;

    /** Get option value.
        @param name The (long) option name.
        @throws OptionError If option does not exist or has the wrong type. */
    template<typename T>
    T get(const string& name) const;

    /** Get option value or default value.
        @param name The (long) option name.
        @param default_value A default value.
        @return The option value or the default value if the option does not
        exist. */
    template<typename T>
    T get(const string& name, const T& default_value) const;

    /** Remaining command line arguments that are not an option or an option
        value. */
    const vector<string>& get_args() const;

private:
    set<string> m_names;

    vector<string> m_args;

    map<string, string> m_map;

    void check_name(const string& name) const;
};

template<typename T>
T Options::get(const string& name) const
{
    T t;
    if (! from_string(get(name), t))
        throw OptionError("Option --" + name + " needs type "
                          + get_type_name(t));
    return t;
}

template<typename T>
T Options::get(const string& name, const T& default_value) const
{
    if (! contains(name))
        return default_value;
    return get<T>(name);
}

inline const vector<string>& Options::get_args() const
{
    return m_args;
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_OPTIONS_H
