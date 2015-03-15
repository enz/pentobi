//-----------------------------------------------------------------------------
/** @file libboardgame_base/RectTransform.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_RECTTRANSFORM_H
#define LIBBOARDGAME_BASE_RECTTRANSFORM_H

#include "Transform.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

class TransfIdentity
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot90
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot270
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRefl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot90Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot180Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot270Refl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
