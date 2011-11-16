//-----------------------------------------------------------------------------
/** @file libpentobi_base/Piece.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Piece.h"

#include <algorithm>
#include <boost/foreach.hpp>
#include "libboardgame_base/GeometryUtil.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

const bool log_piece_creation = false;

/** Check consistency of transformations.
    Checks that the point list (which must be already sorted) has no
    duplicates. */
bool check_consistency(const Piece::Points& points)
{
    for (unsigned int i = 0; i < points.size(); ++i)
        if (i > 0 && points[i] == points[i - 1])
            return false;
    return true;
}

/** Bring piece points into a normal form that is constant under translation. */
void normalize(Piece::Points& points, unsigned int point_type,
               const Geometry& geometry)
{
    if (log_piece_creation)
        log() << "Points " << points << '\n';
    // Given the point type of the piece coordinate (0,0), shift the
    // piece coordinates such that the point types are compatible with the
    // coordinate system of the board
    unsigned int dx = 0; // Init to avoid compiler warning
    unsigned int dy = 0; // Init to avoid compiler warning
    bool found = false;
    for (unsigned int y = 0; ! found && y < geometry.get_period_y(); ++y)
        for (unsigned int x = 0; ! found && x < geometry.get_period_x(); ++x)
            if (geometry.get_point_type(x, y) == point_type)
            {
                dx = x;
                dy = y;
                found = true;
            }
    LIBBOARDGAME_ASSERT(found);
    if (log_piece_creation)
        log() << "Point type " << point_type << " shift " << dx << ' ' << dy
              << '\n';
    BOOST_FOREACH(CoordPoint& p, points)
    {
        p.x += dx;
        p.y += dy;
    }
    // Make the coordinates positive and minimal without changing the point
    // types
    normalize_offset(geometry, points.begin(), points.end());
    // Sort the coordinates
    sort(points.begin(), points.end());
}

} // namespace

//-----------------------------------------------------------------------------

Piece::Piece(const string& name, const Piece::Points& points,
             const Geometry& geometry, const PieceTransforms& transforms)
    : m_name(name),
      m_points(points),
      m_transforms(&transforms)
{
    if (log_piece_creation)
        log() << "Creating transformations for piece " << name
              << ' ' << points << '\n';
    LIBBOARDGAME_ASSERT(points.size() <= max_size);
    LIBBOARDGAME_ASSERT(find(points.begin(), points.end(), CoordPoint(0, 0))
                        != points.end());
    vector<Piece::Points> all_transformed_points;
    Piece::Points transformed_points;
    BOOST_FOREACH(const Transform* transform, transforms.get_all())
    {
        if (log_piece_creation)
            log() << "Transformation " << typeid(*transform).name() << '\n';
        transformed_points = points;
        transform->transform(transformed_points.begin(),
                             transformed_points.end());
        normalize(transformed_points, transform->get_new_point_type(),
                  geometry);
        if (log_piece_creation)
            log() << transformed_points << '\n';
        LIBBOARDGAME_ASSERT(check_consistency(transformed_points));
        auto begin = all_transformed_points.begin();
        auto end = all_transformed_points.end();
        auto pos = find(begin, end, transformed_points);
        if (pos != end)
        {
            if (log_piece_creation)
                log() << "Equivalent to " << (pos - begin) << '\n';
            m_equivalent_transform[transform]
                = transforms.get_all()[pos - begin];
        }
        else
        {
            if (log_piece_creation)
                log() << "New (" << m_uniq_transforms.size() << ")\n";
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
        normalize(normalized_piece_points, transform->get_new_point_type(),
                  geometry);
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
