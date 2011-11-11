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

/** Rotation and/or mirroring. */
class Transform
{
public:
    virtual ~Transform();

    virtual CoordPoint get_transformed(const CoordPoint& p) const = 0;

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
};

//-----------------------------------------------------------------------------

class TransfRot90
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRot270
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRefl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRot90Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRot180Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

class TransfRot270Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
