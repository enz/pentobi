//-----------------------------------------------------------------------------
/** @file Piece.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_H
#define LIBPENTOBI_BASE_PIECE_H

#include <string>
#include <vector>
#include "libboardgame_base/CoordPoint.h"
#include "libboardgame_base/Transform.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

class Piece
{
public:
    /** Maximim number of squares of a piece. */
    static const unsigned int max_size = 5;

    typedef ArrayList<CoordPoint, max_size> Points;

    /** Maximim number of adjacent points to a piece. */
    static const unsigned int max_adj = 12;

    /** Maximim number of corner points of a piece. */
    static const unsigned int max_corner = 8;

    /** Constructor.
        @param name A short unique name for the piece.
        @param points The coordinates of the squares. Must contain the point
        (0, 0). (0, 0) should be a square in or near the center of the
        coordinates; it is used as the center when moving the piece or for
        drawing a label on the piece. */
    Piece(const string& name, const Piece::Points& points);

    const string& get_name() const;

    const Points& get_points() const;

    /** Return the number of squares of the piece. */
    unsigned int get_size() const;

    /** Get a list with unique transformations.
        The list has the same order as Tranform::all but transformations that
        are equivalent to a previous transformation (because of a symmetry of
        the piece) are omitted. */
    const ArrayList<Transform, 8>& get_transforms() const;

    /** Get next transform from the list of unique transforms. */
    Transform get_next_transform(Transform transform) const;

    /** Get previous transform from the list of unique transforms. */
    Transform get_previous_transform(Transform transform) const;

    /** Get the transform from the list of unique transforms that is equivalent
        to a given transform. */
    Transform get_equivalent_transform(Transform transform) const;

    bool can_rotate() const;

    bool can_flip_horizontally(Transform transform) const;

    bool can_flip_vertically(Transform transform) const;

    bool find_transform(const Points& points, Transform& transform) const;

private:
    string m_name;

    Points m_points;

    ArrayList<Transform, 8> m_transforms;

    array<Transform, 8> m_equivalent_transform;
};

inline Transform Piece::get_equivalent_transform(Transform transform) const
{
    return m_equivalent_transform[transform.to_int()];
}

inline const string& Piece::get_name() const
{
    return m_name;
}

inline const Piece::Points& Piece::get_points() const
{
    return m_points;
}

inline unsigned int Piece::get_size() const
{
    return static_cast<unsigned int>(m_points.size());
}

inline const ArrayList<Transform, 8>& Piece::get_transforms() const
{
    return m_transforms;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_H
