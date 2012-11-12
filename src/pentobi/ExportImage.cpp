//-----------------------------------------------------------------------------
/** @file pentobi/ExportImage.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ExportImage.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QSettings>
#include "ShowMessage.h"
#include "libpentobi_gui/BoardPainter.h"

//-----------------------------------------------------------------------------

void exportImage(QWidget* parent, const Board& bd, bool coordinates,
                 const Grid<QString>& labels)
{
    QSettings settings;
    int size = settings.value("export_image_size", 420).toInt();
    bool ok;
    size = QInputDialog::getInt(parent,
                                qApp->translate("ExportImage", "Export Image"),
                                qApp->translate("ExportImage", "Image size:"),
                                size, 0, 2147483647, 40, &ok);
    if (! ok)
        return;
    settings.setValue("export_image_size", size);
    BoardPainter boardPainter;
    boardPainter.setCoordinates(coordinates);
    boardPainter.setCoordinateColor(QColor(100, 100, 100));
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    if (coordinates)
        painter.fillRect(0, 0, size, size, QColor(216, 216, 216));
    boardPainter.paintEmptyBoard(painter, size, size, bd.get_variant(),
                                 bd.get_geometry());
    boardPainter.paintPieces(painter, bd.get_grid(), &labels);
    painter.end();
    QString file;
    while (true)
    {
        file = QFileDialog::getSaveFileName(parent, file);
        if (file.isEmpty())
            break;
        QImageWriter writer(file);
        if (writer.write(image))
            break;
        else
            showError(parent, writer.errorString());
    }
}

//-----------------------------------------------------------------------------
