//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsTrigon.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceTransformsTrigon.h"

#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using libboardgame_util::log;

//-----------------------------------------------------------------------------

PieceTransformsTrigon::PieceTransformsTrigon()
{
    m_all.reserve(12);
    m_all.push_back(&m_identity);
    m_all.push_back(&m_rot120);
    m_all.push_back(&m_rot240);
    m_all.push_back(&m_refl);
    m_all.push_back(&m_refl_rot120);
    m_all.push_back(&m_refl_rot240);
    log() << "TODO: define missing transformations for Trigon\n";
}

const Transform* PieceTransformsTrigon::get_mirrored_horizontally(
                                                  const Transform* transf) const
{
    if (transf == &m_identity)
        return &m_refl;
    if (transf == &m_refl)
        return &m_identity;
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsTrigon::get_mirrored_vertically(
                                                  const Transform* transf) const
{
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsTrigon::get_rotated_anticlockwise(
                                                  const Transform* transf) const
{
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const Transform* PieceTransformsTrigon::get_rotated_clockwise(
                                                  const Transform* transf) const
{
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
