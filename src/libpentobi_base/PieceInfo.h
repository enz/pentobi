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

class PieceInfo
{
public:
    /** Maximum number of points in a piece.
        The maximum piece size occurs with the I4 piece in Nexos (4 real points
        and 3 junction points, see get_points()). */
    static const unsigned max_size = 7;

    /** Maximum number of scored points in a piece.
        This excludes junction points in Nexos. The maximum number of scored
        points occurs in Trigon. */
    static const unsigned max_scored_size = 6;

    typedef ArrayList<CoordPoint, max_size> Points;

    /** Maximum number of attach points and adjacent points of a piece.
        The maximum number occurs with the I6 piece in Trigon (8 adjacent
        points and 14 attach points). */
    static const unsigned max_adj_attach = 22;


    /** Constructor.
        @param name A short unique name for the piece.
        @param points The coordinates of the piece elements.
        @param geo
        @param transforms
        @param board_type
        @param label_pos The coordinates for drawing a label on the piece. */
    PieceInfo(const string& name, const Points& points,
              const Geometry& geo, const PieceTransforms& transforms,
              BoardType board_type, const CoordPoint& label_pos);

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
    unsigned get_score_points() const { return m_score_points; }

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

    bool can_rotate() const;

    bool can_flip_horizontally(const Transform* transform) const;

    bool can_flip_vertically(const Transform* transform) const;

    const Transform* find_transform(const Geometry& geo,
                                    const Points& points) const;

private:
    string m_name;

    Points m_points;

    unsigned m_score_points;

    CoordPoint m_label_pos;

    vector<const Transform*> m_uniq_transforms;

    map<const Transform*,const Transform*> m_equivalent_transform;

    const PieceTransforms* m_transforms;
};

//-----------------------------------------------------------------------------

typedef PieceInfo::Points PiecePoints;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_INFO_H
