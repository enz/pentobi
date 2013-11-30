//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
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
    unsigned width = geo.get_width();
    unsigned height = geo.get_height();
    for (Geometry::Iterator i(geo); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, width, height);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

