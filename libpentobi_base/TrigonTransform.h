//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonTransform.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TRIGON_TRANSFORM_H
#define LIBPENTOBI_BASE_TRIGON_TRANSFORM_H

#include "libboardgame_base/Transform.h"

namespace libpentobi_base {

using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;

//-----------------------------------------------------------------------------

class TransfTrigonIdentity
    : public Transform
{
public:
    TransfTrigonIdentity() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot60
    : public Transform
{
public:
    TransfTrigonRot60() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot120
    : public Transform
{
public:
    TransfTrigonRot120() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot180
    : public Transform
{
public:
    TransfTrigonRot180() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot240
    : public Transform
{
public:
    TransfTrigonRot240() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot300
    : public Transform
{
public:
    TransfTrigonRot300() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRefl
    : public Transform
{
public:
    TransfTrigonRefl() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot60
    : public Transform
{
public:
    TransfTrigonReflRot60() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot120
    : public Transform
{
public:
    TransfTrigonReflRot120() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot180
    : public Transform
{
public:
    TransfTrigonReflRot180() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot240
    : public Transform
{
public:
    TransfTrigonReflRot240() : Transform(0) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot300
    : public Transform
{
public:
    TransfTrigonReflRot300() : Transform(1) {}

    CoordPoint get_transformed(CoordPoint p) const override;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TRIGON_TRANSFORM_H
