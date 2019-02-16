//-----------------------------------------------------------------------------
/** @file libboardgame_base/Reader.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Reader.h"

#include <cctype>
#include <cstdio>
#include <fstream>
#include "libboardgame_base/Assert.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

namespace {

/** Replacement for std::isspace() that returns true only for whitespaces
    in the ASCII range. */
bool is_ascii_space(int c)
{
    return c >= 0 && c < 128 && isspace(c) != 0;
}

} // namespace

//-----------------------------------------------------------------------------

void Reader::consume_char([[maybe_unused]] char expected)
{
    [[maybe_unused]] char c = read_char();
    LIBBOARDGAME_ASSERT(c == expected);
}

void Reader::consume_whitespace()
{
    while (is_ascii_space(peek()))
        m_in->get();
}

void Reader::on_begin_node([[maybe_unused]] bool is_root)
{
    // Default implementation does nothing
}

void Reader::on_begin_tree([[maybe_unused]] bool is_root)
{
    // Default implementation does nothing
}

void Reader::on_end_node()
{
    // Default implementation does nothing
}

void Reader::on_end_tree([[maybe_unused]] bool is_root)
{
    // Default implementation does nothing
}

void Reader::on_property([[maybe_unused]] const string& id,
                         [[maybe_unused]] const vector<string>& values)
{
    // Default implementation does nothing
}

char Reader::peek()
{
    int c = m_in->peek();
    if (c == EOF)
        throw ReadError("Unexpected end of input");
    return char(c);
}

bool Reader::read(istream& in, bool check_single_tree)
{
    m_in = &in;
    m_is_in_main_variation = true;
    consume_whitespace();
    read_tree(true);
    while (true)
    {
        int c = m_in->peek();
        if (c == EOF)
            return false;
        if (c == '(')
        {
            if (check_single_tree)
                throw ReadError("Input has multiple game trees");
            return true;
        }
        if (is_ascii_space(c))
            m_in->get();
        else
            throw ReadError("Extra characters after end of tree.");
    }
}

void Reader::read(const string& file)
{
    ifstream in(file);
    if (! in)
        throw ReadError("Could not open '" + file + "'");
    try
    {
        read(in);
    }
    catch (const ReadError& e)
    {
        throw ReadError("Could not read '" + file + "': " + e.what());
    }
}

char Reader::read_char()
{
    int c = m_in->get();
    if (c == EOF)
        throw ReadError("Unexpected end of SGF stream");
    if (c == '\r')
    {
        // Convert CR+LF or single CR into LF
        if (peek() == '\n')
            m_in->get();
        return '\n';
    }
    return char(c);
}

void Reader::read_expected(char expected)
{
    if (read_char() != expected)
        throw ReadError(string("Expected '") + expected + "'");
}

void Reader::read_node(bool is_root)
{
    read_expected(';');
    if (! m_read_only_main_variation || m_is_in_main_variation)
        on_begin_node(is_root);
    while (true)
    {
        consume_whitespace();
        char c = peek();
        if (c == '(' || c == ')' || c == ';')
            break;
        read_property();
    }
    if (! m_read_only_main_variation || m_is_in_main_variation)
        on_end_node();
}

void Reader::read_property()
{
    if (m_read_only_main_variation && ! m_is_in_main_variation)
    {
        while (peek() != '[')
            read_char();
        while (peek() == '[')
        {
            consume_char('[');
            bool escape = false;
            while (peek() != ']' || escape)
            {
                char c = read_char();
                if (c == '\\' && ! escape)
                {
                    escape = true;
                    continue;
                }
                escape = false;
            }
            consume_char(']');
            consume_whitespace();
        }
    }
    else
    {
        m_id.clear();
        while (peek() != '[')
        {
            char c = read_char();
            if (! is_ascii_space(c))
                m_id += c;
        }
        m_values.clear();
        while (peek() == '[')
        {
            consume_char('[');
            m_value.clear();
            bool escape = false;
            while (peek() != ']' || escape)
            {
                char c = read_char();
                if (c == '\\' && ! escape)
                {
                    escape = true;
                    continue;
                }
                escape = false;
                m_value += c;
            }
            consume_char(']');
            consume_whitespace();
            m_values.push_back(m_value);
        }
        on_property(m_id, m_values);
    }
}

void Reader::read_tree(bool is_root)
{
    read_expected('(');
    on_begin_tree(is_root);
    bool was_root = is_root;
    while (true)
    {
        consume_whitespace();
        char c = peek();
        if (c == ')')
            break;
        if (c == ';')
        {
            read_node(is_root);
            is_root = false;
        }
        else if (c == '(')
            read_tree(false);
        else
            throw ReadError("Extra text before node");
    }
    read_expected(')');
    m_is_in_main_variation = false;
    on_end_tree(was_root);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
