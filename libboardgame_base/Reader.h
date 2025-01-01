//-----------------------------------------------------------------------------
/** @file libboardgame_base/Reader.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_READER_H
#define LIBBOARDGAME_BASE_READER_H

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

class Reader
{
public:
    class ReadError
        : public runtime_error
    {
        using runtime_error::runtime_error;
    };

    virtual ~Reader() = default;

    virtual void on_begin_tree(bool is_root);

    virtual void on_end_tree(bool is_root);

    virtual void on_begin_node(bool is_root);

    virtual void on_end_node();

    virtual void on_property(const string& id, const vector<string>& values);

    /** Read a game tree from a stream.
        @param in The input stream containing the SGF game tree(s).
        @param check_single_tree If true, the caller does not want to
        handle multi-tree SGF files and a ReadError will be thrown if
        non-whitespace characters follow after the first tree before the end of
        the stream.
        @return true, if there are more trees to read in the stream.
        @throws ReadError */
    bool read(istream& in, bool check_single_tree = true);

    void read(const string& file);

private:
    istream* m_in;

    /** Local variable in read_property().
        Reused for efficiency. */
    string m_id;

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

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_READER_H
