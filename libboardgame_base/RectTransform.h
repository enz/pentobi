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
    TransfIdentity() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot90
    : public Transform
{
public:
    TransfRectRot90() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot180
    : public Transform
{
public:
    TransfRectRot180() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot270
    : public Transform
{
public:
    TransfRectRot270() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRefl
    : public Transform
{
public:
    TransfRectRefl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot90Refl
    : public Transform
{
public:
    TransfRectRot90Refl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot180Refl
    : public Transform
{
public:
    TransfRectRot180Refl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfRectRot270Refl
    : public Transform
{
public:
    TransfRectRot270Refl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRANSFORM_H
