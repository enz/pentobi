//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Arguments.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_ARGUMENTS_H
#define LIBBOARDGAME_GTP_ARGUMENTS_H

#if defined __GNUC__
#include <cxxabi.h>
#endif
#include "CmdLine.h"

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

/** Access arguments of command line. */
class Arguments
{
public:
    /** Constructor.
        @param line The command line (@ref libboardgame_doc_storesref) */
    Arguments(const CmdLine& line);

    ~Arguments() throw();

    /** Get argument.
        @param i Argument index starting with 0
        @return Argument value
        @throws Failure If no such argument */
    CmdLineRange get(size_t i) const;

    /** Get single argument.
        @return Argument value
        @throws Failure If no such argument or command has more than one
        arguments */
    CmdLineRange get() const;

    /** Get argument converted to lowercase.
        @param i Argument index starting with 0
        @return Copy of argument value converted to lowercase
        @throws Failure If no such argument */
    string get_tolower(size_t i) const;

    /** Get single argument converted to lowercase. */
    string get_tolower() const;

    /** Get argument converted to a type.
        The type must implement operator<<(istream)
        @param i Argument index starting with 0
        @return The converted argument
        @throws Failure If no such argument, or argument cannot be converted */
    template<typename T>
    T parse(size_t i) const;

    /** Get single argument converted to a type.
        The type must implement operator<<(istream)
        @return The converted argument
        @throws Failure If no such argument, or argument cannot be converted,
        or command has more than one arguments
    */
    template<typename T>
    T parse() const;

    /** Get argument converted to a type and check against a minum value.
        The type must implement operator<< and operator<
        @param i Argument index starting with 0
        @param min Minimum allowed value
        @return Argument value
        @throws Failure If no such argument, argument cannot be converted
        or smaller than the mimimum value
    */
    template<typename T>
    T parse_min(size_t i, T min) const;

    /** Get argument converted to a type and check against a range.
        The type must implement operator<< and operator<
        @param i Argument index starting with 0
        @param min Minimum allowed value
        @param max Maximum allowed value
        @return Argument value
        @throws Failure If no such argument, argument cannot be converted
        or not in range
    */
    template<typename T>
    T parse_min_max(size_t i, T min, T max) const;

    template<typename T>
    T parse_min_max(T min, T max) const;

    /** Check that command has no arguments.
        @throws Failure If command has arguments
    */
    void check_empty() const;

    /** Check number of arguments.
        @param n Expected number of arguments
        @throws Failure If command has a different number of arguments
    */
    void check_size(size_t n) const;

    /** Check maximum number of arguments.
        @param n Expected maximum number of arguments
        @throws Failure If command has more arguments
    */
    void check_size_less_equal(size_t n) const;

    /** Get argument line.
        Get all arguments as a line.
        No modfications to the line were made apart from trimmimg leading
        and trailing white spaces.
    */
    CmdLineRange get_line() const;

    /** Get number of arguments. */
    size_t get_size() const;

    /** Return remaining line after argument.
        @param i Argument index starting with 0
        @return The remaining line after the given argument, unmodified apart
        from leading and trailing whitespaces, which are trimmed. Quotation
        marks are not handled.
        @throws Failure If no such argument
    */
    CmdLineRange get_remaining_line(size_t i) const;

    /** Return remaining line after argument as an argument.
        This function can be used for parsing string parameter values as used
        by analyze commands of type 'param' by GoGui (http://gogui.sf.net).
        GoGui never quotes string parameter values, because it does not know if
        the parameter value is a single argument or a list that will be further
        parsed by the engine. This can cause problems if the argument value is
        an unquoted filename containing spaces. This function can be used to
        handle this situation. It uses the arguments that were detected with
        supports for quoting and returns en empty string if the number of
        arguments is i, the argument i + 1 if the number of arguments is i + 1,
        and get_remaining_line(i) otherwise.
        @param i Argument index starting with 0
        @return The remaining argument.
        @throws Failure If the number of arguments is less than i
    */
    string get_remaining_arg(size_t i) const;

private:
    const CmdLine& m_line;

    template<typename T>
    static string get_type_name();
};

inline Arguments::Arguments(const CmdLine& line)
    : m_line(line)
{
}

inline Arguments::~Arguments() throw()
{
}

inline void Arguments::check_empty() const
{
    check_size(0);
}

inline CmdLineRange Arguments::get() const
{
    check_size(1);
    return get(0);
}

inline CmdLineRange Arguments::get_line() const
{
    return m_line.get_trimmed_line_after_elem(m_line.get_idx_name());
}

inline size_t Arguments::get_size() const
{
    return m_line.get_elements().size() - m_line.get_idx_name() - 1;
}

template<typename T>
string Arguments::get_type_name()
{
#ifdef __GNUC__
    int status;
    auto name_ptr =
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    if (status == 0)
    {
        string result(name_ptr);
        free(name_ptr);
        return result;
    }
    else
        return typeid(T).name();
#else
    return typeid(T).name();
#endif
}

template<typename T>
T Arguments::parse() const
{
    check_size(1);
    return parse<T>(0);
}

template<typename T>
T Arguments::parse(size_t i) const
{
    string s = get(i);
    istringstream in(s);
    T result;
    in >> result;
    if (! in)
    {
        ostringstream msg;
        msg << "argument " << (i + 1) << " ('" << s
            << "') has invalid type (expected " << get_type_name<T>() << ")";
        throw Failure(msg.str());
    }
    return result;
}

template<typename T>
T Arguments::parse_min(std::size_t i, T min) const
{
    T result = parse<T>(i);
    if (result < min)
    {
        ostringstream msg;
        msg << "argument " << (i + 1) << " must be greater or equal " << min;
        throw Failure(msg.str());
    }
    return result;
}

template<typename T>
T Arguments::parse_min_max(T min, T max) const
{
    check_size(1);
    return parse_min_max<T>(0, min, max);
}

template<typename T>
T Arguments::parse_min_max(std::size_t i, T min, T max) const
{
    T result = parse_min(i, min);
    if (max < result)
    {
        ostringstream msg;
        msg << "argument " << (i + 1) << " must be less or equal " << max;
        throw Failure(msg.str());
    }
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_ARGUMENTS_H
