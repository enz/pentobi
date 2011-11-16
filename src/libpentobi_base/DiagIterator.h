//-----------------------------------------------------------------------------
/** @file libpentobi_base/DiagIterator.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_DIAG_ITERATOR_H
#define LIBPENTOBI_BASE_DIAG_ITERATOR_H

#include "Board.h"
#include "libboardgame_util/NullTermList.h"

namespace libpentobi_base {

using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

class DiagIterator
    : public NullTermList<Point, 9>::Iterator
{
public:
    DiagIterator(const Geometry& geometry, Point p);

    DiagIterator(const Board& bd, Point p);
};

inline DiagIterator::DiagIterator(const Geometry& geometry, Point p)
    : NullTermList<Point, 9>::Iterator(geometry.get_diag(p))
{
}

inline DiagIterator::DiagIterator(const Board& bd, Point p)
    : NullTermList<Point, 9>::Iterator(bd.get_geometry().get_diag(p))
{
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_DIAG_ITERATOR_H
