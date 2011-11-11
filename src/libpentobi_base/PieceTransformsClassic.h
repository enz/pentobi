//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsClassic.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_PIECE_TRANSFORMS_CLASSIC_H
#define LIBPENTOBI_PIECE_TRANSFORMS_CLASSIC_H

#include "PieceTransforms.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class PieceTransformsClassic
    : public PieceTransforms
{
public:
    PieceTransformsClassic();

    const Transform* get_mirrored_horizontally(const Transform* transf) const;

    const Transform* get_mirrored_vertically(const Transform* transf) const;

    const Transform* get_rotated_anticlockwise(const Transform* transf) const;

    const Transform* get_rotated_clockwise(const Transform* transf) const;

private:
    Transform m_identity;

    Transform m_rot90;

    Transform m_rot180;

    Transform m_rot270;

    Transform m_refl;

    Transform m_rot90refl;

    Transform m_rot180refl;

    Transform m_rot270refl;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_PIECE_TRANSFORMS_CLASSIC_H
