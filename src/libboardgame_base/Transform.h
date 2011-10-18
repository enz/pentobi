//-----------------------------------------------------------------------------
/** @file Transform.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRANSFORM_H
#define LIBBOARDGAME_BASE_TRANSFORM_H

#include <vector>
#include "Point.h"
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

    static const vector<Transform>& get_all();

    Transform(Type type = Transform::identity);

    bool operator==(Transform transform) const;

    bool operator!=(Transform transform) const;

    bool is_identity() const;

    CoordPoint get_transformed(const CoordPoint& p) const;

    /** Transform a point.
        @tparam P An instance of class Point.
        @return The transformed point or Point::null() if the transformed point
        would be outside the current geometry given by height and width. */
    template<class P>
    P get_transformed(const P& p, unsigned int width,
                      unsigned int height) const;

    /** @tparam I An iterator of a container with elements of type CoordPoint */
    template<class I>
    void transform(I begin, I end) const;

    Transform get_rotated_anticlockwise() const;

    Transform get_rotated_clockwise() const;

    Transform get_mirrored_vertically() const;

    Transform get_mirrored_horizontally() const;

    unsigned int to_int() const;

private:
    Type m_type;
};

inline Transform::Transform(Type type)
    : m_type(type)
{
}

inline bool Transform::operator==(Transform transform) const
{
    return m_type == transform.m_type;
}

inline bool Transform::operator!=(Transform transform) const
{
    return ! operator==(transform);
}

template<class P>
P Transform::get_transformed(const P& p, unsigned int width,
                             unsigned int height) const
{
    unsigned int x;
    unsigned int y;
    switch (m_type)
    {
    case identity:
        x = p.get_x();
        y = p.get_y();
        break;
    case rotate_90:
        x = p.get_y();
        y = width - p.get_x() - 1;
        break;
    case rotate_180:
        x = width - p.get_x() - 1;
        y = height - p.get_y() - 1;
        break;
    case rotate_270:
        x = height - p.get_y() - 1;
        y = p.get_x();
        break;
    case mirror:
        x = width - p.get_x() - 1;
        y = p.get_y();
        break;
    case rotate_90_mirror:
        x = p.get_y();
        y = p.get_x();
        break;
    case rotate_180_mirror:
        x = p.get_x();
        y = height - p.get_y() - 1;
        break;
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        x = height - p.get_y() - 1;
        y = width - p.get_x() - 1;
        break;
    }
    if (x < width && y < height)
        return P(x, y);
    else
        return P::null();
}

inline bool Transform::is_identity() const
{
    return m_type == identity;
}

inline unsigned int Transform::to_int() const
{
    return static_cast<unsigned int>(m_type);
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
