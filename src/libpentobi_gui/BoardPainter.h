//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_BOARD_PAINTER_H
#define LIBPENTOBI_GUI_BOARD_PAINTER_H

#include <QtGui>
#include "MarkupFlags.h"
#include "libpentobi_base/Grid.h"
#include "libpentobi_base/Board.h"

using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::FullGrid;
using libpentobi_base::GameVariant;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::PointStateExt;
using libpentobi_base::StartingPoints;

//-----------------------------------------------------------------------------

/** Paints a board.
    The painter can be used without having to create an instance of class Board,
    which is undesirable for use cases like the thumbnailer because of the slow
    creation of the BoardConst class. Instead, the board state is passed to the
    paint() function as a grid of point states. */
class BoardPainter
{
public:
    BoardPainter();

    void clearSelectedPiece();

    void setDrawCoordLabels(bool enable);

    void setCoordLabelColor(const QColor& color);

    void setSelectedPiece(Color c, const MovePoints& points, bool isLegal);

    void paint(QPainter& painter, unsigned int width, unsigned int height,
               GameVariant gameVariant,
               const FullGrid<PointStateExt>& pointState,
               const Grid<QString>* labels = 0,
               const Grid<MarkupFlags>* markupFlags = 0);

    /** Get the corresponding board coordinates of a pixel.
        @return The board coordinates or CoordPoint::null() if paint() was
        not called yet or the pixel is outside the board. */
    CoordPoint getCoordPoint(int x, int y);

    QRect getRect(Point p) const;

    bool hasPainted() const;

private:
    bool m_hasPainted;

    bool m_drawCoordLabels;

    /** The width of the last board painted. */
    int m_width;

    /** The height of the last board painted. */
    int m_height;

    /** The width in pixels of the last board painted. */
    int m_boardWidth;

    /** The height in pixels of the last board painted. */
    int m_boardHeight;

    Color m_selectedPieceColor;

    MovePoints m_selectedPiecePoints;

    bool m_isSelectedPieceLegal;

    QColor m_coordLabelColor;

    int m_fieldWidth;

    int m_fieldHeight;

    QPoint m_boardOffset;

    QFont m_font;

    QFont m_fontUnderlined;

    QFont m_fontSmall;

    StartingPoints m_startingPoints;

    void drawLabel(QPainter& painter, int x, int y, const QString& label,
                   bool underline, bool smallFont = false);

    void drawSelectedPiece(QPainter& painter, GameVariant gameVariant,
                           const Geometry& geometry,
                           const FullGrid<PointStateExt>& pointState);
};

inline void BoardPainter::clearSelectedPiece()
{
    m_selectedPiecePoints.clear();
}

inline void BoardPainter::setCoordLabelColor(const QColor& color)
{
    m_coordLabelColor = color;
}

inline bool BoardPainter::hasPainted() const
{
    return m_hasPainted;
}

inline void BoardPainter::setDrawCoordLabels(bool enable)
{
    m_drawCoordLabels = enable;
}

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_BOARD_PAINTER_H
