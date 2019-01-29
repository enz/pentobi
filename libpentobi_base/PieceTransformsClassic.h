//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsClassic.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_TRANSFORMS_CLASSIC_H
#define LIBPENTOBI_BASE_PIECE_TRANSFORMS_CLASSIC_H

#include "PieceTransforms.h"
#include "libboardgame_base/RectTransform.h"

namespace libpentobi_base {

using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfRectRot90;
using libboardgame_base::TransfRectRot180;
using libboardgame_base::TransfRectRot270;
using libboardgame_base::TransfRectRefl;
using libboardgame_base::TransfRectRot90Refl;
using libboardgame_base::TransfRectRot180Refl;
using libboardgame_base::TransfRectRot270Refl;

//-----------------------------------------------------------------------------

class PieceTransformsClassic final
    : public PieceTransforms
{
public:
    PieceTransformsClassic();

    const Transform* get_mirrored_horizontally(
            const Transform* transf) const override;

    const Transform* get_mirrored_vertically(
            const Transform* transf) const override;

    const Transform* get_rotated_anticlockwise(
            const Transform* transf) const override;

    const Transform* get_rotated_clockwise(
            const Transform* transf) const override;

private:
    TransfIdentity m_identity;

    TransfRectRot90 m_rot90;

    TransfRectRot180 m_rot180;

    TransfRectRot270 m_rot270;

    TransfRectRefl m_refl;

    TransfRectRot90Refl m_rot90refl;

    TransfRectRot180Refl m_rot180refl;

    TransfRectRot270Refl m_rot270refl;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_TRANSFORMS_CLASSIC_H
