//-----------------------------------------------------------------------------
/** @file Transform.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Transform.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

namespace {

vector<Transform> create_all()
{
    vector<Transform> result;
    result.push_back(Transform::identity);
    result.push_back(Transform::rotate_90);
    result.push_back(Transform::rotate_180);
    result.push_back(Transform::rotate_270);
    result.push_back(Transform::mirror);
    result.push_back(Transform::rotate_90_mirror);
    result.push_back(Transform::rotate_180_mirror);
    result.push_back(Transform::rotate_270_mirror);
    return result;
}

} // namespace

//-----------------------------------------------------------------------------

const vector<Transform>& Transform::get_all()
{
    static vector<Transform> all = create_all();
    return all;
}

Transform Transform::get_mirrored_horizontally() const
{
    switch (m_type)
    {
    case identity:
        return Transform(mirror);
    case rotate_90:
        return Transform(rotate_270_mirror);
    case rotate_180:
        return Transform(rotate_180_mirror);
    case rotate_270:
        return Transform(rotate_90_mirror);
    case mirror:
        return Transform(identity);
    case rotate_90_mirror:
        return Transform(rotate_270);
    case rotate_180_mirror:
        return Transform(rotate_180);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return Transform(rotate_90);
    }
}

Transform Transform::get_mirrored_vertically() const
{
    switch (m_type)
    {
    case identity:
        return Transform(rotate_180_mirror);
    case rotate_90:
        return Transform(rotate_90_mirror);
    case rotate_180:
        return Transform(mirror);
    case rotate_270:
        return Transform(rotate_270_mirror);
    case mirror:
        return Transform(rotate_180);
    case rotate_90_mirror:
        return Transform(rotate_90);
    case rotate_180_mirror:
        return Transform(identity);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return Transform(rotate_270);
    }
}

Transform Transform::get_rotated_anticlockwise() const
{
    switch (m_type)
    {
    case identity:
        return Transform(rotate_270);
    case rotate_90:
        return Transform(identity);
    case rotate_180:
        return Transform(rotate_90);
    case rotate_270:
        return Transform(rotate_180);
    case mirror:
        return Transform(rotate_270_mirror);
    case rotate_90_mirror:
        return Transform(mirror);
    case rotate_180_mirror:
        return Transform(rotate_90_mirror);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return Transform(rotate_180_mirror);
    }
}

Transform Transform::get_rotated_clockwise() const
{
    switch (m_type)
    {
    case identity:
        return Transform(rotate_90);
    case rotate_90:
        return Transform(rotate_180);
    case rotate_180:
        return Transform(rotate_270);
    case rotate_270:
        return Transform(identity);
    case mirror:
        return Transform(rotate_90_mirror);
    case rotate_90_mirror:
        return Transform(rotate_180_mirror);
    case rotate_180_mirror:
        return Transform(rotate_270_mirror);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return Transform(mirror);
    }
}

CoordPoint Transform::get_transformed(const CoordPoint& p) const
{
    switch (m_type)
    {
    case identity:
        return p;
    case rotate_90:
        return CoordPoint(p.y, -p.x);
    case rotate_180:
        return CoordPoint(-p.x, -p.y);
    case rotate_270:
        return CoordPoint(-p.y, p.x);
    case mirror:
        return CoordPoint(-p.x, p.y);
    case rotate_90_mirror:
        return CoordPoint(p.y, p.x);
    case rotate_180_mirror:
        return CoordPoint(p.x, -p.y);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return CoordPoint(-p.y, -p.x);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
