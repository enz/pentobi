//-----------------------------------------------------------------------------
/** @file libpentobi_gui/PieceSelector.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceSelector.h"

#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include "libpentobi_gui/Util.h"

using namespace std;
using boost::trim;
using libboardgame_base::CoordPoint;
using libpentobi_base::BoardConst;
using libpentobi_base::BoardType;
using libpentobi_base::GameVariant;
using libpentobi_base::Geometry;
using libpentobi_base::board_type_trigon;

//-----------------------------------------------------------------------------

namespace {

const string pieceLayoutClassic =
    " 1 .V3V3 .L4L4L4 .T4 . . .Z4Z4 .L5L5L5L5 .V5V5V5 . . . . N N . . . ."
    " . . .V3 . . .L4 .T4T4 .Z4Z4 . .L5 . . . .V5 . . .T5 . . . N N N .I5"
    " 2 2 . . .I4 . . .T4 . . . . . . . . X . .V5 .T5T5T5 . . . . . . .I5"
    " . . .I3 .I4 . P . . . W W . F . . X X X . . . . .T5 .Z5Z5 . U U .I5"
    " O O .I3 .I4 . P P . W W . . F F . . X . . Y . . . . .Z5 . . . U .I5"
    " O O .I3 .I4 . P P . W . . F F . . . . . Y Y Y Y . .Z5Z5 . . U U .I5";

const string pieceLayoutTrigon =
    " F F F . . V V V V . . . . . S S S . . . . X X X . . .A6A6 . . G G G G . . Y Y . . . .I4I4I4I4"
    " . F F F . . . . V V . . . . . . S S S . . X X X . .A6A6A6A6 . . . G G . . Y Y Y Y . . . . . ."
    " . . . . . .L6 . . . . . O O O . . . . . . . . . . . . . . . . . . . . . . . . . . . . .I3I3I3"
    " . .L6L6L6L6L6 . . . . . O O O . . . .P5P5P5P5 . . . . W . . . .C4C4 . .I5I5I5I5I5 . . . . . ."
    "L5 . . . . . . .C5 .C5 . . . . .P6 . . . .P5 . .A4 . . W W W . .C4C4 . . . . . . . . . . . 2 2"
    "L5L5L5L5 . . . .C5C5C5 . . . .P6P6P6P6P6 . . .A4A4A4 . . W W . . . . .I6I6I6I6I6I6 . . 1 . . .";
}

//-----------------------------------------------------------------------------

PieceSelector::PieceSelector(QWidget* parent, const Board& bd, Color color)
    : QWidget(parent),
      m_bd(bd),
      m_color(color)
{
    setMinimumWidth(170);
    setMinimumHeight(30);
    init();
}

void PieceSelector::findPiecePoints(const Piece& piece,
                                    unsigned int x, unsigned int y,
                                    Piece::Points& points) const
{
    CoordPoint p(x, m_nuRows - y - 1);
    if (x < 0 || x >= m_nuColumns || y < 0 || y >= m_nuRows
        || m_piece[x][y] != &piece ||  points.contains(p))
        return;
    points.push_back(p);
    // This assumes that no Trigon pieces touch at the corners, otherwise
    // we would need to iterate over neighboring CoordPoint's like AdjIterator
    // iterates over neighboring Point's
    findPiecePoints(piece, x + 1, y, points);
    findPiecePoints(piece, x - 1, y, points);
    findPiecePoints(piece, x, y + 1, points);
    findPiecePoints(piece, x, y - 1, points);
}

bool PieceSelector::hasHeightForWidth() const
{
    return true;
}

int PieceSelector::heightForWidth(int width) const
{
    // Use ratio for layout of classic pieces, which has larger relative width
    // because the limiting factor in the right panel of the main window is the
    // width
    return width / 34 * 6;
}

void PieceSelector::init()
{
    BoardType board_type = m_bd.get_board_type();
    const string* pieceLayout;
    if (board_type == board_type_trigon)
    {
        pieceLayout = &pieceLayoutTrigon;
        m_nuColumns = 47;
        m_nuRows = 6;
    }
    else
    {
        pieceLayout = &pieceLayoutClassic;
        m_nuColumns = 34;
        m_nuRows = 6;
    }
    for (unsigned int y = 0; y < m_nuRows; ++y)
        for (unsigned int x = 0; x < m_nuColumns; ++x)
        {
            string name = pieceLayout->substr(y * m_nuColumns * 2 + x * 2, 2);
            trim(name);
            const Piece* piece = 0;
            if (name != ".")
            {
                for (unsigned int i = 0; i < m_bd.get_nu_pieces(); ++i)
                {
                    if (m_bd.get_piece(i).get_name() == name)
                    {
                        piece = &m_bd.get_piece(i);
                        break;
                    }
                }
                LIBBOARDGAME_ASSERT(piece != 0);
            }
            m_piece[x][y] = piece;
        }
    for (unsigned int y = 0; y < m_nuRows; ++y)
        for (unsigned int x = 0; x < m_nuColumns; ++x)
        {
            const Piece* piece = m_piece[x][y];
            if (piece == 0)
                continue;
            Piece::Points points;
            findPiecePoints(*piece, x, y, points);
            m_transform[x][y] =
                piece->find_transform(m_bd.get_geometry(), points);
            LIBBOARDGAME_ASSERT(m_transform[x][y] != 0);
        }
}

void PieceSelector::mousePressEvent(QMouseEvent* event)
{
    qreal pixelX = event->x() - 0.5 * (width() - m_selectorWidth);
    qreal pixelY = event->y() - 0.5 * (height() - m_selectorHeight);
    if (pixelX < 0 || pixelX >= m_selectorWidth
        || pixelY < 0 || pixelY >= m_selectorHeight)
        return;
    int x = pixelX / m_fieldWidth;
    int y = pixelY / m_fieldHeight;
    const Piece* piece = m_piece[x][y];
    if (piece == 0 || ! m_bd.is_piece_left(m_color, *piece))
        return;
    update();
    emit pieceSelected(m_color, *piece, m_transform[x][y]);
}

void PieceSelector::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHint(QPainter::Antialiasing, true);
    BoardType boardType = m_bd.get_board_type();
    if (boardType == board_type_trigon)
    {
        qreal ratio = 1.732;
        m_fieldWidth = min(qreal(width()) / (m_nuColumns + 1),
                           qreal(height()) / (ratio * m_nuRows));
        m_fieldHeight = ratio * m_fieldWidth;
    }
    else
    {
        m_fieldWidth = min(qreal(width()) / m_nuColumns,
                           qreal(height()) / m_nuRows);
        m_fieldHeight = m_fieldWidth;
    }
    m_selectorWidth = m_fieldWidth * m_nuColumns;
    m_selectorHeight = m_fieldHeight * m_nuRows;
    painter.save();
    painter.translate(0.5 * (width() - m_selectorWidth),
                      0.5 * (height() - m_selectorHeight));
    GameVariant gameVariant = m_bd.get_game_variant();
    const Geometry& geometry = m_bd.get_geometry();
    for (unsigned int x = 0; x < m_nuColumns; ++x)
        for (unsigned int y = 0; y < m_nuRows; ++y)
        {
            const Piece* piece = m_piece[x][y];
            if (piece != 0 && m_bd.is_piece_left(m_color, *piece))
            {
                if (boardType == board_type_trigon)
                {
                    bool isUpside =
                        (geometry.get_point_type(x, m_nuRows - y - 1) == 1);
                    Util::paintColorTriangle(painter, gameVariant, m_color,
                                             isUpside, x * m_fieldWidth,
                                             y * m_fieldHeight, m_fieldWidth,
                                             m_fieldHeight);
                }
                else
                    Util::paintColorSquare(painter, gameVariant, m_color,
                                           x * m_fieldWidth, y * m_fieldHeight,
                                           m_fieldWidth);
            }
        }
    painter.restore();
}

//-----------------------------------------------------------------------------
