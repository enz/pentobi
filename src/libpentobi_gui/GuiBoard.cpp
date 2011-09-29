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
      m_isMoveShown(false)
{
    setMinimumWidth(14 * (Point::max_size + 2));
    setMinimumHeight(14 * (Point::max_size + 2));
    connect(&m_currentMoveShownAnimationTimer, SIGNAL(timeout()),
            this, SLOT(showMoveAnimation()));
}

void GuiBoard::clearMarkup()
{
    for (Geometry::Iterator i(m_labels.get_geometry()); i; ++i)
    {
        setLabel(*i, "");
        m_markupFlags[*i].reset();
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
    updateSelectedPiecePoints();
    setMouseTracking(false);
}

void GuiBoard::copyFromBoard(const Board& bd)
{
    m_gameVariant = bd.get_game_variant();
    if (! m_isInitialized
        || m_bd.get_size() != m_pointState.get_geometry().get_size())
    {
        m_isInitialized = true;
        m_pointState = bd.get_grid();
        const Geometry& geometry = bd.get_geometry();
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
    unsigned int sz = m_bd.get_size();
    if (m_selectedPieceOffset.is_null())
        m_selectedPieceOffset = CoordPoint(sz / 2, sz - 1);
    else if (m_selectedPieceOffset.y > 0)
        --m_selectedPieceOffset.y;
    setSelectedPieceOffset(m_selectedPieceOffset);
    updateSelectedPiecePoints();
}

void GuiBoard::moveSelectedPieceLeft()
{
    if (m_selectedPiece == 0)
        return;
    unsigned int sz = m_bd.get_size();
    if (m_selectedPieceOffset.is_null())
        m_selectedPieceOffset = CoordPoint(sz - 1, sz / 2);
    else if (m_selectedPieceOffset.x > 0)
        --m_selectedPieceOffset.x;
    setSelectedPieceOffset(m_selectedPieceOffset);
    updateSelectedPiecePoints();
}

void GuiBoard::moveSelectedPieceRight()
{
    if (m_selectedPiece == 0)
        return;
    int sz = m_bd.get_size();
    if (m_selectedPieceOffset.is_null())
        m_selectedPieceOffset = CoordPoint(0, sz / 2);
    else if (m_selectedPieceOffset.x < sz - 1)
        ++m_selectedPieceOffset.x;
    setSelectedPieceOffset(m_selectedPieceOffset);
    updateSelectedPiecePoints();
}

void GuiBoard::moveSelectedPieceUp()
{
    if (m_selectedPiece == 0)
        return;
    int sz = m_bd.get_size();
    if (m_selectedPieceOffset.is_null())
        m_selectedPieceOffset = CoordPoint(sz / 2, 0);
    else if (m_selectedPieceOffset.y < sz - 1)
        ++m_selectedPieceOffset.y;
    setSelectedPieceOffset(m_selectedPieceOffset);
    updateSelectedPiecePoints();
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
                                            m_currentMoveShownPoints);
        else
            m_boardPainter.clearSelectedPiece();
    }
    else
    {
        if (m_selectedPiece != 0 && ! m_selectedPieceOffset.is_null())
            m_boardPainter.setSelectedPiece(m_selectedPieceColor,
                                            m_selectedPiecePoints);
        else
            m_boardPainter.clearSelectedPiece();
    }
    m_boardPainter.paint(painter, width(), height(), m_gameVariant,
                         m_pointState, &m_labels, &m_markupFlags);
}

void GuiBoard::placeSelectedPiece()
{
    if (m_selectedPiece == 0 || m_selectedPieceOffset.is_null())
        return;
    const Piece::Points& points = m_selectedPiece->get_points();
    MovePoints movePoints;
    int sz = static_cast<int>(m_bd.get_size());
    BOOST_FOREACH(CoordPoint p, points)
    {
        p = m_selectedPieceTransform.get_transformed(p);
        int x = p.x + m_selectedPieceOffset.x;
        int y = p.y + m_selectedPieceOffset.y;
        if (x < 0 || x >= sz || y < 0 || y >= sz)
            return;
        movePoints.push_back(Point(x, y));
    }
    Move mv;
    if (m_bd.find_move(movePoints, mv)
        && m_bd.is_legal(m_selectedPieceColor, mv))
        emit play(m_selectedPieceColor, mv);
}

void GuiBoard::selectPiece(Color color, const Piece& piece)
{
    if (m_selectedPiece == &piece && m_selectedPieceColor == color)
        return;
    m_selectedPieceColor = color;
    m_selectedPieceTransform = Transform();
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
    if (! m_isInitialized || ! p.is_onboard(m_labels.get_geometry().get_size()))
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

void GuiBoard::setSelectedPieceOffset(const CoordPoint& offset)
{
    m_selectedPieceOffset = offset;
    if (m_selectedPieceOffset.is_null())
        return;
    int minX = numeric_limits<int>::max();
    int maxX = numeric_limits<int>::min();
    int minY = numeric_limits<int>::max();
    int maxY = numeric_limits<int>::min();
    BOOST_FOREACH(const CoordPoint& piecePoint, m_selectedPiece->get_points())
    {
        CoordPoint p = m_selectedPieceTransform.get_transformed(piecePoint);
        p = p + m_selectedPieceOffset;
        if (p.x < minX)
            minX = p.x;
        if (p.x > maxX)
            maxX = p.x;
        if (p.y < minY)
            minY = p.y;
        if (p.y > maxY)
            maxY = p.y;
    }
    int sz = static_cast<int>(m_bd.get_size());
    if (minX < 0)
        m_selectedPieceOffset.x -= minX;
    if (maxX >=  sz)
        m_selectedPieceOffset.x -= (maxX - sz + 1);
    if (minY < 0)
        m_selectedPieceOffset.y -= minY;
    if (maxY >=  sz)
        m_selectedPieceOffset.y -= (maxY - sz + 1);
}

void GuiBoard::setSelectedPieceTransform(Transform transform)
{
    if (m_selectedPieceTransform == transform)
        return;
    m_selectedPieceTransform = transform;
    setSelectedPieceOffset(m_selectedPieceOffset);
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
    update(m_boardPainter.getRect(p));
}

void GuiBoard::updateSelectedPiecePoints()
{
    MovePoints points;
    if (m_selectedPiece != 0 && m_selectedPieceOffset != CoordPoint::null())
    {
        int sz = m_bd.get_size();
        BOOST_FOREACH(CoordPoint p, m_selectedPiece->get_points())
        {
            p = m_selectedPieceTransform.get_transformed(p);
            int x = p.x + m_selectedPieceOffset.x;
            int y = p.y + m_selectedPieceOffset.y;
            if (x >= 0 && x < sz && y >= 0 && y < sz)
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
