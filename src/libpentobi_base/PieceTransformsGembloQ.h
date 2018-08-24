//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsGembloQ.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_TRANSFORMS_GEMBLOQ_H
#define LIBPENTOBI_BASE_PIECE_TRANSFORMS_GEMBLOQ_H

#include "PieceTransforms.h"
#include "GembloQTransform.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class PieceTransformsGembloQ final
    : public PieceTransforms
{
public:
    PieceTransformsGembloQ();

    const Transform* get_mirrored_horizontally(
            const Transform* transf) const override;

    const Transform* get_mirrored_vertically(
            const Transform* transf) const override;

    const Transform* get_rotated_anticlockwise(
            const Transform* transf) const override;

    const Transform* get_rotated_clockwise(
            const Transform* transf) const override;

    const Transform* get_default() const override;

private:
    TransfGembloQIdentity m_identity;

    TransfGembloQRot90 m_rot90;

    TransfGembloQRot180 m_rot180;

    TransfGembloQRot270 m_rot270;

    TransfGembloQRefl m_refl;

    TransfGembloQRot90Refl m_rot90refl;

    TransfGembloQRot180Refl m_rot180refl;

    TransfGembloQRot270Refl m_rot270refl;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_TRANSFORMS_GEMBLOQ_H
