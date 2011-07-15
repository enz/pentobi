//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOOK_H
#define LIBPENTOBI_BASE_BOOK_H

#include <iostream>
#include "Board.h"
#include "Game.h"
#include "Tree.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libpentobi_base {

using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

class Book
{
public:
    static double get_value(const Tree& tree, const Node& node);

    static void get_value_and_count(const Tree& tree, const Node& node,
                                    double& value, unsigned int& count);

    static const Node* select_child(RandomGenerator& random, const Board& bd,
                                    Color c, const Tree& tree,
                                    const Node& node, double delta,
                                    double max_delta);

    static void set_value_and_count(Game& game, double value, double count);

    void load(istream& in);

    Move genmove(const Board& bd, Color c, double delta, double max_delta);

    const Tree& get_tree() const;

private:
    Tree m_tree;

    RandomGenerator m_random;
};

inline const Tree& Book::get_tree() const
{
    return m_tree;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOOK_H
