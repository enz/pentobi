//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Reader.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_READER_H
#define LIBBOARDGAME_SGF_READER_H

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

class Reader
{
public:
    class ReadError
        : public runtime_error
    {
    public:
        ReadError(const string& s)
            : runtime_error(s)
        { }
    };

    Reader();

    virtual ~Reader();

    virtual void on_begin_tree(bool is_root);

    virtual void on_end_tree(bool is_root);

    virtual void on_begin_node(bool is_root);

    virtual void on_end_node();

    virtual void on_property(const string& id, const vector<string>& values);

    /** Read only the main variation.
        Reduces CPU time and memory if only the main variation is needed. */
    void set_read_only_main_variation(bool enable);

    /** Read a game tree from the file.
        @param in
        @param check_single_tree Throw an error if non-whitespace characters
        follow after the tree before the end of the stream. This is mainly
        useful to ensure that the input is not a SGF file with multiple game
        trees if the caller does not want to handle this case. If
        check_single_tree is false, you can call read() multiple times to read
        all game trees.
        @param[out] more_game_trees_left set to true if check_single_tree is
        false and there are more game trees to read.
        @throws ReadError */
    void read(istream& in, bool check_single_tree = true,
              bool* more_game_trees_left = nullptr);

    /** See read(istream&,bool) */
    void read(const string& file);

private:
    bool m_read_only_main_variation;

    bool m_is_in_main_variation;

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

inline void Reader::set_read_only_main_variation(bool enable)
{
    m_read_only_main_variation = enable;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_READER_H
