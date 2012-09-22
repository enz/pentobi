//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SymmetricPoints.h"

#include "libboardgame_base/PointTransform.h"

namespace libpentobi_base {

using libboardgame_base::PointTransfRot180;

//-----------------------------------------------------------------------------

void SymmetricPoints::init(const Geometry& geometry)
{
    m_symmetric_point.init(geometry);
    PointTransfRot180<Point> transform;
    unsigned width = geometry.get_width();
    unsigned height = geometry.get_height();
    for (Geometry::Iterator i(geometry); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, width, height);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

