//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceInfo.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceInfo.h"

#include <algorithm>
#include "libboardgame_base/GeometryUtil.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_base::geometry_util::type_match_shift;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

const bool log_piece_creation = false;

struct NormalizedPoints
{
    /** The normalized points of the transformed piece.
        The points were shifted using GeometryUtil::normalize_offset(). */
    PiecePoints points;

    /** The point type of (0,0) in the normalized points. */
    unsigned point_type;

    bool operator==(const NormalizedPoints& n) const
    {
        return points == n.points && point_type == n.point_type;
    }
};

#if LIBBOARDGAME_DEBUG
/** Check consistency of transformations.
    Checks that the point list (which must be already sorted) has no
    duplicates. */
bool check_consistency(const PiecePoints& points)
{
    for (unsigned i = 0; i < points.size(); ++i)
        if (i > 0 && points[i] == points[i - 1])
            return false;
    return true;
}
#endif // LIBBOARDGAME_DEBUG

/** Bring piece points into a normal form that is constant under translation. */
NormalizedPoints normalize(const PiecePoints& points, unsigned point_type,
                           const Geometry& geo)
{
    if (log_piece_creation)
        log("Points ", points);
    NormalizedPoints normalized;
    normalized.points = points;
    type_match_shift(geo, normalized.points.begin(),
                     normalized.points.end(), point_type);
    if (log_piece_creation)
        log("Point type ", point_type, ", type match shift ",
            normalized.points);
    // Make the coordinates positive and minimal
    unsigned width; // unused
    unsigned height; // unused
    CoordPoint offset;
    normalize_offset(normalized.points.begin(), normalized.points.end(),
                     width, height, offset);
    normalized.point_type = geo.get_point_type(offset);
    // Sort the coordinates
    sort(normalized.points.begin(), normalized.points.end());
    return normalized;
}

} // namespace

//-----------------------------------------------------------------------------

PieceInfo::PieceInfo(const string& name, const PiecePoints& points,
                     const Geometry& geo, const PieceTransforms& transforms,
                     const CoordPoint& label_pos)
    : m_name(name),
      m_points(points),
      m_label_pos(label_pos),
      m_transforms(&transforms)
{
    if (log_piece_creation)
        log("Creating transformations for piece ", name, ' ', points);
    LIBBOARDGAME_ASSERT(points.contains(CoordPoint(0, 0)));
    vector<NormalizedPoints> all_transformed_points;
    PiecePoints transformed_points;
    for (const Transform* transform : transforms.get_all())
    {
        if (log_piece_creation)
            log("Transformation ", typeid(*transform).name());
        transformed_points = points;
        transform->transform(transformed_points.begin(),
                             transformed_points.end());
        NormalizedPoints normalized = normalize(transformed_points,
                                                transform->get_new_point_type(),
                                                geo);
        if (log_piece_creation)
            log("Normalized ", normalized.points, " point type ",
                normalized.point_type);
        LIBBOARDGAME_ASSERT(check_consistency(normalized.points));
        auto begin = all_transformed_points.begin();
        auto end = all_transformed_points.end();
        auto pos = find(begin, end, normalized);
        if (pos != end)
        {
            if (log_piece_creation)
                log("Equivalent to ", pos - begin);
            m_equivalent_transform[transform]
                = transforms.get_all()[pos - begin];
        }
        else
        {
            if (log_piece_creation)
                log("New (", m_uniq_transforms.size(), ")");
            m_equivalent_transform[transform] = transform;
            m_uniq_transforms.push_back(transform);
        }
        all_transformed_points.push_back(normalized);
    };
}

bool PieceInfo::can_flip_horizontally(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    auto flip = get_equivalent_transform(
                            m_transforms->get_mirrored_horizontally(transform));
    return flip != transform;
}

bool PieceInfo::can_flip_vertically(const Transform* transform) const
{
    transform = get_equivalent_transform(transform);
    auto flip = get_equivalent_transform(
                              m_transforms->get_mirrored_vertically(transform));
    return flip != transform;
}

bool PieceInfo::can_rotate() const
{
    auto transform = m_uniq_transforms[0];
    auto rotate = get_equivalent_transform(
                                m_transforms->get_rotated_clockwise(transform));
    return rotate != transform;
}

const Transform* PieceInfo::find_transform(const Geometry& geo,
                                           const Points& points) const
{
    NormalizedPoints normalized =
        normalize(points, geo.get_point_type(0, 0), geo);
    for (const Transform* transform : get_transforms())
    {
        Points piece_points = get_points();
        transform->transform(piece_points.begin(), piece_points.end());
        NormalizedPoints normalized_piece =
            normalize(piece_points, transform->get_new_point_type(), geo);
        if (normalized_piece == normalized)
            return transform;
    }
    return nullptr;
}

const Transform* PieceInfo::get_equivalent_transform(
                                               const Transform* transform) const
{
    auto pos = m_equivalent_transform.find(transform);
    if (pos == m_equivalent_transform.end())
    {
        LIBBOARDGAME_ASSERT(false);
        return nullptr;
    }
    return pos->second;
}

const Transform* PieceInfo::get_next_transform(const Transform* transform) const
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

const Transform* PieceInfo::get_previous_transform(
                                              const Transform* transform) const
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
