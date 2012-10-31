//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SymmetricPoints.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

void SymmetricPoints::init(const Geometry& geometry,
                           const PointTransform<Point>& transform)
{
    m_symmetric_point.init(geometry);
    unsigned width = geometry.get_width();
    unsigned height = geometry.get_height();
    for (Geometry::Iterator i(geometry); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, width, height);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

