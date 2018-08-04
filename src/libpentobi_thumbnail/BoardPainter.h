//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_BOARD_PAINTER_H
#define LIBPENTOBI_GUI_BOARD_PAINTER_H

#include <QPainter>
#include "libpentobi_base/Grid.h"
#include "libpentobi_base/Board.h"

using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::Variant;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::MovePoints;
using libpentobi_base::PieceInfo;
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
    enum
    {
        dot = 1 << 1,

        circle = 1 << 2
    };

    BoardPainter();

    void setCoordinates(bool enable) { m_coordinates = enable; }

    void setCoordinateColor(const QColor& color) { m_coordinateColor = color; }

    /** Paint the board.
        This function must be called before painting any pieces because it
        initializes some members that are used by the piece painting
        functions. */
    void paintEmptyBoard(QPainter& painter, unsigned width, unsigned height,
                         Variant variant, const Geometry& geo);

    /** Paint the pieces and markup.
        The pieceId parameter only needs to be initialized in game variant
        Nexos and is needed to paint the junctions between segment. Only
        segment points of pieceId are used (point type 1 or 2) and must be 0 if
        the point is empty or contain a unique value for segments of the same
        piece. */
    void paintPieces(QPainter& painter, const Grid<PointState>& pointState,
                     const Grid<unsigned>& pieceId,
                     const Grid<QString>* labels = nullptr,
                     const Grid<int>* marks = nullptr);

    /** Paint the selected piece.
        Paints the selected piece either transparent (if not legal) or opaque
        (if legal). */
    void paintSelectedPiece(QPainter& painter, Color c,
                            const MovePoints& points, bool isOnePiece,
                            bool isLegal);

    /** Get the corresponding board coordinates of a pixel.
        @return The board coordinates or CoordPoint::null() if paint() was
        not called yet or the pixel is outside the board. */
    CoordPoint getCoordPoint(int x, int y);

    bool hasPainted() const { return m_hasPainted; }

private:
    bool m_hasPainted = false;

    bool m_coordinates = false;

    bool m_isTrigon;

    bool m_isNexos;

    bool m_isCallisto;

    bool m_isGembloQ;

    const Geometry* m_geo;

    Variant m_variant;

    /** The width of the last board painted. */
    int m_width;

    /** The height of the last board painted. */
    int m_height;

    QColor m_coordinateColor = Qt::black;

    qreal m_fieldWidth;

    qreal m_fieldHeight;

    QPointF m_boardOffset;

    QFont m_font;

    QFont m_fontCondensed;

    QFont m_fontSemiCondensed;

    QFont m_fontCoordLabels;

    StartingPoints m_startingPoints;


    void paintCoordinates(QPainter& painter);

    void paintJunction(QPainter& painter, Variant variant,
                       const Grid<PointState>& pointState,
                       const Grid<unsigned>& pieceId, int x, int y,
                       qreal fieldX, qreal fieldY);

    void paintLabel(QPainter& painter, qreal x, qreal y, qreal width,
                    qreal height, const QString& label, bool isCoordLabel);

    void paintLabels(QPainter& painter, const Grid<PointState>& pointState,
                     Variant variant, const Grid<QString>& labels);

    void paintMarks(QPainter& painter, const Grid<PointState>& pointState,
                    Variant variant, const Grid<int>& marks);

    void paintStartingPoints(QPainter& painter, Variant variant,
                             const Grid<PointState>& pointState,
                             ColorMap<bool> isFirstPiece);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_BOARD_PAINTER_H
