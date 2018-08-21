//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceInfo.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PieceInfo.h"

#include <algorithm>
#include "libboardgame_base/GeometryUtil.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_sys/Compiler.h"

namespace libpentobi_base {

using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_base::geometry_util::type_match_shift;
using libboardgame_sys::get_type_name;

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

#ifdef LIBBOARDGAME_DEBUG
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
        LIBBOARDGAME_LOG("Points ", points);
    NormalizedPoints normalized;
    normalized.points = points;
    type_match_shift(geo, normalized.points.begin(),
                     normalized.points.end(), point_type);
    if (log_piece_creation)
        LIBBOARDGAME_LOG("Point type ", point_type, ", type match shift ",
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
                     GeometryType geometry_type, CoordPoint label_pos,
                     unsigned nu_instances)
    : m_nu_instances(nu_instances),
      m_points(points),
      m_label_pos(label_pos),
      m_transforms(&transforms),
      m_name(name)
{
    LIBBOARDGAME_ASSERT(nu_instances > 0);
    LIBBOARDGAME_ASSERT(nu_instances <= PieceInfo::max_instances);
    if (log_piece_creation)
        LIBBOARDGAME_LOG("Creating transformations for piece ", name, ' ',
                         points);
    auto& all_transforms = transforms.get_all();
    vector<NormalizedPoints> all_transformed_points;
    all_transformed_points.reserve(all_transforms.size());
    m_uniq_transforms.reserve(all_transforms.size()); // Upper limit
    PiecePoints transformed_points;
    for (auto transform : all_transforms)
    {
        if (log_piece_creation)
            LIBBOARDGAME_LOG("Transformation ", get_type_name(*transform));
        transformed_points = points;
        transform->transform(transformed_points.begin(),
                             transformed_points.end());
        NormalizedPoints normalized = normalize(transformed_points,
                                                transform->get_point_type(),
                                                geo);
        if (log_piece_creation)
            LIBBOARDGAME_LOG("Normalized ", normalized.points, " point type ",
                             normalized.point_type);
        LIBBOARDGAME_ASSERT(check_consistency(normalized.points));
        auto begin = all_transformed_points.begin();
        auto end = all_transformed_points.end();
        auto pos = find(begin, end, normalized);
        if (pos != end)
        {
            if (log_piece_creation)
                LIBBOARDGAME_LOG("Equivalent to ", pos - begin);
            m_equivalent_transform[transform]
                = transforms.get_all()[pos - begin];
        }
        else
        {
            if (log_piece_creation)
                LIBBOARDGAME_LOG("New (", m_uniq_transforms.size(), ")");
            m_equivalent_transform[transform] = transform;
            m_uniq_transforms.push_back(transform);
        }
        all_transformed_points.push_back(normalized);
    }
    if (geometry_type == GeometryType::nexos)
    {
        m_score_points = 0;
        for (auto& p : points)
        {
            auto point_type = geo.get_point_type(p);
            LIBBOARDGAME_ASSERT(point_type <= 2);
            if (point_type == 1 || point_type == 2) // Line segment
                ++m_score_points;
        }
    }
    else if (geometry_type == GeometryType::gembloq)
        m_score_points = 0.25f * static_cast<ScoreType>(points.size());
    else if (points.size() == 1 && geometry_type == GeometryType::callisto)
        m_score_points = 0;
    else
        m_score_points = static_cast<ScoreType>(points.size());
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
            normalize(piece_points, transform->get_point_type(), geo);
        if (normalized_piece == normalized)
            return transform;
    }
    return nullptr;
}

const Transform* PieceInfo::get_equivalent_transform(
                                               const Transform* transform) const
{
    auto pos = m_equivalent_transform.find(transform);
    LIBBOARDGAME_ASSERT(pos != m_equivalent_transform.end());
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
    return *(pos - 1);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
