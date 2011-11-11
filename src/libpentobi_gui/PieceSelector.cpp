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

//-----------------------------------------------------------------------------

namespace {

const string pieceLayout =
    " 1 .V3V3 .L4L4L4 .T4 . . .Z4Z4 .L5L5L5L5 .V5V5V5 . . . . N N . . . ."
    " . . .V3 . . .L4 .T4T4 .Z4Z4 . .L5 . . . .V5 . . .T5 . . . N N N .I5"
    " 2 2 . . .I4 . . .T4 . . . . . . . . X . .V5 .T5T5T5 . . . . . . .I5"
    " . . .I3 .I4 . P . . . W W . F . . X X X . . . . .T5 .Z5Z5 . U U .I5"
    " O O .I3 .I4 . P P . W W . . F F . . X . . Y . . . . .Z5 . . . U .I5"
    " O O .I3 .I4 . P P . W . . F F . . . . . Y Y Y Y . .Z5Z5 . . U U .I5";

}

//-----------------------------------------------------------------------------

PieceSelector::PieceSelector(QWidget* parent, const Board& bd, Color color)
    : QWidget(parent),
      m_bd(bd),
      m_color(color)
{
    setMinimumHeight(5 * nuRows);
    setMinimumWidth(5 * nuColumns);
    init();
}

void PieceSelector::findPiecePoints(const Piece& piece,
                                    unsigned int x, unsigned int y,
                                    Piece::Points& points) const
{
    CoordPoint p(x, nuRows - y - 1);
    if (x < 0 || x >= nuColumns || y < 0 || y >= nuRows
        || m_piece[x][y] != &piece ||  points.contains(p))
        return;
    points.push_back(p);
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
    return width / nuColumns * nuRows;
}

void PieceSelector::init()
{
    for (unsigned int y = 0; y < nuRows; ++y)
        for (unsigned int x = 0; x < nuColumns; ++x)
        {
            string name = pieceLayout.substr(y * nuColumns * 2 + x * 2, 2);
            trim(name);
            const Piece* piece = 0;
            if (name != ".")
                for (unsigned int i = 0; i < m_bd.get_nu_pieces(); ++i)
                {
                    if (m_bd.get_piece(i).get_name() == name)
                    {
                        piece = &m_bd.get_piece(i);
                        break;
                    }
                }
            m_piece[x][y] = piece;
        }
    for (unsigned int y = 0; y < nuRows; ++y)
        for (unsigned int x = 0; x < nuColumns; ++x)
        {
            const Piece* piece = m_piece[x][y];
            if (piece == 0)
                continue;
            Piece::Points points;
            findPiecePoints(*piece, x, y, points);
            m_transform[x][y] = piece->find_transform(points);
            LIBBOARDGAME_ASSERT(m_transform[x][y] != 0);
        }
}

void PieceSelector::mousePressEvent(QMouseEvent* event)
{
    int squareSize = min(width() / nuColumns, height() / nuRows);
    int selectorWidth = squareSize * nuColumns;
    int selectorHeight = squareSize * nuRows;
    int pixelX = event->x() - (width() - selectorWidth) / 2;
    int pixelY = event->y() - (height() - selectorHeight) / 2;
    if (pixelX < 0 || pixelX >= selectorWidth
        || pixelY < 0 || pixelY >= selectorHeight)
        return;
    int x = pixelX / squareSize;
    int y = pixelY / squareSize;
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
    int squareSize = min(width() / nuColumns, height() / nuRows);
    int selectorWidth = squareSize * nuColumns;
    int selectorHeight = squareSize * nuRows;
    painter.save();
    painter.translate((width() - selectorWidth) / 2,
                      (height() - selectorHeight) / 2);
    for (unsigned int x = 0; x < nuColumns; ++x)
        for (unsigned int y = 0; y < nuRows; ++y)
        {
            const Piece* piece = m_piece[x][y];
            if (piece != 0 && m_bd.is_piece_left(m_color, *piece))
                Util::paintColorSquare(painter, m_bd.get_game_variant(),
                                       m_color, x * squareSize, y * squareSize,
                                       squareSize);
        }
    painter.restore();
}

//-----------------------------------------------------------------------------
