//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Reader.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Reader.h"

#include <cctype>
#include <cstdio>
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_sgf {

using namespace std;
using boost::filesystem::ifstream;

//-----------------------------------------------------------------------------

Reader::ReadError::ReadError(const string& s)
    : Exception(s)
{
}

Reader::ReadError::ReadError(const format& f)
    : Exception(f)
{
}

//-----------------------------------------------------------------------------

Reader::Reader()
  : m_read_only_main_variation(false)
{
}

Reader::~Reader() throw()
{
}

void Reader::consume_char(char expected)
{
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(expected);
    char c = read_char();
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(c);
    LIBBOARDGAME_ASSERT(c == expected);
}

void Reader::consume_whitespace()
{
    char c;
    while (isspace(c = peek()))
        consume_char(c);
}

void Reader::on_begin_node(bool is_root)
{
    // Default implementation does nothing
    LIBBOARDGAME_UNUSED(is_root);
}

void Reader::on_begin_tree(bool is_root)
{
    // Default implementation does nothing
    LIBBOARDGAME_UNUSED(is_root);
}

void Reader::on_end_node()
{
    // Default implementation does nothing
}

void Reader::on_end_tree(bool is_root)
{
    // Default implementation does nothing
    LIBBOARDGAME_UNUSED(is_root);
}

void Reader::on_property(const string& identifier,
                         const vector<string>& values)
{
    // Default implementation does nothing
    LIBBOARDGAME_UNUSED(identifier);
    LIBBOARDGAME_UNUSED(values);
}

char Reader::peek()
{
    int c = m_in->peek();
    if (c == EOF)
        throw ReadError("Unexpected end of input");
    return char(c);
}

void Reader::read(istream& in, bool check_single_tree)
{
    m_in = &in;
    m_is_in_main_variation = true;
    consume_whitespace();
    read_tree(true);
    if (check_single_tree)
    {
        while (true)
        {
            int c = m_in->peek();
            if (c == EOF)
                break;
            else if (c == '(')
                throw ReadError("Input has multiple game trees");
            else if (isspace(c))
                consume_char(c);
            else
                throw ReadError("Extra characters after end of tree.");
        }
    }
}

void Reader::read(const path& file, bool check_single_tree)
{
    ifstream in(file);
    if (! in)
        throw ReadError(format("Could not open '%1%'") % file);
    try
    {
        read(in, check_single_tree);
    }
    catch (const ReadError& e)
    {
        throw ReadError(format("Could not read '%1%': %2%") % file % e.what());
    }
}

char Reader::read_char()
{
    int c = m_in->get();
    if (c == EOF)
        throw ReadError("Unexpected end of SGF stream");
    return char(c);
}

void Reader::read_expected(char expected)
{
    if (read_char() != expected)
        throw ReadError(format("Expected '%1%'") % expected);
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
        m_identifier.clear();
        while (peek() != '[')
            m_identifier += read_char();
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
        on_property(m_identifier, m_values);
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
        else if (c == ';')
        {
            read_node(is_root);
            is_root = false;
        }
        else if (c == '(')
            read_tree(false);
    }
    read_expected(')');
    m_is_in_main_variation = false;
    on_end_tree(was_root);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
