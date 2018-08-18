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
    m_all.push_back(&m_refl_rot60);
    m_all.push_back(&m_refl_rot120);
    m_all.push_back(&m_refl_rot180);
    m_all.push_back(&m_refl_rot240);
    m_all.push_back(&m_refl_rot300);
    m_all.push_back(&m_refl);
}

const Transform* PieceTransformsTrigon::get_default() const
{
    return &m_identity;
}

const Transform* PieceTransformsTrigon::get_mirrored_horizontally(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_refl;
    else if (transf == &m_rot60)
        result = &m_refl_rot300;
    else if (transf == &m_rot120)
        result = &m_refl_rot240;
    else if (transf == &m_rot180)
        result = &m_refl_rot180;
    else if (transf == &m_rot240)
        result = &m_refl_rot120;
    else if (transf == &m_rot300)
        result = &m_refl_rot60;
    else if (transf == &m_refl)
        result = &m_identity;
    else if (transf == &m_refl_rot60)
        result = &m_rot300;
    else if (transf == &m_refl_rot120)
        result = &m_rot240;
    else if (transf == &m_refl_rot180)
        result = &m_rot180;
    else if (transf == &m_refl_rot240)
        result = &m_rot120;
    else if (transf == &m_refl_rot300)
        result = &m_rot60;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsTrigon::get_mirrored_vertically(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_refl_rot180;
    else if (transf == &m_rot60)
        result = &m_refl_rot120;
    else if (transf == &m_rot120)
        result = &m_refl_rot60;
    else if (transf == &m_rot180)
        result = &m_refl;
    else if (transf == &m_rot240)
        result = &m_refl_rot300;
    else if (transf == &m_rot300)
        result = &m_refl_rot240;
    else if (transf == &m_refl)
        result = &m_rot180;
    else if (transf == &m_refl_rot60)
        result = &m_rot120;
    else if (transf == &m_refl_rot120)
        result = &m_rot60;
    else if (transf == &m_refl_rot180)
        result = &m_identity;
    else if (transf == &m_refl_rot240)
        result = &m_rot300;
    else if (transf == &m_refl_rot300)
        result = &m_rot240;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsTrigon::get_rotated_anticlockwise(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_rot300;
    else if (transf == &m_rot60)
        result = &m_identity;
    else if (transf == &m_rot120)
        result = &m_rot60;
    else if (transf == &m_rot180)
        result = &m_rot120;
    else if (transf == &m_rot240)
        result = &m_rot180;
    else if (transf == &m_rot300)
        result = &m_rot240;
    else if (transf == &m_refl)
        result = &m_refl_rot300;
    else if (transf == &m_refl_rot60)
        result = &m_refl;
    else if (transf == &m_refl_rot120)
        result = &m_refl_rot60;
    else if (transf == &m_refl_rot180)
        result = &m_refl_rot120;
    else if (transf == &m_refl_rot240)
        result = &m_refl_rot180;
    else if (transf == &m_refl_rot300)
        result = &m_refl_rot240;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsTrigon::get_rotated_clockwise(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_rot60;
    else if (transf == &m_rot60)
        result = &m_rot120;
    else if (transf == &m_rot120)
        result = &m_rot180;
    else if (transf == &m_rot180)
        result = &m_rot240;
    else if (transf == &m_rot240)
        result = &m_rot300;
    else if (transf == &m_rot300)
        result = &m_identity;
    else if (transf == &m_refl)
        result = &m_refl_rot60;
    else if (transf == &m_refl_rot60)
        result = &m_refl_rot120;
    else if (transf == &m_refl_rot120)
        result = &m_refl_rot180;
    else if (transf == &m_refl_rot180)
        result = &m_refl_rot240;
    else if (transf == &m_refl_rot240)
        result = &m_refl_rot300;
    else if (transf == &m_refl_rot300)
        result = &m_refl;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
