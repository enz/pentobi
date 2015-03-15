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
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot60
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot120
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot240
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRot300
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonRefl
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot60
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot120
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot180
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot240
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

class TransfTrigonReflRot300
    : public Transform
{
public:
    CoordPoint get_transformed(const CoordPoint& p) const override;

    unsigned get_new_point_type() const override;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TRIGON_TRANSFORM_H
