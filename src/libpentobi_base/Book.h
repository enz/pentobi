//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOOK_H
#define LIBPENTOBI_BASE_BOOK_H

#include <iosfwd>
#include "Board.h"
#include "Game.h"
#include "Tree.h"
#include "libboardgame_base/PointTransform.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libpentobi_base {

using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

/** Opening book.
    Opening books are stored as trees in SGF files. Thay contain move
    annotation properties according to the SGF standard. The book will select
    randomly among the child nodes that have the move annotation good move
    or very good move (TE[1] or TE[2]). */
class Book
{
public:
    Book(Variant variant);

    void load(istream& in);

    Move genmove(const Board& bd, Color c);

    const Tree& get_tree() const;

private:
    typedef libboardgame_base::PointTransform<Point> PointTransform;

    Tree m_tree;

    RandomGenerator m_random;

    bool genmove(const Board& bd, Color c, Move& mv,
                 const PointTransform& transform,
                 const PointTransform& inv_transform);

    Move get_transformed(const Board& bd, Move mv,
                         const PointTransform& transform) const;

    const Node* select_child(const Board& bd, Color c, const Tree& tree,
                             const Node& node,
                             const PointTransform& inv_transform);
};

inline const Tree& Book::get_tree() const
{
    return m_tree;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOOK_H
