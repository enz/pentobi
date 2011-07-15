//-----------------------------------------------------------------------------
/** @file SymmetricPoints.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SymmetricPoints.h"

#include "libboardgame_base/Transform.h"

namespace libpentobi_base {

using libboardgame_base::Geometry;
using libboardgame_base::Transform;

//-----------------------------------------------------------------------------

void SymmetricPoints::init(unsigned int sz)
{
    m_symmetric_point.init(sz);
    Transform transform(Transform::rotate_180);
    for (Geometry<Point>::Iterator i(sz); i; ++i)
        m_symmetric_point[*i] = transform.get_transformed(*i, sz);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

