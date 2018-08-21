//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "SymmetricPoints.h"

#include "libboardgame_base/PointTransform.h"

namespace libpentobi_base {

using libboardgame_base::PointTransfRot180;

//-----------------------------------------------------------------------------

void SymmetricPoints::init(const Geometry& geo)
{
    PointTransfRot180<Point> transform;
    for (Point p : geo)
        m_symmetric_point[p] = transform.get_transformed(p, geo);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

