//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOOK_H
#define LIBPENTOBI_BASE_BOOK_H

#include <iostream>
#include "Board.h"
#include "Game.h"
#include "Tree.h"
#include "libboardgame_base/PointTransform.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libpentobi_base {

using libboardgame_base::PointTransform;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

/** Opening book.
    Opening books are stored as treed in SGF files. This class can handle
    two variants of opening books. Human-generated books contain move
    annotation properties according to the SGF standard. The book will select
    randomly among the child nodes that have the move annotation good move
    or very good move (TE[1] or TE[2]) if there is at least one such move.
    Otherwise it assumes that the book was computer-generated (e.g. by
    libpentobi_mcts::BookBuilder. Computer-generated books have a floating
    point position value stored as a comment property with key "v". The book
    will select a child randomly with a probability exponentially decreasing
    with the distance of the value from the value of the best child. */
class Book
{
public:
    Book(Variant variant);

    /** Select a child in a human-generated book. */
    const Node* select_annotated_child(RandomGenerator& random,
                                       const Board& bd, Color c,
                                       const Tree& tree, const Node& node);

    /** Select a child in a computer-generated book.
        @param random
        @param bd
        @param c
        @param tree
        @param node
        @param delta The width of the exponential probabilty function used
        for move selection from computer-generated books.
        @param max_delta Cutoff for the probabilty function used for move
        selection from computer-generated books. */
    const Node* select_child(RandomGenerator& random, const Board& bd, Color c,
                             const Tree& tree, const Node& node, double delta,
                             double max_delta);

    void load(istream& in);

    Move genmove(const Board& bd, Color c, double delta, double max_delta);

    const Tree& get_tree() const;

    /** Invert the value.
        Returns the negated value. Currently assumes that the inverted value is
        1-value, but this may become configurable in the future. */
    double inv_value(double value) const;

private:
    Tree m_tree;

    RandomGenerator m_random;

    bool m_is_computer_generated;

    Move genmove(const Board& bd, Color c, double delta, double max_delta,
                 const PointTransform<Point>& transform,
                 const PointTransform<Point>& inv_transform);

    Move get_transformed(const Board& bd, Move mv,
                         const PointTransform<Point>& transform) const;
};

inline const Tree& Book::get_tree() const
{
    return m_tree;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOOK_H
