//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoard.h"

#include <algorithm>
#include <limits>
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::Transform;
using libboardgame_util::log;
using libpentobi_base::board_type_trigon;
using libpentobi_base::board_type_trigon_3;
using libpentobi_base::BoardIterator;
using libpentobi_base::Geometry;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

GuiBoard::GuiBoard(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd),
      m_isInitialized(false),
      m_selectedPiece(0),
      m_selectedPieceTransform(0),
      m_isMoveShown(false)
{
    setMinimumWidth(14 * (Point::max_width + 2));
    setMinimumHeight(14 * (Point::max_height + 2));
    connect(&m_currentMoveShownAnimationTimer, SIGNAL(timeout()),
            this, SLOT(showMoveAnimation()));
}

void GuiBoard::clearMarkup()
{
    for (Geometry::Iterator i(m_labels.get_geometry()); i; ++i)
    {
        setLabel(*i, "");
        for (unsigned int j = 0; j < _nu_markup_flags; ++j)
            clearMarkupFlag(*i, static_cast<MarkupFlag>(j));
    }
}

void GuiBoard::clearMarkupFlag(Point p, MarkupFlag flag)
{
    if (! m_markupFlags[p].test(flag))
        return;
    m_markupFlags[p].reset(flag);
    updatePoint(p);
}

void GuiBoard::clearSelectedPiece()
{
    m_selectedPiece = 0;
    m_selectedPieceTransform = 0;
    updateSelectedPiecePoints();
    setMouseTracking(false);
}

void GuiBoard::copyFromBoard(const Board& bd)
{
    m_gameVariant = bd.get_game_variant();
    const Geometry& geometry = bd.get_geometry();
    if (! m_isInitialized
        || geometry.get_width() != m_pointState.get_geometry().get_width())
    {
        m_isInitialized = true;
        m_pointState = bd.get_grid();
        m_labels.init(geometry, "");
        m_markupFlags.init(geometry);
        update();
    }
    else
        for (BoardIterator i(bd); i; ++i)
            if (m_pointState[*i] != bd.get_point_state(*i))
            {
                m_pointState[*i] = bd.get_point_state(*i);
                updatePoint(*i);
            }
}

Move GuiBoard::findSelectedPieceMove()
{
    if (m_selectedPiece == 0 || m_selectedPieceOffset.is_null())
        return Move::null();
    const Piece::Points& points = m_selectedPiece->get_points();
    MovePoints movePoints;
    int width = static_cast<int>(m_bd.get_geometry().get_width());
    int height = static_cast<int>(m_bd.get_geometry().get_height());
    BOOST_FOREACH(CoordPoint p, points)
    {
        p = m_selectedPieceTransform->get_transformed(p);
        int x = p.x + m_selectedPieceOffset.x;
        int y = p.y + m_selectedPieceOffset.y;
        if (x < 0 || x >= width || y < 0 || y >= height)
            return Move::null();
        movePoints.push_back(Point(x, y));
    }
    Move mv;
    if (m_bd.find_move(movePoints, mv)
        && m_bd.is_legal(m_selectedPieceColor, mv))
        return mv;
    return Move::null();
}

bool GuiBoard::hasHeightForWidth() const
{
    return true;
}

int GuiBoard::heightForWidth(int width) const
{
    return width;
}

void GuiBoard::leaveEvent(QEvent*)
{
    m_selectedPieceOffset = CoordPoint::null();
    updateSelectedPiecePoints();
}

void GuiBoard::mouseMoveEvent(QMouseEvent* event)
{
    if (m_selectedPiece == 0)
        return;
    CoordPoint oldOffset = m_selectedPieceOffset;
    setSelectedPieceOffset(*event);
    if (m_selectedPieceOffset != oldOffset)
        updateSelectedPiecePoints();
}

void GuiBoard::mousePressEvent(QMouseEvent* event)
{
    if (m_selectedPiece == 0)
        return;
    setSelectedPieceOffset(*event);
    placeSelectedPiece();
}

void GuiBoard::moveSelectedPieceDown()
{
    if (m_selectedPiece == 0)
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset =
            CoordPoint(geometry.get_width() / 2, geometry.get_height() - 1);
        setSelectedPieceOffset(newOffset, false);
        updateSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == board_type_trigon
            || m_bd.get_board_type() == board_type_trigon_3)
        {
            if (m_selectedPieceOffset.x % 2 == 0)
                ++newOffset.x;
            else
                --newOffset.x;
            --newOffset.y;
        }
        else
            --newOffset.y;
        if (geometry.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            updateSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceLeft()
{
    if (m_selectedPiece == 0)
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset =
            CoordPoint(geometry.get_width() - 1, geometry.get_height() / 2);
        setSelectedPieceOffset(newOffset, false);
        updateSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == board_type_trigon
            || m_bd.get_board_type() == board_type_trigon_3)
            newOffset.x -= 2;
        else
            --newOffset.x;
        if (geometry.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            updateSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceRight()
{
    if (m_selectedPiece == 0)
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(0, geometry.get_height() / 2);
        setSelectedPieceOffset(newOffset, false);
        updateSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == board_type_trigon
            || m_bd.get_board_type() == board_type_trigon_3)
            newOffset.x += 2;
        else
            ++newOffset.x;
        if (geometry.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            updateSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceUp()
{
    if (m_selectedPiece == 0)
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(geometry.get_width() / 2, 0);
        setSelectedPieceOffset(newOffset, false);
        updateSelectedPiecePoints();
    }
    else
    {
        newOffset = m_selectedPieceOffset;
        if (m_bd.get_board_type() == board_type_trigon
            || m_bd.get_board_type() == board_type_trigon_3)
        {
            if (m_selectedPieceOffset.x % 2 == 0)
                ++newOffset.x;
            else
                --newOffset.x;
            ++newOffset.y;
        }
        else
            ++newOffset.y;
        if (geometry.is_onboard(newOffset))
        {
            setSelectedPieceOffset(newOffset);
            updateSelectedPiecePoints();
        }
    }
}

void GuiBoard::paintEvent(QPaintEvent* event)
{
    if (! m_isInitialized)
        return;
    m_boardPainter.setCoordLabelColor(palette().color(QPalette::WindowText));
    QPainter painter(this);
    painter.setClipRegion(event->region());
    if (m_isMoveShown)
    {
        if (m_currentMoveShownAnimationIndex % 2 == 0)
            m_boardPainter.setSelectedPiece(m_currentMoveShownColor,
                                            m_currentMoveShownPoints, true);
        else
            m_boardPainter.clearSelectedPiece();
    }
    else
    {
        if (m_selectedPiece != 0 && ! m_selectedPieceOffset.is_null())
        {
            bool isLegal = ! findSelectedPieceMove().is_null();
            m_boardPainter.setSelectedPiece(m_selectedPieceColor,
                                            m_selectedPiecePoints, isLegal);
        }
        else
            m_boardPainter.clearSelectedPiece();
    }
    m_boardPainter.paint(painter, width(), height(), m_gameVariant,
                         m_pointState, &m_labels, &m_markupFlags);
}

void GuiBoard::placeSelectedPiece()
{
    Move mv = findSelectedPieceMove();
    if (! mv.is_null())
        emit play(m_selectedPieceColor, mv);
}

void GuiBoard::selectPiece(Color color, const Piece& piece)
{
    if (m_selectedPiece == &piece && m_selectedPieceColor == color)
        return;
    m_selectedPieceColor = color;
    m_selectedPieceTransform = m_bd.get_transforms().get_default();
    if (m_selectedPiece == 0)
        m_selectedPieceOffset = CoordPoint::null();
    m_selectedPiece = &piece;
    setSelectedPieceOffset(m_selectedPieceOffset);
    updateSelectedPiecePoints();
    setMouseTracking(true);
}

void GuiBoard::setDrawCoordLabels(bool enable)
{
    m_boardPainter.setDrawCoordLabels(enable);
    update();
}

void GuiBoard::setLabel(Point p, const QString& text)
{
    if (! m_isInitialized || ! m_labels.get_geometry().is_onboard(p))
    {
        // Call copyFromBoard(), which initialized the current board size,
        // before calling setLabel()
        LIBBOARDGAME_ASSERT(false);
        return;
    }
    if (m_labels[p] != text)
    {
        m_labels[p] = text;
        updatePoint(p);
    }
}

void GuiBoard::setMarkupFlag(Point p, MarkupFlag flag)
{
    if (m_markupFlags[p].test(flag))
        return;
    m_markupFlags[p].set(flag);
    updatePoint(p);
}

void GuiBoard::setSelectedPieceOffset(const QMouseEvent& event)
{
    setSelectedPieceOffset(m_boardPainter.getCoordPoint(event.x(), event.y()));
}

void GuiBoard::setSelectedPieceOffset(const CoordPoint& offset,
                                      bool requireOnboard)
{
    if (offset.is_null())
    {
        m_selectedPieceOffset = offset;
        return;
    }
    const Geometry& geometry = m_bd.get_geometry();
    unsigned int old_point_type = geometry.get_point_type(offset);
    unsigned int point_type = m_selectedPieceTransform->get_new_point_type();
    CoordPoint type_matching_offset = offset;
    if (old_point_type != point_type)
    {
        if ((point_type == 0
             && geometry.is_onboard(CoordPoint(offset.x + 1, offset.y)))
            || (point_type == 1
                && ! geometry.is_onboard(CoordPoint(offset.x - 1, offset.y))))
            ++type_matching_offset.x;
        else
            --type_matching_offset.x;
    }
    if (requireOnboard)
    {
        BOOST_FOREACH(const CoordPoint& piecePoint,
                      m_selectedPiece->get_points())
        {
            CoordPoint p =
                m_selectedPieceTransform->get_transformed(piecePoint);
            p = p + type_matching_offset;
            if (! geometry.is_onboard(p))
                return;
        }
    }
    m_selectedPieceOffset = type_matching_offset;
}

void GuiBoard::setSelectedPieceTransform(const Transform* transform)
{
    if (m_selectedPieceTransform == transform)
        return;
    m_selectedPieceTransform = transform;
    setSelectedPieceOffset(m_selectedPieceOffset, false);
    updateSelectedPiecePoints();
}

void GuiBoard::showMove(Color c, Move mv)
{
    m_isMoveShown = true;
    m_currentMoveShownColor = c;
    m_currentMoveShownPoints = m_bd.get_move_points(mv);
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

void GuiBoard::updatePoint(Point p)
{
    update(m_boardPainter.getRect(p, m_gameVariant));
}

void GuiBoard::updateSelectedPiecePoints()
{
    MovePoints points;
    if (m_selectedPiece != 0 && m_selectedPieceOffset != CoordPoint::null())
    {
        int width = static_cast<int>(m_bd.get_geometry().get_width());
        int height = static_cast<int>(m_bd.get_geometry().get_height());
        BOOST_FOREACH(CoordPoint p, m_selectedPiece->get_points())
        {
            p = m_selectedPieceTransform->get_transformed(p);
            int x = p.x + m_selectedPieceOffset.x;
            int y = p.y + m_selectedPieceOffset.y;
            if (x >= 0 && x < width && y >= 0 && y < height)
                points.push_back(Point(x, y));
        }
    }
    if (points != m_selectedPiecePoints)
    {
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
            updatePoint(p);
        m_selectedPiecePoints = points;
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
            updatePoint(p);
    }
}

//-----------------------------------------------------------------------------
