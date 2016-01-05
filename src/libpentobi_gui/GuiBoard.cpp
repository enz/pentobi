//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoard.h"

#include <limits>
#include <QApplication>
#include <QMouseEvent>
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::Transform;
using libpentobi_base::BoardType;
using libpentobi_base::Geometry;
using libpentobi_base::MovePoints;
using libpentobi_base::PiecePoints;
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
    setMinimumWidth(350);
    setMinimumHeight(350);
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
    if (get_board_type(variant) == BoardType::nexos)
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
    if (m_selectedPiece.is_null() || m_selectedPieceOffset.is_null())
        return Move::null();
    const PiecePoints& points =
        m_bd.get_piece_info(m_selectedPiece).get_points();
    auto& geo = m_bd.get_geometry();
    int width = static_cast<int>(geo.get_width());
    int height = static_cast<int>(geo.get_height());
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

int GuiBoard::heightForWidth(int width) const
{
    return width;
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
    CoordPoint oldOffset = m_selectedPieceOffset;
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
    auto& geo = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(geo.get_width() / 2, 0);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == BoardType::trigon
                || m_bd.get_board_type() == BoardType::trigon_3)
        {
            if (m_selectedPieceOffset.x % 2 == 0)
                ++newOffset.x;
            else
                --newOffset.x;
            ++newOffset.y;
        }
        else
            newOffset.y += geo.get_period_y();
        if (geo.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::movePieceLeft()
{
    if (m_selectedPiece.is_null())
        return;
    auto& geo = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(geo.get_width() - 1, geo.get_height() / 2);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        newOffset.x -= geo.get_period_x();
        if (geo.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::movePieceRight()
{
    if (m_selectedPiece.is_null())
        return;
    auto& geo = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(0, geo.get_height() / 2);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        newOffset.x += geo.get_period_x();
        if (geo.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::movePieceUp()
{
    if (m_selectedPiece.is_null())
        return;
    auto& geo = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(geo.get_width() / 2, geo.get_height() - 1);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == BoardType::trigon
                || m_bd.get_board_type() == BoardType::trigon_3)
        {
            if (m_selectedPieceOffset.x % 2 == 0)
                ++newOffset.x;
            else
                --newOffset.x;
            --newOffset.y;
        }
        else
            newOffset.y -= geo.get_period_y();
        if (geo.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            setSelectedPiecePoints();
        }
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
    setSelectedPieceOffset(m_selectedPieceOffset);
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

void GuiBoard::setSelectedPieceOffset(const CoordPoint& offset)
{
    if (offset.is_null())
    {
        m_selectedPieceOffset = offset;
        return;
    }
    auto& geo = m_bd.get_geometry();
    auto board_type = m_bd.get_board_type();
    unsigned old_point_type = geo.get_point_type(offset);
    CoordPoint type_matched_offset = offset;
    if (board_type == BoardType::trigon || board_type == BoardType::trigon_3)
    {
        // Offset must match the point type (triangle up/down) of
        // CoordPoint(0, 0) after the piece transformation
        unsigned point_type = m_selectedPieceTransform->get_new_point_type();
        bool hasLeft = geo.is_onboard(CoordPoint(offset.x - 1, offset.y));
        bool hasRight = geo.is_onboard(CoordPoint(offset.x + 1, offset.y));
        if (old_point_type != point_type)
        {
            if ((point_type == 0 && hasRight)
                    || (point_type == 1 && ! hasLeft))
                ++type_matched_offset.x;
            else
                --type_matched_offset.x;
        }
    }
    else if (board_type == BoardType::nexos)
    {
        // Offset must be a junction
        if (old_point_type == 1) // horiz. segment
            --type_matched_offset.x;
        else if (old_point_type == 2) // vert. segment
            --type_matched_offset.y;
        else if (old_point_type == 3) // hole
        {
            --type_matched_offset.x;
            --type_matched_offset.y;
        }
    }
    m_selectedPieceOffset = type_matched_offset;
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
    setSelectedPieceOffset(m_selectedPieceOffset);
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
