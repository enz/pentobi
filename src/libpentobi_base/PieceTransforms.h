//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransforms.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_PIECE_TRANSFORMS_H
#define LIBPENTOBI_PIECE_TRANSFORMS_H

#include <vector>
#include "libboardgame_base/Transform.h"

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

    virtual const Transform* get_default() const;

    const vector<const Transform*>& get_all() const;

    /** Find the transform by its class.
        @tparam T The class of the transform.
        @return The pointer to the transform or null if the transforms do not
        contain the instance of the given class. */
    template<class T>
    const Transform* find() const;

protected:
    /** All piece transformations.
        Must be initialized in constructor of subclass. */
    vector<const Transform*> m_all;
};

template<class T>
const Transform* PieceTransforms::find() const
{
    for (auto t : m_all)
        if (dynamic_cast<const T*>(t))
            return t;
    return nullptr;
}

inline const Transform* PieceTransforms::get_default() const
{
    return m_all[0];
}

inline const vector<const Transform*>& PieceTransforms::get_all() const
{
    return m_all;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_PIECE_TRANSFORMS_H
