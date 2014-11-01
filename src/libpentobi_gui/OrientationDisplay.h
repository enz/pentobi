//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
#define LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QWidget>
#include "libpentobi_base/Board.h"

using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;

//-----------------------------------------------------------------------------

class OrientationDisplay
    : public QWidget
{
    Q_OBJECT

public:
    OrientationDisplay(QWidget* parent, const Board& bd);

    int heightForWidth(int width) const override;

    void selectColor(Color c);

    void clearSelectedColor();

    void clearSelectedPiece();

    void setSelectedPiece(Piece piece);

    void setSelectedPieceTransform(const Transform* transform);

signals:
    /** A mouse click on the orientation display while a color but no no piece
        was selected. */
    void colorClicked(Color color);

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    const Board& m_bd;

    Piece m_piece;

    const Transform* m_transform;

    bool m_isColorSelected;

    Color m_color;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
