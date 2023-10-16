//-----------------------------------------------------------------------------
/** @file pentobi/Paint.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_PAINT_H
#define PENTOBI_PAINT_H

#include <QtGlobal>
#include "libpentobi_base/Grid.h"
#include "libpentobi_base/PointState.h"
#include "libpentobi_base/Variant.h"

class QColor;
class QPainter;

using libpentobi_base::Grid;
using libpentobi_base::Geometry;
using libpentobi_base::PointState;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Paint the board and pieces.
    This function takes a Grid<PointState> for the board positions instead of
    an instance of libpentobi_base::Board, because creating a Board is too
    expensive for lightweight use cases like a thumbnailer.

    The pieceId parameter only needs to be initialized in game variants Nexos
    and Callisto. It is needed to paint the junctions between piece elements.
    They must be 0 for empty points and contain a unique value for points
    of the same piece. */
void paint(QPainter& painter, qreal width, qreal height, Variant variant,
           const Geometry& geo, const Grid<PointState>& pointState,
           const Grid<unsigned>& pieceId);

/** Paint empty board. */
void paintBoard(QPainter& painter, qreal width, qreal height, Variant variant,
                const QColor& base, const QColor& light, const QColor& dark,
                const QColor& centerBase, const QColor& centerLight,
                const QColor& centerDark);

void paintCallistoOnePiece(QPainter& painter, qreal x, qreal y, qreal width,
                           qreal height, const QColor& base,
                           const QColor& light, const QColor& dark);

void paintJunctionAll(QPainter& painter, qreal x, qreal y, qreal width,
                      qreal height, const QColor& base);

void paintJunctionRight(QPainter& painter, qreal x, qreal y, qreal width,
                        qreal height, const QColor& base);

void paintJunctionStraight(QPainter& painter, qreal x, qreal y, qreal width,
                           qreal height, const QColor& base);

void paintJunctionT(QPainter& painter, qreal x, qreal y, qreal width,
                    qreal height, const QColor& base);

void paintQuarterSquare(QPainter& painter, qreal x, qreal y, qreal width,
                        qreal height, const QColor& base, const QColor& light);

void paintSquare(QPainter& painter, qreal x, qreal y, qreal width,
                 qreal height, const QColor& base, const QColor& light,
                 const QColor& dark);

void paintTriangleDown(QPainter& painter, qreal x, qreal y, qreal width,
                       qreal height, const QColor& base, const QColor& light,
                       const QColor& dark);

void paintTriangleUp(QPainter& painter, qreal x, qreal y, qreal width,
                     qreal height, const QColor& base, const QColor& light,
                     const QColor& dark);

//-----------------------------------------------------------------------------

#endif // PENTOBI_PAINT_H
