//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsTrigon.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_TRANSFORMS_TRIGON_H
#define LIBPENTOBI_BASE_PIECE_TRANSFORMS_TRIGON_H

#include "PieceTransforms.h"
#include "TrigonTransform.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class PieceTransformsTrigon final
    : public PieceTransforms
{
public:
    PieceTransformsTrigon();

    const Transform* get_mirrored_horizontally(
            const Transform* transf) const override;

    const Transform* get_mirrored_vertically(
            const Transform* transf) const override;

    const Transform* get_rotated_anticlockwise(
            const Transform* transf) const override;

    const Transform* get_rotated_clockwise(
            const Transform* transf) const override;

private:
    TransfTrigonIdentity m_identity;

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

    TransfTrigonReflRot300 m_refl_rot300;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_TRANSFORMS_TRIGON_H
