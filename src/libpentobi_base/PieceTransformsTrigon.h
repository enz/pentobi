//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsTrigon.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H
#define LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H

#include "PieceTransforms.h"

namespace libpentobi_base {

using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfTrigonRefl;
using libboardgame_base::TransfTrigonReflRot60;
using libboardgame_base::TransfTrigonReflRot120;
using libboardgame_base::TransfTrigonReflRot180;
using libboardgame_base::TransfTrigonReflRot240;
using libboardgame_base::TransfTrigonReflRot300;
using libboardgame_base::TransfTrigonRot60;
using libboardgame_base::TransfTrigonRot120;
using libboardgame_base::TransfTrigonRot180;
using libboardgame_base::TransfTrigonRot240;
using libboardgame_base::TransfTrigonRot300;

//-----------------------------------------------------------------------------

class PieceTransformsTrigon
    : public PieceTransforms
{
public:
    PieceTransformsTrigon();

    const Transform* get_mirrored_horizontally(const Transform* transf) const;

    const Transform* get_mirrored_vertically(const Transform* transf) const;

    const Transform* get_rotated_anticlockwise(const Transform* transf) const;

    const Transform* get_rotated_clockwise(const Transform* transf) const;

private:
    TransfIdentity m_identity;

    TransfTrigonRot60 m_rot60;

    TransfTrigonRot120 m_rot120;

    TransfTrigonRot180 m_rot180;

    TransfTrigonRot240 m_rot240;

    TransfTrigonRot300 m_rot300;

    TransfTrigonRefl m_refl;

    TransfTrigonReflRot60 m_refl_rot60;

    TransfTrigonReflRot120 m_refl_rot120;

    TransfTrigonReflRot180 m_refl_rot180;

    TransfTrigonReflRot240 m_refl_rot240;

    TransfTrigonReflRot240 m_refl_rot300;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H
