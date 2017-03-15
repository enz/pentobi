//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoard.h"

#include <QApplication>
#include <QMouseEvent>
#include "libboardgame_base/GeometryUtil.h"
#include "libboardgame_base/Transform.h"

using namespace std;
using libboardgame_base::Transform;
using libboardgame_base::geometry_util::type_match_offset;
using libpentobi_base::Geometry;
using libpentobi_base::MovePoints;
using libpentobi_base::PiecePoints;
using libpentobi_base::PieceSet;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

bool allPointEmpty(const Board& bd, Move mv)
{
    for (Point p : bd.get_move_points(mv))
        if (! bd.get_point_state(p).is_empty())
            return false;
    return true;
}

QPixmap* createPixmap(const QPainter& painter, const QSize& size)
{
    auto devicePixelRatio = painter.device()->devicePixelRatio();
    auto pixmap = new QPixmap(devicePixelRatio * size);
    pixmap->setDevicePixelRatio(devicePixelRatio);
    return pixmap;
}

} // namespace

//-----------------------------------------------------------------------------

GuiBoard::GuiBoard(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd)
{
    setMinimumSize(350, 350);
    connect(&m_currentMoveShownAnimationTimer, SIGNAL(timeout()),
            SLOT(showMoveAnimation()));
}

void GuiBoard::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::StyleChange)
        setEmptyBoardDirty();
}

void GuiBoard::clearMarkup()
{
    for (Point p : m_bd)
    {
        m_marks[p] = 0;
        setLabel(p, "");
    }
}

void GuiBoard::clearPiece()
{
    m_selectedPiece = Piece::null();
    m_selectedPieceTransform = nullptr;
    setSelectedPiecePoints();
    setMouseTracking(false);
}

void GuiBoard::copyFromBoard(const Board& bd)
{
    auto& geo = bd.get_geometry();
    auto variant = bd.get_variant();
    m_pointState.copy_from(bd.get_point_state(), geo);
    auto pieceSet = get_piece_set(variant);
    if (pieceSet == PieceSet::nexos || pieceSet == PieceSet::callisto)
    {
        m_pieceId.fill(0, geo);
        unsigned n = 0;
        for (Color c : bd.get_colors())
            for (Move mv : bd.get_setup().placements[c])
            {
                ++n;
                for (Point p : bd.get_move_points(mv))
                    m_pieceId[p] = n;
            }
        for (auto mv : bd.get_moves())
        {
            ++n;
            for (Point p : bd.get_move_points(mv.move))
                m_pieceId[p] = n;
        }
    }
    if (! m_isInitialized || m_variant != variant)
    {
        m_variant = variant;
        m_isInitialized = true;
        m_labels.fill("", geo);
        m_marks.fill(0, geo);
        setEmptyBoardDirty();
    }
    else
        setDirty();
}

Move GuiBoard::findSelectedPieceMove()
{
    auto& points = m_bd.get_piece_info(m_selectedPiece).get_points();
    auto& geo = m_bd.get_geometry();
    auto width = static_cast<int>(geo.get_width());
    auto height = static_cast<int>(geo.get_height());
    MovePoints movePoints;
    for (CoordPoint p : points)
    {
        p = m_selectedPieceTransform->get_transformed(p);
        int x = p.x + m_selectedPieceOffset.x;
        int y = p.y + m_selectedPieceOffset.y;
        if (x < 0 || x >= width || y < 0 || y >= height)
            return Move::null();
        Point pp = geo.get_point(x, y);
        if (pp.is_null())
            return Move::null();
        movePoints.push_back(pp);
    }
    Move mv;
    if (! m_bd.find_move(movePoints, m_selectedPiece, mv)
            || (m_freePlacement && ! allPointEmpty(m_bd, mv))
            || (! m_freePlacement
                && ! m_bd.is_legal(m_selectedPieceColor, mv)))
        return Move::null();
    else
        return mv;
}

bool GuiBoard::isSelectedPiecePartiallyOnBoard(CoordPoint offset) const
{
    if (m_selectedPiece.is_null() || offset.is_null()
            || ! m_selectedPieceTransform)
        return false;
    auto& points = m_bd.get_piece_info(m_selectedPiece).get_points();
    auto& geo = m_bd.get_geometry();
    auto width = static_cast<int>(geo.get_width());
    auto height = static_cast<int>(geo.get_height());
    for (auto p : points)
    {
        p = m_selectedPieceTransform->get_transformed(p);
        auto x = p.x + offset.x;
        auto y = p.y + offset.y;
        if (x < 0 || x >= width || y < 0 || y >= height)
            continue;
        Point pp = geo.get_point(x, y);
        if (! pp.is_null())
            return true;
    }
    return false;
}

void GuiBoard::leaveEvent(QEvent*)
{
    m_selectedPieceOffset = CoordPoint::null();
    setSelectedPiecePoints();
}

void GuiBoard::mouseMoveEvent(QMouseEvent* event)
{
    if (m_selectedPiece.is_null())
        return;
    auto oldOffset = m_selectedPieceOffset;
    setSelectedPieceOffset(*event);
    if (m_selectedPieceOffset != oldOffset)
        setSelectedPiecePoints();
}

void GuiBoard::mousePressEvent(QMouseEvent* event)
{
    if (m_selectedPiece.is_null())
    {
        CoordPoint p = m_boardPainter.getCoordPoint(event->x(), event->y());
        auto& geo = m_bd.get_geometry();
        if (geo.is_onboard(p))
            emit pointClicked(geo.get_point(p.x, p.y));
        return;
    }
    setSelectedPieceOffset(*event);
    placePiece();
}

void GuiBoard::movePieceDown()
{
    if (m_selectedPiece.is_null())
        return;
    if (m_selectedPieceOffset.is_null())
    {
        auto& geo = m_bd.get_geometry();
        auto width = static_cast<int>(geo.get_width());
        auto height = static_cast<int>(geo.get_height());
        for (int y = 0; y < height; ++y)
        {
            CoordPoint offset(width / 2, y);
            if (isSelectedPiecePartiallyOnBoard(offset))
            {
                setSelectedPieceOffset(offset, false, true);
                setSelectedPiecePoints();
                return;
            }
        }
    }
    else
    {
        auto offset = m_selectedPieceOffset;
        ++offset.y;
        setSelectedPieceOffset(offset, offset.y % 2 == 0, true);
        setSelectedPiecePoints();
    }
}

void GuiBoard::movePieceLeft()
{
    if (m_selectedPiece.is_null())
        return;
    if (m_selectedPieceOffset.is_null())
    {
        auto& geo = m_bd.get_geometry();
        auto width = static_cast<int>(geo.get_width());
        auto height = static_cast<int>(geo.get_height());
        for (int x = width - 1; x >= 0; --x)
        {
            CoordPoint offset(x, height / 2);
            if (isSelectedPiecePartiallyOnBoard(offset))
            {
                setSelectedPieceOffset(offset, false);
                setSelectedPiecePoints();
                return;
            }
        }
    }
    else
    {
        auto offset = m_selectedPieceOffset;
        --offset.x;
        setSelectedPieceOffset(offset, false);
        setSelectedPiecePoints();
    }
}

void GuiBoard::movePieceRight()
{
    if (m_selectedPiece.is_null())
        return;
    CoordPoint offset;
    if (m_selectedPieceOffset.is_null())
    {
        auto& geo = m_bd.get_geometry();
        auto width = static_cast<int>(geo.get_width());
        auto height = static_cast<int>(geo.get_height());
        for (int x = 0; x < width; ++x)
        {
            CoordPoint offset(x, height / 2);
            if (isSelectedPiecePartiallyOnBoard(offset))
            {
                setSelectedPieceOffset(offset, true);
                setSelectedPiecePoints();
                return;
            }
        }
    }
    else
    {
        auto offset = m_selectedPieceOffset;
        ++offset.x;
        setSelectedPieceOffset(offset, true);
        setSelectedPiecePoints();
    }
}

void GuiBoard::movePieceUp()
{
    if (m_selectedPiece.is_null())
        return;
    if (m_selectedPieceOffset.is_null())
    {
        auto& geo = m_bd.get_geometry();
        auto width = static_cast<int>(geo.get_width());
        auto height = static_cast<int>(geo.get_height());
        for (int y = height - 1; y >= 0; --y)
        {
            CoordPoint offset(width / 2, y);
            if (isSelectedPiecePartiallyOnBoard(offset))
            {
                setSelectedPieceOffset(offset, false, false);
                setSelectedPiecePoints();
                return;
            }
        }
    }
    else
    {
        auto offset = m_selectedPieceOffset;
        --offset.y;
        setSelectedPieceOffset(offset, offset.y % 2 == 0, false);
        setSelectedPiecePoints();
    }
}

void GuiBoard::paintEvent(QPaintEvent*)
{
    if (! m_isInitialized)
        return;
    QPainter painter(this);
    if (! m_emptyBoardPixmap || m_emptyBoardPixmap->size() != size())
    {
        m_emptyBoardPixmap.reset(createPixmap(painter, size()));
        m_emptyBoardDirty = true;
    }
    if (! m_boardPixmap || m_boardPixmap->size() != size())
    {
        m_boardPixmap.reset(createPixmap(painter, size()));
        m_dirty = true;
    }
    if (m_emptyBoardDirty)
    {
        QColor coordLabelColor =
            QApplication::palette().color(QPalette::WindowText);
        m_boardPainter.setCoordinateColor(coordLabelColor);
        m_emptyBoardPixmap->fill(Qt::transparent);
        QPainter painter(m_emptyBoardPixmap.get());
        m_boardPainter.paintEmptyBoard(painter, width(), height(), m_variant,
                                       m_bd.get_geometry());
        m_emptyBoardDirty = false;
    }
    if (m_dirty)
    {
        m_boardPixmap->fill(Qt::transparent);
        QPainter painter(m_boardPixmap.get());
        painter.drawPixmap(0, 0, *m_emptyBoardPixmap);
        m_boardPainter.paintPieces(painter, m_pointState, m_pieceId, &m_labels,
                                   &m_marks);
        m_dirty = false;
    }
    painter.drawPixmap(0, 0, *m_boardPixmap);
    if (m_isMoveShown)
    {
        if (m_currentMoveShownAnimationIndex % 2 == 0)
            m_boardPainter.paintSelectedPiece(painter, m_currentMoveShownColor,
                                              m_currentMoveShownPoints, true);
    }
    else if (! m_selectedPiecePoints.empty())
    {
        bool isLegal = ! findSelectedPieceMove().is_null();
        m_boardPainter.paintSelectedPiece(painter, m_selectedPieceColor,
                                          m_selectedPiecePoints, isLegal);
    }
}

void GuiBoard::placePiece()
{
    auto mv = findSelectedPieceMove();
    if (! mv.is_null())
        emit play(m_selectedPieceColor, mv);
}

void GuiBoard::selectPiece(Color color, Piece piece)
{
    if (m_selectedPiece == piece && m_selectedPieceColor == color)
        return;
    m_selectedPieceColor = color;
    m_selectedPieceTransform = m_bd.get_transforms().get_default();
    if (m_selectedPiece.is_null())
        m_selectedPieceOffset = CoordPoint::null();
    m_selectedPiece = piece;
    auto wantedOffset = m_selectedPieceOffset;
    m_selectedPieceOffset = CoordPoint::null();
    setSelectedPieceOffset(wantedOffset);
    setSelectedPiecePoints();
    setMouseTracking(true);
}

void GuiBoard::setEmptyBoardDirty()
{
    m_emptyBoardDirty = true;
    m_dirty = true;
    update();
}

void GuiBoard::setDirty()
{
    m_dirty = true;
    update();
}

void GuiBoard::setCoordinates(bool enable)
{
    m_boardPainter.setCoordinates(enable);
    setEmptyBoardDirty();
}

void GuiBoard::setFreePlacement(bool enable)
{
    m_freePlacement = enable;
    update();
}

void GuiBoard::setLabel(Point p, const QString& text)
{
    if (! m_isInitialized)
        return;
    if (m_labels[p] != text)
    {
        m_labels[p] = text;
        setDirty();
    }
}

void GuiBoard::setMark(Point p, int mark, bool enable)
{
    if (! m_isInitialized)
        return;
    if (((m_marks[p] & mark) != 0) != enable)
    {
        m_marks[p] ^= mark;
        setDirty();
    }
}

void GuiBoard::setSelectedPieceOffset(const QMouseEvent& event)
{
    setSelectedPieceOffset(m_boardPainter.getCoordPoint(event.x(), event.y()));
}

/** Set new selected piece offset.
    Shifts the wanted offset to match the geometry given the currently selected
    piece and transformation and sets the new offset if all elements of the
    piece are on the board.
    @param wantedOffset
    @param preferRightShift Prefer a geometry-matching shift to the right
    instead of to the left.
    @param preferDownShift Prefer a downward geometry-matching shift
    instead of upward. */
void GuiBoard::setSelectedPieceOffset(const CoordPoint& wantedOffset,
                                      bool preferRightShift,
                                      bool preferDownShift)
{
    auto& geo = m_bd.get_geometry();
    auto transformPointType = m_selectedPieceTransform->get_point_type();
    auto offset =
            wantedOffset
            - type_match_offset(geo, geo.get_point_type(wantedOffset))
            + type_match_offset(geo, transformPointType);
    if (preferRightShift)
    {
        if (offset.x < wantedOffset.x)
            offset.x += geo.get_period_x();
    }
    else if (offset.x > wantedOffset.x)
        offset.x -= geo.get_period_x();
    if (preferDownShift)
    {
        if (offset.y < wantedOffset.y)
            offset.y += geo.get_period_y();
    }
    else if (offset.y > wantedOffset.y)
        offset.y -= geo.get_period_y();
    if (isSelectedPiecePartiallyOnBoard(offset))
        m_selectedPieceOffset = offset;
}

void GuiBoard::setSelectedPiecePoints(Move mv)
{
    m_selectedPiecePoints.clear();
    for (Point p : m_bd.get_move_points(mv))
        m_selectedPiecePoints.push_back(p);
    update();
}

void GuiBoard::setSelectedPiecePoints()
{
    m_selectedPiecePoints.clear();
    if (! m_selectedPiece.is_null() && ! m_selectedPieceOffset.is_null())
    {
        auto& geo = m_bd.get_geometry();
        int width = static_cast<int>(geo.get_width());
        int height = static_cast<int>(geo.get_height());
        for (CoordPoint p : m_bd.get_piece_info(m_selectedPiece).get_points())
        {
            p = m_selectedPieceTransform->get_transformed(p);
            int x = p.x + m_selectedPieceOffset.x;
            int y = p.y + m_selectedPieceOffset.y;
            if (x >= 0 && x < width && y >= 0 && y < height)
                m_selectedPiecePoints.push_back(geo.get_point(x, y));
        }
    }
    update();
}

void GuiBoard::setSelectedPieceTransform(const Transform* transform)
{
    if (m_selectedPieceTransform == transform)
        return;
    m_selectedPieceTransform = transform;
    auto wantedOffset = m_selectedPieceOffset;
    m_selectedPieceOffset = CoordPoint::null();

    // Avoid a drift when changing the transformation multiple times
    auto preferRightShift = wantedOffset.x % 2 == 0;
    auto preferDownShift = wantedOffset.y % 2 == 0;
    setSelectedPieceOffset(wantedOffset, preferRightShift, preferDownShift);

    // Shift towards center if completely outside board
    auto& geo = m_bd.get_geometry();
    int width = static_cast<int>(geo.get_width());
    int height = static_cast<int>(geo.get_height());
    while (m_selectedPieceOffset.is_null())
    {
        preferRightShift = wantedOffset.x < width / 2;
        if (preferRightShift)
            ++wantedOffset.x;
        else
            --wantedOffset.x;
        preferDownShift = wantedOffset.y < height / 2;
        if (preferDownShift)
            ++wantedOffset.y;
        else
            --wantedOffset.y;
        setSelectedPieceOffset(wantedOffset, preferRightShift, preferDownShift);
    }

    setSelectedPiecePoints();
}

void GuiBoard::showMove(Color c, Move mv)
{
    m_isMoveShown = true;
    m_currentMoveShownColor = c;
    m_currentMoveShownPoints.clear();
    for (Point p : m_bd.get_move_points(mv))
        m_currentMoveShownPoints.push_back(p);
    m_currentMoveShownAnimationIndex = 0;
    m_currentMoveShownAnimationTimer.start(500);
    update();
}

void GuiBoard::showMoveAnimation()
{
    ++m_currentMoveShownAnimationIndex;
    if (m_currentMoveShownAnimationIndex > 5)
    {
        m_isMoveShown = false;
        m_currentMoveShownAnimationTimer.stop();
    }
    update();
}

//-----------------------------------------------------------------------------
