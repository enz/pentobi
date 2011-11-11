//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
#define LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H

#include <QtGui>
#include "libpentobi_base/Board.h"

using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

class OrientationDisplay
    : public QWidget
{
    Q_OBJECT

public:
    OrientationDisplay(QWidget* parent, const Board& bd);

    void selectColor(Color c);

    void clearSelectedColor();

    void clearSelectedPiece();

    void setSelectedPiece(const Piece& piece);

    void setSelectedPieceTransform(const Transform* transform);

protected:
    void paintEvent(QPaintEvent* event);

private:
    const Board& m_bd;

    const Piece* m_piece;

    const Transform* m_transform;

    bool m_isColorSelected;

    Color m_color;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_ORIENTATION_DISPLAY_H
