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

    /** @tparam P An instance of class Point. */
    template<class P>
    P get_transformed(const P& p, unsigned int sz) const;

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
P Transform::get_transformed(const P& p, unsigned int sz) const
{
    switch (m_type)
    {
    case identity:
        return p;
    case rotate_90:
        return P(p.get_y(), sz - p.get_x() - 1);
    case rotate_180:
        return P(sz - p.get_x() - 1, sz - p.get_y() - 1);
    case rotate_270:
        return P(sz - p.get_y() - 1, p.get_x());
    case mirror:
        return P(sz - p.get_x() - 1, p.get_y());
    case rotate_90_mirror:
        return P(p.get_y(), p.get_x());
    case rotate_180_mirror:
        return P(p.get_x(), sz - p.get_y() - 1);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return P(sz - p.get_y() - 1, sz - p.get_x() - 1);
    }
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
