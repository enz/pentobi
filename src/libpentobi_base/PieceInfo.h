//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceInfo.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_INFO_H
#define LIBPENTOBI_BASE_PIECE_INFO_H

#include <map>
#include <string>
#include <vector>
#include "Geometry.h"
#include "PieceTransforms.h"
#include "Variant.h"
#include "libboardgame_base/CoordPoint.h"
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

using ScoreType = float;

//-----------------------------------------------------------------------------

class PieceInfo
{
public:
    /** Maximum number of points in a piece. */
    static const unsigned max_size = 22;

    /** Maximum number of scored points in a piece.
        Currently the same as max_size, needed for GembloQ. If Nexos was the
        game with the largest pieces, some memory could be saved because
        junction points in Nexos are not scored. */
    static const unsigned max_scored_size = 22;

    /** Maximum number of instances of a piece per player. */
    static const unsigned max_instances = 3;

    using Points = ArrayList<CoordPoint, max_size>;


    /** Constructor.
        @param name A short unique name for the piece.
        @param points The coordinates of the piece elements.
        @param geo
        @param transforms
        @param geometry_type
        @param label_pos The coordinates for drawing a label on the piece.
        @param nu_instances The number of instances of the piece per player. */
    PieceInfo(const string& name, const Points& points,
              const Geometry& geo, const PieceTransforms& transforms,
              GeometryType geometry_type, CoordPoint label_pos,
              unsigned nu_instances = 1);

    const string& get_name() const { return m_name; }

    /** The points of the piece.
        In Nexos, the points of a piece contain the coordinates of line
        segments and of junctions that are essentially needed to mark the
        intersection as non-crossable (i.e. junctions that touch exactly two
        line segments of the piece with identical orientation. */
    const Points& get_points() const { return m_points; }

    const CoordPoint& get_label_pos() const { return m_label_pos; }

    /** Return the number of points of the piece that contribute to the score.
        This excludes any junction points included in the piece definition in
        Nexos.*/
    ScoreType get_score_points() const { return m_score_points; }

    unsigned get_nu_instances() const { return m_nu_instances; }

    /** Get a list with unique transformations.
        The list has the same order as PieceTransforms::get_all() but
        transformations that are equivalent to a previous transformation
        (because of a symmetry of the piece) are omitted. */
    const vector<const Transform*>& get_transforms() const
    {
        return m_uniq_transforms;
    }

    /** Get next transform from the list of unique transforms. */
    const Transform* get_next_transform(const Transform* transform) const;

    /** Get previous transform from the list of unique transforms. */
    const Transform* get_previous_transform(const Transform* transform) const;

    /** Get the transform from the list of unique transforms that is equivalent
        to a given transform. */
    const Transform* get_equivalent_transform(const Transform* transform) const;

    const Transform* find_transform(const Geometry& geo,
                                    const Points& points) const;

private:
    unsigned m_nu_instances;

    Points m_points;

    CoordPoint m_label_pos;

    ScoreType m_score_points;

    string m_name;

    vector<const Transform*> m_uniq_transforms;

    map<const Transform*,const Transform*> m_equivalent_transform;
};

//-----------------------------------------------------------------------------

using PiecePoints = PieceInfo::Points;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_INFO_H
