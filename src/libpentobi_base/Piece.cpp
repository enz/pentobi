//-----------------------------------------------------------------------------
/** @file libpentobi_base/Piece.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Piece.h"

#include <algorithm>
#include <boost/foreach.hpp>
#include "libboardgame_util/Assert.h"
#include "libboardgame_base/GeometryUtil.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::geometry_util::normalize_offset;

//-----------------------------------------------------------------------------

Piece::Piece(const string& name, const Piece::Points& points,
             const Geometry& geometry, const PieceTransforms& transforms)
    : m_name(name),
      m_points(points),
      m_transforms(&transforms)
{
    LIBBOARDGAME_ASSERT(points.size() <= max_size);
    LIBBOARDGAME_ASSERT(find(points.begin(), points.end(), CoordPoint(0, 0))
                    != points.end());
    vector<Piece::Points> all_transformed_points;
    Piece::Points transformed_points;
    BOOST_FOREACH(const Transform* transform, transforms.get_all())
    {
        transformed_points = points;
        transform->transform(transformed_points.begin(),
                             transformed_points.end());
        normalize_offset(geometry, transformed_points.begin(),
                         transformed_points.end());
        sort(transformed_points.begin(), transformed_points.end());
        auto begin = all_transformed_points.begin();
        auto end = all_transformed_points.end();
        auto pos = find(begin, end, transformed_points);
        if (pos != end)
            m_equivalent_transform[transform]
                = transforms.get_all()[pos - begin];
        else
        {
            m_equivalent_transform[transform] = transform;
            m_uniq_transforms.push_back(transform);
        }
        all_transformed_points.push_back(transformed_points);
    };
}

bool Piece::can_flip_horizontally(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    const Transform* flip = get_equivalent_transform(
                            m_transforms->get_mirrored_horizontally(transform));
    return flip != transform;
}

bool Piece::can_flip_vertically(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    const Transform* flip = get_equivalent_transform(
                              m_transforms->get_mirrored_vertically(transform));
    return flip != transform;
}

bool Piece::can_rotate() const
{
    const Transform* transform = m_uniq_transforms[0];
    const Transform* rotate = get_equivalent_transform(
                                m_transforms->get_rotated_clockwise(transform));
    return rotate != transform;
}

const Transform* Piece::find_transform(const Geometry& geometry,
                                       const Points& points) const
{
    Points normalized_points = points;
    normalize_offset(geometry, normalized_points.begin(),
                     normalized_points.end());
    sort(normalized_points.begin(), normalized_points.end());
    BOOST_FOREACH(const Transform* transform, get_transforms())
    {
        Points normalized_piece_points = get_points();
        transform->transform(normalized_piece_points.begin(),
                             normalized_piece_points.end());
        normalize_offset(geometry, normalized_piece_points.begin(),
                         normalized_piece_points.end());
        sort(normalized_piece_points.begin(), normalized_piece_points.end());
        if (normalized_piece_points == normalized_points)
            return transform;
    }
    return 0;
}

const Transform* Piece::get_equivalent_transform(
                                               const Transform* transform) const
{
    auto pos = m_equivalent_transform.find(transform);
    if (pos == m_equivalent_transform.end())
    {
        LIBBOARDGAME_ASSERT(false);
        return 0;
    }
    return pos->second;
}

const Transform* Piece::get_next_transform(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    auto begin = m_uniq_transforms.begin();
    auto end = m_uniq_transforms.end();
    auto pos = find(begin, end, transform);
    LIBBOARDGAME_ASSERT(pos != end);
    if (pos + 1 == end)
        return *begin;
    else
        return *(pos + 1);
}

const Transform* Piece::get_previous_transform(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    auto begin = m_uniq_transforms.begin();
    auto end = m_uniq_transforms.end();
    auto pos = find(begin, end, transform);
    LIBBOARDGAME_ASSERT(pos != end);
    if (pos == begin)
        return *(end - 1);
    else
        return *(pos - 1);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
