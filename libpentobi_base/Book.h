//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOOK_H
#define LIBPENTOBI_BASE_BOOK_H

#include <iosfwd>
#include "Board.h"
#include "PentobiTree.h"
#include "libboardgame_base/PointTransform.h"
#include "libboardgame_base/RandomGenerator.h"

namespace libpentobi_base {

using libboardgame_base::RandomGenerator;

//-----------------------------------------------------------------------------

/** Opening book.
    Opening books are stored as trees in SGF files. They contain move
    annotation properties according to the SGF standard. The book will select
    randomly among the child nodes that have the move annotation good move
    or very good move (TE[1] or TE[2]). */
class Book
{
public:
    explicit Book(Variant variant);

    ~Book();

    void load(istream& in);

    Move genmove(const Board& bd, Color c);

    const PentobiTree& get_tree() const;

private:
    using PointTransform = libboardgame_base::PointTransform<Point>;


    PentobiTree m_tree;

    RandomGenerator m_random;

    vector<unique_ptr<PointTransform>> m_transforms;

    vector<unique_ptr<PointTransform>> m_inv_transforms;

    bool genmove(const Board& bd, Color c, Move& mv,
                 const PointTransform& transform,
                 const PointTransform& inv_transform);

    const SgfNode* select_child(const Board& bd, Color c,
                                const PentobiTree& tree, const SgfNode& node,
                                const PointTransform& inv_transform);
};

inline const PentobiTree& Book::get_tree() const
{
    return m_tree;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOOK_H
