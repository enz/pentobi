//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SymmetricPoints.h"

#include "libboardgame_base/Transform.h"

namespace libpentobi_base {

using libboardgame_base::Transform;

//-----------------------------------------------------------------------------

void SymmetricPoints::init(const Geometry& geometry)
{
    m_symmetric_point.init(geometry);
    Transform transform(Transform::rotate_180);
    unsigned int sz = geometry.get_size();
    for (Geometry::Iterator i(geometry); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, sz);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

