//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransformsTrigon.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H
#define LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H

#include "PieceTransforms.h"

namespace libpentobi_base {

using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfTrigonRefl;

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

    TransfTrigonRefl m_refl;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_PIECE_TRANSFORMS_TRIGON_H
