//-----------------------------------------------------------------------------
/** @file libpentobi_base/PentobiSgfUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PENTOBI_SGF_UTIL_H
#define LIBPENTOBI_BASE_PENTOBI_SGF_UTIL_H

#include "Color.h"
#include "Variant.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Get SGF move property ID for a color in a game variant. */
const char* get_color_id(Variant variant, Color c);

/** Get SGF setup property ID for a color in a game variant. */
const char* get_setup_id(Variant variant, Color c);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PENTOBI_SGF_UTIL_H
