//-----------------------------------------------------------------------------
/** @file libboardgame_base/Transform.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRANSFORM_H
#define LIBBOARDGAME_BASE_TRANSFORM_H

#include "CoordPoint.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** Rotation and/or reflection of local coordinates on the board. */
class Transform
{
public:
    virtual ~Transform();

    virtual CoordPoint get_transformed(CoordPoint p) const = 0;

    /** Get the new point type of the (0,0) coordinates.
        The transformation may change the point type of the (0,0) coordinates.
        For example, in the Blokus Trigon board, a reflection at the y axis
        changes the type from 0 (=downside triangle) to 1 (=upside triangle).
        @see Geometry::get_point_type() */
    unsigned get_point_type() const { return m_point_type; }

    /** @tparam I An iterator of a container with elements of type CoordPoint */
    template<class I>
    void transform(I begin, I end) const;

protected:
    explicit Transform(unsigned point_type)
        : m_point_type(point_type)
    {}

private:
    unsigned m_point_type;
};

template<class I>
void Transform::transform(I begin, I end) const
{
    for (I i = begin; i != end; ++i)
        *i = get_transformed(*i);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
