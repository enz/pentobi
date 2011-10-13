//-----------------------------------------------------------------------------
/** @file Reader.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_READER_H
#define LIBBOARDGAME_SGF_READER_H

#include <iosfwd>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "libboardgame_util/Exception.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;
using boost::filesystem::path;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class Reader
{
public:
    class ReadError
        : public Exception
    {
    public:
        ReadError(const string& s);

        ReadError(const format& f);
    };

    Reader();

    virtual ~Reader() throw();

    virtual void on_begin_tree(bool is_root);

    virtual void on_end_tree(bool is_root);

    virtual void on_begin_node(bool is_root);

    virtual void on_end_node();

    virtual void on_property(const string& identifier,
                             const vector<string>& values);

    /** Read only the main variation.
        Reduces CPU time and memory if only the main variation is needed. */
    void set_read_only_main_variation(bool enable);

    /** @throws ReadError */
    void read(istream& in);

    /** @throws ReadError */
    void read(const path& file);

private:
    bool m_read_only_main_variation;

    bool m_is_in_main_variation;

    istream* m_in;

    /** Local variable in read_property().
        Reused for efficiency. */
    string m_identifier;

    /** Local variable in read_property().
        Reused for efficiency. */
    string m_value;

    /** Local variable in read_property().
        Reused for efficiency. */
    vector<string> m_values;

    void consume_char(char expected);

    void consume_whitespace();

    char peek();

    char read_char();

    void read_expected(char expected);

    void read_node(bool is_root);

    void read_property();

    void read_tree(bool is_root);
};

inline void Reader::set_read_only_main_variation(bool enable)
{
    m_read_only_main_variation = enable;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_READER_H
