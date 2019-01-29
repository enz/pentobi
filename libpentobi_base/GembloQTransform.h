//-----------------------------------------------------------------------------
/** @file libpentobi_base/GembloQTransform.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GEMBLOQ_TRANSFORM_H
#define LIBPENTOBI_BASE_GEMBLOQ_TRANSFORM_H

#include "libboardgame_base/Transform.h"

namespace libpentobi_base {

using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;

//-----------------------------------------------------------------------------

class TransfGembloQIdentity
    : public Transform
{
public:
    TransfGembloQIdentity() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot90
    : public Transform
{
public:
    TransfGembloQRot90() : Transform(3) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot180
    : public Transform
{
public:
    TransfGembloQRot180() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot270
    : public Transform
{
public:
    TransfGembloQRot270() : Transform(2) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRefl
    : public Transform
{
public:
    TransfGembloQRefl() : Transform(3) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot90Refl
    : public Transform
{
public:
    TransfGembloQRot90Refl() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot180Refl
    : public Transform
{
public:
    TransfGembloQRot180Refl() : Transform(2) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfGembloQRot270Refl
    : public Transform
{
public:
    TransfGembloQRot270Refl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GEMBLOQ_TRANSFORM_H
