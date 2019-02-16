//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsGembloQ.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PieceTransformsGembloQ.h"

#include "libboardgame_base/Assert.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

PieceTransformsGembloQ::PieceTransformsGembloQ()
{
    m_all.reserve(8);
    m_all.push_back(&m_identity);
    m_all.push_back(&m_rot90);
    m_all.push_back(&m_rot180);
    m_all.push_back(&m_rot270);
    m_all.push_back(&m_rot90refl);
    m_all.push_back(&m_rot180refl);
    m_all.push_back(&m_rot270refl);
    m_all.push_back(&m_refl);
}

const Transform* PieceTransformsGembloQ::get_mirrored_horizontally(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_refl;
    else if (transf == &m_rot90)
        result = &m_rot270refl;
    else if (transf == &m_rot180)
        result = &m_rot180refl;
    else if (transf == &m_rot270)
        result = &m_rot90refl;
    else if (transf == &m_refl)
        result = &m_identity;
    else if (transf == &m_rot90refl)
        result = &m_rot270;
    else if (transf == &m_rot180refl)
        result = &m_rot180;
    else if (transf == &m_rot270refl)
        result = &m_rot90;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsGembloQ::get_mirrored_vertically(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_rot180refl;
    else if (transf == &m_rot90)
        result = &m_rot90refl;
    else if (transf == &m_rot180)
        result = &m_refl;
    else if (transf == &m_rot270)
        result = &m_rot270refl;
    else if (transf == &m_refl)
        result = &m_rot180;
    else if (transf == &m_rot90refl)
        result = &m_rot90;
    else if (transf == &m_rot180refl)
        result = &m_identity;
    else if (transf == &m_rot270refl)
        result = &m_rot270;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsGembloQ::get_rotated_anticlockwise(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_rot270;
    else if (transf == &m_rot90)
        result = &m_identity;
    else if (transf == &m_rot180)
        result = &m_rot90;
    else if (transf == &m_rot270)
        result = &m_rot180;
    else if (transf == &m_refl)
        result = &m_rot270refl;
    else if (transf == &m_rot90refl)
        result = &m_refl;
    else if (transf == &m_rot180refl)
        result = &m_rot90refl;
    else if (transf == &m_rot270refl)
        result = &m_rot180refl;
    else
    {
        LIBBOARDGAME_ASSERT(false);
        result = nullptr;
    }
    return result;
}

const Transform* PieceTransformsGembloQ::get_rotated_clockwise(
                                                 const Transform* transf) const
{
    const Transform* result;
    if (transf == &m_identity)
        result = &m_rot90;
    else if (transf == &m_rot90)
        result = &m_rot180;
    else if (transf == &m_rot180)
        result = &m_rot270;
    else if (transf == &m_rot270)
        result = &m_identity;
    else if (transf == &m_refl)
        result = &m_rot90refl;
    else if (transf == &m_rot90refl)
        result = &m_rot180refl;
    else if (transf == &m_rot180refl)
        result = &m_rot270refl;
    else if (transf == &m_rot270refl)
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
