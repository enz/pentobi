//-----------------------------------------------------------------------------
/** @file Transform.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRANSFORM_H
#define LIBBOARDGAME_BASE_TRANSFORM_H

#include <vector>
#include "CoordPoint.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Rotation and/or mirroring. */
class Transform
{
public:
    enum Type
    {
        identity,
        rotate_90,
        rotate_180,
        rotate_270,
        mirror,
        rotate_90_mirror,
        rotate_180_mirror,
        rotate_270_mirror
    };

    Transform(Type type);

    CoordPoint get_transformed(const CoordPoint& p) const;

    /** @tparam I An iterator of a container with elements of type CoordPoint */
    template<class I>
    void transform(I begin, I end) const;

private:
    Type m_type;
};

inline Transform::Transform(Type type)
    : m_type(type)
{
}

template<class I>
void Transform::transform(I begin, I end) const
{
    for (I i = begin; i != end; ++i)
        *i = get_transformed(*i);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
