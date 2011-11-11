//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsClassic.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceTransformsClassic.h"

#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

PieceTransformsClassic::PieceTransformsClassic()
  : m_identity(Transform::identity),
    m_rot90(Transform::rotate_90),
    m_rot180(Transform::rotate_180),
    m_rot270(Transform::rotate_270),
    m_refl(Transform::mirror),
    m_rot90refl(Transform::rotate_90_mirror),
    m_rot180refl(Transform::rotate_180_mirror),
    m_rot270refl(Transform::rotate_270_mirror)
{
    m_all.reserve(8);
    m_all.push_back(&m_identity);
    m_all.push_back(&m_rot90);
    m_all.push_back(&m_rot180);
    m_all.push_back(&m_rot270);
    m_all.push_back(&m_refl);
    m_all.push_back(&m_rot90refl);
    m_all.push_back(&m_rot180refl);
    m_all.push_back(&m_rot270refl);
}

const Transform* PieceTransformsClassic::get_mirrored_horizontally(
                                                  const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_refl;
    if (transf == &m_rot90)
        return &m_rot270refl;
    if (transf == &m_rot180)
        return &m_rot180refl;
    if (transf == &m_rot270)
        return &m_rot90refl;
    if (transf == &m_refl)
        return &m_identity;
    if (transf == &m_rot90refl)
        return &m_rot270;
    if (transf == &m_rot180refl)
        return &m_rot180;
    if (transf == &m_rot270refl)
        return &m_rot90;
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsClassic::get_mirrored_vertically(
                                                  const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_rot180refl;
    if (transf == &m_rot90)
        return &m_rot90refl;
    if (transf == &m_rot180)
        return &m_refl;
    if (transf == &m_rot270)
        return &m_rot270refl;
    if (transf == &m_refl)
        return &m_rot180;
    if (transf == &m_rot90refl)
        return &m_rot90;
    if (transf == &m_rot180refl)
        return &m_identity;
    if (transf == &m_rot270refl)
        return &m_rot270;
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsClassic::get_rotated_anticlockwise(
                                                  const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_rot270;
    if (transf == &m_rot90)
        return &m_identity;
    if (transf == &m_rot180)
        return &m_rot90;
    if (transf == &m_rot270)
        return &m_rot180;
    if (transf == &m_refl)
        return &m_rot270refl;
    if (transf == &m_rot90refl)
        return &m_refl;
    if (transf == &m_rot180refl)
        return &m_rot90refl;
    if (transf == &m_rot270refl)
        return &m_rot180refl;
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsClassic::get_rotated_clockwise(
                                                  const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_rot90;
    if (transf == &m_rot90)
        return &m_rot180;
    if (transf == &m_rot180)
        return &m_rot270;
    if (transf == &m_rot270)
        return &m_identity;
    if (transf == &m_refl)
        return &m_rot90refl;
    if (transf == &m_rot90refl)
        return &m_rot180refl;
    if (transf == &m_rot180refl)
        return &m_rot270refl;
    if (transf == &m_rot270refl)
        return &m_refl;
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
