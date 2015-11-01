//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsTrigon.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceTransformsTrigon.h"

#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

PieceTransformsTrigon::PieceTransformsTrigon()
{
    m_all.reserve(12);
    m_all.push_back(&m_identity);
    m_all.push_back(&m_rot60);
    m_all.push_back(&m_rot120);
    m_all.push_back(&m_rot180);
    m_all.push_back(&m_rot240);
    m_all.push_back(&m_rot300);
    m_all.push_back(&m_refl);
    m_all.push_back(&m_refl_rot60);
    m_all.push_back(&m_refl_rot120);
    m_all.push_back(&m_refl_rot180);
    m_all.push_back(&m_refl_rot240);
    m_all.push_back(&m_refl_rot300);
}

const Transform* PieceTransformsTrigon::get_default() const
{
    return &m_identity;
}

const Transform* PieceTransformsTrigon::get_mirrored_horizontally(
                                                 const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_refl;
    if (transf == &m_rot60)
        return &m_refl_rot300;
    if (transf == &m_rot120)
        return &m_refl_rot240;
    if (transf == &m_rot180)
        return &m_refl_rot180;
    if (transf == &m_rot240)
        return &m_refl_rot120;
    if (transf == &m_rot300)
        return &m_refl_rot60;
    if (transf == &m_refl)
        return &m_identity;
    if (transf == &m_refl_rot60)
        return &m_rot300;
    if (transf == &m_refl_rot120)
        return &m_rot240;
    if (transf == &m_refl_rot180)
        return &m_rot180;
    if (transf == &m_refl_rot240)
        return &m_rot120;
    if (transf == &m_refl_rot300)
        return &m_rot60;
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

const Transform* PieceTransformsTrigon::get_mirrored_vertically(
                                                 const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_refl_rot180;
    if (transf == &m_rot60)
        return &m_refl_rot120;
    if (transf == &m_rot120)
        return &m_refl_rot60;
    if (transf == &m_rot180)
        return &m_refl;
    if (transf == &m_rot240)
        return &m_refl_rot300;
    if (transf == &m_rot300)
        return &m_refl_rot240;
    if (transf == &m_refl)
        return &m_rot180;
    if (transf == &m_refl_rot60)
        return &m_rot120;
    if (transf == &m_refl_rot120)
        return &m_rot60;
    if (transf == &m_refl_rot180)
        return &m_identity;
    if (transf == &m_refl_rot240)
        return &m_rot300;
    if (transf == &m_refl_rot300)
        return &m_rot240;
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

const Transform* PieceTransformsTrigon::get_rotated_anticlockwise(
                                                 const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_rot300;
    if (transf == &m_rot60)
        return &m_identity;
    if (transf == &m_rot120)
        return &m_rot60;
    if (transf == &m_rot180)
        return &m_rot120;
    if (transf == &m_rot240)
        return &m_rot180;
    if (transf == &m_rot300)
        return &m_rot240;
    if (transf == &m_refl)
        return &m_refl_rot300;
    if (transf == &m_refl_rot60)
        return &m_refl;
    if (transf == &m_refl_rot120)
        return &m_refl_rot60;
    if (transf == &m_refl_rot180)
        return &m_refl_rot120;
    if (transf == &m_refl_rot240)
        return &m_refl_rot180;
    if (transf == &m_refl_rot300)
        return &m_refl_rot240;
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

const Transform* PieceTransformsTrigon::get_rotated_clockwise(
                                                 const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_rot60;
    if (transf == &m_rot60)
        return &m_rot120;
    if (transf == &m_rot120)
        return &m_rot180;
    if (transf == &m_rot180)
        return &m_rot240;
    if (transf == &m_rot240)
        return &m_rot300;
    if (transf == &m_rot300)
        return &m_identity;
    if (transf == &m_refl)
        return &m_refl_rot60;
    if (transf == &m_refl_rot60)
        return &m_refl_rot120;
    if (transf == &m_refl_rot120)
        return &m_refl_rot180;
    if (transf == &m_refl_rot180)
        return &m_refl_rot240;
    if (transf == &m_refl_rot240)
        return &m_refl_rot300;
    if (transf == &m_refl_rot300)
        return &m_refl;
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
