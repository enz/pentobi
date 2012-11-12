//-----------------------------------------------------------------------------
/** @file pentobi/ExportImage.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_EXPORT_IMAGE_H
#define PENTOBI_EXPORT_IMAGE_H

class QString;
class QWidget;

namespace libpentobi_base {
class Board;
template<typename T> class Grid;
}

using libpentobi_base::Board;
using libpentobi_base::Grid;

//-----------------------------------------------------------------------------

void exportImage(QWidget* parent, const Board& bd, bool coordinates,
                 const Grid<QString>& labels);

//-----------------------------------------------------------------------------

#endif // PENTOBI_EXPORT_IMAGE_H
