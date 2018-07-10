//-----------------------------------------------------------------------------
/** @file pentobi/Util.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_UTIL_H
#define PENTOBI_UTIL_H

#include "libboardgame_base/Rating.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Variant.h"

class QDialog;
class QString;

using libboardgame_base::Rating;
using libpentobi_base::Color;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

namespace Util {

/** Return the platform-dependent directory for storing data for the current
    application. */
QString getDataDir();

/** Create the platform-dependent directory for storing data for the current
    application if it does not exist yet. */
void initDataDir();

/** Set an empty window title for message boxes and similar small dialogs. */
void setNoTitle(QDialog& dialog);

} // namespace Util

//-----------------------------------------------------------------------------

#endif // PENTOBI_UTIL_H
