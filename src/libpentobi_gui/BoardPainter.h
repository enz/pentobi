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
using libpentobi_base::Grid;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::PointStateExt;

//-----------------------------------------------------------------------------

class BoardPainter
{
public:
    BoardPainter();

    void clearSelectedPiece();

    void setDrawCoordLabels(bool enable);

    void setCoordLabelColor(const QColor& color);

    void setSelectedPiece(Color c, const MovePoints& points);

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

    /** The size (rows/columns) of the last board painted. */
    int m_sz;

    /** The size in pixels of the last board painted. */
    int m_boardSize;

    Color m_selectedPieceColor;

    MovePoints m_selectedPiecePoints;

    QColor m_coordLabelColor;

    int m_squareSize;

    QPoint m_boardOffset;

    QFont m_font;

    void drawLabel(QPainter& painter, int x, int y, const QString& label);

    void drawSelectedPiece(QPainter& painter, GameVariant gameVariant,
                           const FullGrid<PointStateExt>& pointState);

    void drawVariationTriangle(QPainter& painter, int x, int y);
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
