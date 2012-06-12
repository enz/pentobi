//-----------------------------------------------------------------------------
/** @file libpentobi_base/SgfUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_SGF_UTIL_H
#define LIBPENTOBI_BASE_SGF_UTIL_H

#include "Color.h"
#include "Variant.h"

namespace libpentobi_base {
namespace sgf_util {

//-----------------------------------------------------------------------------

/** Get SGF move property ID for a color in a game variant. */
const char* get_color_id(Variant variant, Color c);

/** Get SGF setup property ID for a color in a game variant. */
const char* get_setup_id(Variant variant, Color c);

//-----------------------------------------------------------------------------

} // namespace sgf_util
} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_SGF_UTIL_H
