//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceTransforms.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceTransforms.h"

namespace libpentobi_base {

using libboardgame_base::TransfIdentity;

//-----------------------------------------------------------------------------

PieceTransforms::~PieceTransforms()
{
}

const Transform* PieceTransforms::get_identity() const
{
    auto t = find<TransfIdentity>();
    LIBBOARDGAME_ASSERT(t != nullptr);
    return t;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
