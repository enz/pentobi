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

/** Opening book.
    Opening books are stored as treed in SGF files. This class can handle
    two variants of opening books. Human-generated books contain move
    annotation properties according to the SGF standard. The book will select
    randomly among the child nodes that have the move annotation good move
    or very good move (TE[1] or TE[2]) if there is at least one such move.
    Otherwise it assumes that the book was computer-generated (e.g. by
    libpentobi_mcts::BookBuilder. Computer-generated books have two numbers
    stored as text in the comment property. The first number is the value
    of the move, the second number a visit count that shows how often the
    position was visited during move generation. The book will select a child
    randomly with a probably exponentially decreasing with the distance of the
    value from the value of the best child. */
class Book
{
public:
    Book();

    static double get_value(const Tree& tree, const Node& node);

    static void get_value_and_count(const Tree& tree, const Node& node,
                                    double& value, unsigned int& count);

    /** Select a child in a human-generated book. */
    const Node* select_annotated_child(RandomGenerator& random,
                                       const Board& bd, Color c,
                                       const Tree& tree, const Node& node);

    /** Select a child in a computer-generated book.
        @param delta The width of the exponential probabilty function used
        for move selection from computer-generated books.
        @param max_delta Cutoff for the probabilty function used for move
        selection from computer-generated books. */
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

    bool m_is_computer_generated;
};

inline const Tree& Book::get_tree() const
{
    return m_tree;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOOK_H
