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
    /** Maximum number of fields of a piece. */
    static const unsigned max_size = 6;

    typedef ArrayList<CoordPoint, max_size> Points;

    /** Maximum number of adjacent points to a piece. */
    static const unsigned max_adj = 12;

    /** Maximum number of attach points of a piece. */
    static const unsigned max_attach = 14;

    /** Maximum number of attach and adjacent points of a piece. */
    static const unsigned max_adj_attach = 22;

    /** Constructor.
        @param name A short unique name for the piece.
        @param points The coordinates of the fields. Must contain the point
        (0, 0). (0, 0) should be a field in or near the center of the
        coordinates; it is used as the center when moving the piece or for
        drawing a label on the piece.
        @param geo
        @param transforms */
    PieceInfo(const string& name, const Points& points,
              const Geometry& geo, const PieceTransforms& transforms);

    const string& get_name() const;

    const Points& get_points() const;

    /** Return the number of fields of the piece. */
    unsigned get_size() const;

    /** Get a list with unique transformations.
        The list has the same order as PieceTransforms::get_all() but
        transformations that are equivalent to a previous transformation
        (because of a symmetry of the piece) are omitted. */
    const vector<const Transform*>& get_transforms() const;

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

    vector<const Transform*> m_uniq_transforms;

    map<const Transform*,const Transform*> m_equivalent_transform;

    const PieceTransforms* m_transforms;
};

inline const string& PieceInfo::get_name() const
{
    return m_name;
}

inline const PieceInfo::Points& PieceInfo::get_points() const
{
    return m_points;
}

inline unsigned PieceInfo::get_size() const
{
    return static_cast<unsigned>(m_points.size());
}

inline const vector<const Transform*>& PieceInfo::get_transforms() const
{
    return m_uniq_transforms;
}

//-----------------------------------------------------------------------------

typedef PieceInfo::Points PiecePoints;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_INFO_H
