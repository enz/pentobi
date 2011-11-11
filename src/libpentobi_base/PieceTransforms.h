//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransforms.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_PIECE_TRANSFORMS_H
#define LIBPENTOBI_PIECE_TRANSFORMS_H

#include <vector>
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::Transform;

//-----------------------------------------------------------------------------

class PieceTransforms
{
public:
    virtual ~PieceTransforms();

    virtual const Transform* get_mirrored_horizontally(
                                             const Transform* transf) const = 0;

    virtual const Transform* get_mirrored_vertically(
                                             const Transform* transf) const = 0;

    virtual const Transform* get_rotated_anticlockwise(
                                             const Transform* transf) const = 0;

    virtual const Transform* get_rotated_clockwise(
                                             const Transform* transf) const = 0;

    const vector<const Transform*>& get_all() const;

    const Transform* get_default() const;

protected:
    /** All piece transformations.
        Must be initialized in constructor of subclass. */
    vector<const Transform*> m_all;
};

inline const vector<const Transform*>& PieceTransforms::get_all() const
{
    return m_all;
}

inline const Transform* PieceTransforms::get_default() const
{
    LIBBOARDGAME_ASSERT(m_all.size() > 0);
    return m_all[0];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_PIECE_TRANSFORMS_H
