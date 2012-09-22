//-----------------------------------------------------------------------------
/** @file Transform.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRANSFORM_H
#define LIBBOARDGAME_BASE_TRANSFORM_H

#include <vector>
#include "CoordPoint.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Rotation and/or reflection of local coordinates on the board.
    Used for local patterns. */
class Transform
{
public:
    virtual ~Transform();

    virtual CoordPoint get_transformed(const CoordPoint& p) const = 0;

    /** Get the point type of the (0,0) coordinates.
        The transformation can only applied to this point type at (0,0). */
    virtual unsigned get_point_type() const = 0;

    /** Get the new point type of the (0,0) coordinates.
        The transformation may change the point type of the (0,0) coordinates.
        For example, in the Blokus Trigon board, a reflection at the y axis
        changes the type from 0 (=downside triangle) to 1 (=upside triangle).
        @see Geometry::get_point_type() */
    virtual unsigned get_new_point_type() const = 0;

    /** @tparam I An iterator of a container with elements of type CoordPoint */
    template<class I>
    void transform(I begin, I end) const;
};

template<class I>
void Transform::transform(I begin, I end) const
{
    for (I i = begin; i != end; ++i)
        *i = get_transformed(*i);
}

//-----------------------------------------------------------------------------

class TransfIdentity
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot90
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot270
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRefl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot90Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot180Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfRectRot270Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot60
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot120
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot240
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot300
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonRefl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot60
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot120
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot240
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot300
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;

    unsigned get_point_type() const;

    unsigned get_new_point_type() const;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
