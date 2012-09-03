//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoard.h"

#include <algorithm>
#include <limits>
#include <boost/foreach.hpp>
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
using libpentobi_base::PiecePoints;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

bool allPointEmpty(const Board& bd, Move mv)
{
    BOOST_FOREACH(Point p, bd.get_move_info(mv).points)
        if (! bd.is_empty(p))
            return false;
    return true;
}

} // namespace

//-----------------------------------------------------------------------------

GuiBoard::GuiBoard(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd),
      m_isInitialized(false),
      m_freePlacement(false),
      m_emptyBoardDirty(true),
      m_dirty(true),
      m_selectedPiece(Piece::null()),
      m_selectedPieceTransform(0),
      m_emptyBoardPixmap(0),
      m_boardPixmap(0),
      m_isMoveShown(false)
{
    setMinimumWidth(14 * (Point::max_width + 2));
    setMinimumHeight(14 * (Point::max_height + 2));
    connect(&m_currentMoveShownAnimationTimer, SIGNAL(timeout()),
            this, SLOT(showMoveAnimation()));
}

GuiBoard::~GuiBoard()
{
    delete m_emptyBoardPixmap;
    delete m_boardPixmap;
}

void GuiBoard::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::StyleChange)
        setEmptyBoardDirty();
}

void GuiBoard::clearMarkup()
{
    for (Geometry::Iterator i(m_labels.get_geometry()); i; ++i)
        setLabel(*i, "");
}

void GuiBoard::clearSelectedPiece()
{
    m_selectedPiece = Piece::null();
    m_selectedPieceTransform = 0;
    setSelectedPiecePoints();
    setMouseTracking(false);
}

void GuiBoard::copyFromBoard(const Board& bd)
{
    const Geometry& geometry = bd.get_geometry();
    if (! m_isInitialized || m_variant != bd.get_variant())
    {
        m_variant = bd.get_variant();
        m_isInitialized = true;
        m_pointState = bd.get_grid();
        m_labels.init(geometry, "");
        setEmptyBoardDirty();
    }
    else
    {
        for (BoardIterator i(bd); i; ++i)
            if (m_pointState[*i] != bd.get_point_state(*i))
                m_pointState[*i] = bd.get_point_state(*i);
        setDirty();
    }
}

Move GuiBoard::findSelectedPieceMove()
{
    if (m_selectedPiece.is_null() || m_selectedPieceOffset.is_null())
        return Move::null();
    const PiecePoints& points =
        m_bd.get_piece_info(m_selectedPiece).get_points();
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
    if (! m_bd.find_move(movePoints, mv)
        || (m_freePlacement && ! allPointEmpty(m_bd, mv))
        || (! m_freePlacement && ! m_bd.is_legal(m_selectedPieceColor, mv)))
        return Move::null();
    else
        return mv;
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
        if (m_bd.get_geometry().is_onboard(p))
            emit pointClicked(Point(p.x, p.y));
        return;
    }
    setSelectedPieceOffset(*event);
    placeSelectedPiece();
}

void GuiBoard::moveSelectedPieceDown()
{
    if (m_selectedPiece.is_null())
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset =
            CoordPoint(geometry.get_width() / 2, geometry.get_height() - 1);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
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
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceLeft()
{
    if (m_selectedPiece.is_null())
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset =
            CoordPoint(geometry.get_width() - 1, geometry.get_height() / 2);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
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
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceRight()
{
    if (m_selectedPiece.is_null())
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(0, geometry.get_height() / 2);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
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
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::moveSelectedPieceUp()
{
    if (m_selectedPiece.is_null())
        return;
    const Geometry& geometry = m_bd.get_geometry();
    CoordPoint newOffset;
    if (m_selectedPieceOffset.is_null())
    {
        newOffset = CoordPoint(geometry.get_width() / 2, 0);
        setSelectedPieceOffset(newOffset);
        setSelectedPiecePoints();
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
            setSelectedPiecePoints();
        }
    }
}

void GuiBoard::paintEvent(QPaintEvent*)
{
    if (! m_isInitialized)
        return;
    if (m_emptyBoardPixmap == 0 || m_emptyBoardPixmap->size() != size())
    {
        delete m_emptyBoardPixmap;
        m_emptyBoardPixmap = new QPixmap(size());
        m_emptyBoardDirty = true;
    }
    if (m_boardPixmap == 0 || m_boardPixmap->size() != size())
    {
        delete m_boardPixmap;
        m_boardPixmap = new QPixmap(size());
        m_dirty = true;
    }
    if (m_emptyBoardDirty)
    {
        QColor coordLabelColor =
            QApplication::palette().color(QPalette::WindowText);
        m_boardPainter.setCoordinateColor(coordLabelColor);
        // Why does QPixmap::fill(QWidget) not work on KDE? It does not fill the
        // pixmap with the backgound gradient but with a solid color (tested
        // with Qt 4.7). As a workaround, we use transparency.
        //m_emptyBoardPixmap->fill(this, 0, 0);
        m_emptyBoardPixmap->fill(Qt::transparent);
        QPainter painter(m_emptyBoardPixmap);
        m_boardPainter.paintEmptyBoard(painter, width(), height(),
                                       m_variant,
                                       m_pointState.get_geometry());
        m_emptyBoardDirty = false;
    }
    if (m_dirty)
    {
        m_boardPixmap->fill(Qt::transparent);
        QPainter painter(m_boardPixmap);
        painter.drawPixmap(0, 0, *m_emptyBoardPixmap);
        m_boardPainter.paintPieces(painter, m_pointState, &m_labels);
        m_dirty = false;
    }
    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_boardPixmap);
    if (m_isMoveShown)
    {
        if (m_currentMoveShownAnimationIndex % 2 == 0)
            m_boardPainter.paintSelectedPiece(painter, m_currentMoveShownColor,
                                              m_currentMoveShownPoints, true);
    }
    else
    {
        if (! m_selectedPiece.is_null() && ! m_selectedPieceOffset.is_null())
        {
            bool isLegal = ! findSelectedPieceMove().is_null();
            m_boardPainter.paintSelectedPiece(painter, m_selectedPieceColor,
                                              m_selectedPiecePoints, isLegal);
        }
    }
}

void GuiBoard::placeSelectedPiece()
{
    Move mv = findSelectedPieceMove();
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
    m_selectedPieceOffset = type_matching_offset;
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

void GuiBoard::setSelectedPiecePoints()
{
    m_selectedPiecePoints.clear();
    if (! m_selectedPiece.is_null() && ! m_selectedPieceOffset.is_null())
    {
        int width = static_cast<int>(m_bd.get_geometry().get_width());
        int height = static_cast<int>(m_bd.get_geometry().get_height());
        BOOST_FOREACH(CoordPoint p,
                      m_bd.get_piece_info(m_selectedPiece).get_points())
        {
            p = m_selectedPieceTransform->get_transformed(p);
            int x = p.x + m_selectedPieceOffset.x;
            int y = p.y + m_selectedPieceOffset.y;
            if (x >= 0 && x < width && y >= 0 && y < height)
                m_selectedPiecePoints.push_back(Point(x, y));
        }
    }
    update();
}

//-----------------------------------------------------------------------------
