//-----------------------------------------------------------------------------
/** @file libpentobi_base/AdjDiagIterator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_ADJ_DIAG_ITERATOR_H
#define LIBPENTOBI_BASE_ADJ_DIAG_ITERATOR_H

#include "Geometry.h"
#include "libboardgame_util/NullTermList.h"

namespace libpentobi_base {

using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

class AdjDiagIterator
    : public NullTermList<Point, 12>::Iterator
{
public:
    AdjDiagIterator(const Geometry& geo, Point p)
        : NullTermList<Point, 12>::Iterator(geo.get_adj_diag(p))
    { }
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_ADJ_DIAG_ITERATOR_H
