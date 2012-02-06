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
using libpentobi_base::GameVariant;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::PointState;
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

    void setDrawCoordLabels(bool enable);

    void setCoordLabelColor(const QColor& color);

    void paintEmptyBoard(QPainter& painter, unsigned int width,
                         unsigned int height, GameVariant gameVariant,
                         const Geometry& geometry);

    /** Paint the pieces and markup.
        This function must only be called after paintEmptyBoard() because it
        uses the arguments from the paintEmptyBoard() function to determine the
        board properties. */
    void paintPieces(QPainter& painter, const Grid<PointState>& pointState,
                     const Grid<QString>* labels = 0,
                     const Grid<MarkupFlags>* markupFlags = 0);

    /** Paint the selected piece.
        Paints the selected piece either transparent (if not legal) or opaque
        (if legal). This function must only be called after paintEmptyBoard()
        because it uses the arguments from the paintEmptyBoard() function to
        determine the board properties. */
    void paintSelectedPiece(QPainter& painter, Color c,
                            const MovePoints& points, bool isLegal);

    /** Get the corresponding board coordinates of a pixel.
        @return The board coordinates or CoordPoint::null() if paint() was
        not called yet or the pixel is outside the board. */
    CoordPoint getCoordPoint(int x, int y);

    bool hasPainted() const;

private:
    bool m_hasPainted;

    bool m_drawCoordLabels;

    bool m_isTrigon;

    const Geometry* m_geometry;

    GameVariant m_gameVariant;

    /** The width of the last board painted. */
    int m_width;

    /** The height of the last board painted. */
    int m_height;

    QColor m_coordLabelColor;

    qreal m_fieldWidth;

    qreal m_fieldHeight;

    QPointF m_boardOffset;

    QFont m_font;

    QFont m_fontSmall;

    QFont m_fontUnderlined;

    StartingPoints m_startingPoints;

    void drawCoordinates(QPainter& painter, bool isTrigon);

    void drawLabel(QPainter& painter, qreal x, qreal y, qreal width,
                   qreal height, const QString& label, bool underline,
                   bool small);

    void drawLabels(QPainter& painter, const Grid<PointState>& pointState,
                    GameVariant gameVariant, const Grid<QString>* labels,
                    const Grid<MarkupFlags>* markupFlags);
};

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
