//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SymmetricPoints.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

void SymmetricPoints::init(const Geometry& geo,
                           const PointTransform<Point>& transform)
{
    for (Geometry::Iterator i(geo); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, geo);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

