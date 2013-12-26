//-----------------------------------------------------------------------------
/** @file libpentobi_gui/PieceSelector.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_PIECE_SELECTOR_H
#define LIBPENTOBI_GUI_PIECE_SELECTOR_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QWidget>
#include "libpentobi_base/Board.h"
#include "libpentobi_base/Color.h"

using libboardgame_base::Transform;
using libboardgame_util::ArrayList;
using libpentobi_base::Color;
using libpentobi_base::Board;
using libpentobi_base::Piece;
using libpentobi_base::PiecePoints;

//-----------------------------------------------------------------------------

class PieceSelector
    : public QWidget
{
    Q_OBJECT

public:
    PieceSelector(QWidget* parent, const Board& bd, Color color);

    /** Needs to be called after the game variant of the current board has
        changed because references to pieces are only unique within a
        game variant. */
    void init();

    int heightForWidth(int width) const;

    /** Call update() if pieces left have changed since last paint. */
    void checkUpdate();

signals:
    void pieceSelected(Color color, Piece piece, const Transform* transform);

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    static const unsigned maxColumns = 47;

    static const unsigned maxRows = 6;

    const Board& m_bd;

    Color m_color;

    unsigned m_nuColumns;

    unsigned m_nuRows;

    Piece m_piece[maxColumns][maxRows];

    const Transform* m_transform[maxColumns][maxRows];

    /** Pieces left at last time the widget was painted. */
    bool m_disabledStatus[maxColumns][maxRows];

    qreal m_fieldWidth;

    qreal m_fieldHeight;

    qreal m_selectorWidth;

    qreal m_selectorHeight;

    void findPiecePoints(Piece piece, unsigned x, unsigned y,
                         PiecePoints& points) const;

    void setDisabledStatus(bool disabledStatus[maxColumns][maxRows]);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_PIECE_SELECTOR_H
