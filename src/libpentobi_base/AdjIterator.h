//-----------------------------------------------------------------------------
/** @file libpentobi_base/AdjIterator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_ADJ_ITERATOR_H
#define LIBPENTOBI_BASE_ADJ_ITERATOR_H

#include "Board.h"
#include "libboardgame_util/NullTermList.h"

namespace libpentobi_base {

using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

class AdjIterator
    : public NullTermList<Point, 4>::Iterator
{
public:
    AdjIterator(const Geometry& geo, Point p);

    AdjIterator(const Board& bd, Point p);
};

inline AdjIterator::AdjIterator(const Geometry& geo, Point p)
    : NullTermList<Point, 4>::Iterator(geo.get_adj(p))
{
}

inline AdjIterator::AdjIterator(const Board& bd, Point p)
    : NullTermList<Point, 4>::Iterator(bd.get_geometry().get_adj(p))
{
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_ADJ_ITERATOR_H
