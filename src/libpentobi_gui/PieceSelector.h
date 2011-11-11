//-----------------------------------------------------------------------------
/** @file libpentobi_gui/PieceSelector.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_PIECE_SELECTOR_H
#define LIBPENTOBI_GUI_PIECE_SELECTOR_H

#include <QtGui>
#include "libpentobi_base/Board.h"
#include "libpentobi_base/Color.h"

using libboardgame_base::Transform;
using libpentobi_base::Color;
using libpentobi_base::Board;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

class PieceSelector
    : public QWidget
{
    Q_OBJECT

public:
    PieceSelector(QWidget* parent, const Board& bd, Color color);

    /** Needs to be called after the game variant of the current board has
        changed because references to pieces are only unique within a
        game variant. */
    void init();

    bool hasHeightForWidth() const;

    int heightForWidth(int width) const;

signals:
    void pieceSelected(Color color, const Piece& piece,
                       const Transform* transform);

protected:
    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

private:
    static const unsigned int nuColumns = 34;

    static const unsigned int nuRows = 6;

    const Board& m_bd;

    Color m_color;

    const Piece* m_piece[nuColumns][nuRows];

    const Transform* m_transform[nuColumns][nuRows];

    void findPiecePoints(const Piece& piece, unsigned int x,
                         unsigned int y, Piece::Points& points) const;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_PIECE_SELECTOR_H
