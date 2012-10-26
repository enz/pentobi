//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
#define LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H

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

    int heightForWidth(int width) const;

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
    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

private:
    const Board& m_bd;

    Piece m_piece;

    const Transform* m_transform;

    bool m_isColorSelected;

    Color m_color;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
