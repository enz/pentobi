//-----------------------------------------------------------------------------
/** @file pentobi/ExportImage.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_EXPORT_IMAGE_H
#define PENTOBI_EXPORT_IMAGE_H

#include "libpentobi_base/Grid.h"

class QString;
class QWidget;

namespace libpentobi_base {
class Board;
}

using libpentobi_base::Board;
using libpentobi_base::Grid;

//-----------------------------------------------------------------------------

void exportImage(QWidget* parent, const Board& bd, bool coordinates,
                 const Grid<QString>& labels);

//-----------------------------------------------------------------------------

#endif // PENTOBI_EXPORT_IMAGE_H
