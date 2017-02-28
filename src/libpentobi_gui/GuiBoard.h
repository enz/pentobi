//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_GUI_BOARD_H
#define LIBPENTOBI_GUI_GUI_BOARD_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>
#include <QTimer>
#include <QWidget>
#include "BoardPainter.h"
#include "libboardgame_base/CoordPoint.h"
#include "libpentobi_base/Board.h"

using namespace std;
using libpentobi_base::Color;
using libboardgame_base::CoordPoint;
using libpentobi_base::Board;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;

//-----------------------------------------------------------------------------

class GuiBoard
    : public QWidget
{
    Q_OBJECT

public:
    GuiBoard(QWidget* parent, const Board& bd);

    void setCoordinates(bool enable);

    const Board& getBoard() const;

    const Grid<QString>& getLabels() const;

    Piece getSelectedPiece() const;

    const Transform* getSelectedPieceTransform() const;

    void setSelectedPieceTransform(const Transform* transform);

    void showMove(Color c, Move mv);

    void copyFromBoard(const Board& bd);

    void setLabel(Point p, const QString& text);

    void setMark(Point p, int mark, bool enable = true);

    void clearMarkup();

    void setFreePlacement(bool enable);

    void setSelectedPiecePoints(Move mv);

public slots:
    void clearPiece();

    void selectPiece(Color color, Piece piece);

    void movePieceLeft();

    void movePieceRight();

    void movePieceUp();

    void movePieceDown();

    void placePiece();

signals:
    void play(Color color, Move mv);

    void pointClicked(Point p);

protected:
    void changeEvent(QEvent* event) override;

    void leaveEvent(QEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    const Board& m_bd;

    bool m_isInitialized = false;

    bool m_freePlacement = false;

    /** Does the empty board need redrawing? */
    bool m_emptyBoardDirty = true;

    /** Do the pieces and markup on the board need redrawing?
        If true, the cached board pixmap needs to be repainted. This does not
        include the selected piece (the selected piece is always painted). */
    bool m_dirty = true;

    bool m_isMoveShown = false;

    Variant m_variant;

    Board::PointStateGrid m_pointState;

    Grid<unsigned> m_pieceId;

    Piece m_selectedPiece = Piece::null();

    Color m_selectedPieceColor;

    const Transform* m_selectedPieceTransform = nullptr;

    CoordPoint m_selectedPieceOffset;

    MovePoints m_selectedPiecePoints;

    Grid<QString> m_labels;

    Grid<int> m_marks;

    BoardPainter m_boardPainter;

    unique_ptr<QPixmap> m_emptyBoardPixmap;

    unique_ptr<QPixmap> m_boardPixmap;

    Color m_currentMoveShownColor;

    MovePoints m_currentMoveShownPoints;

    int m_currentMoveShownAnimationIndex;

    QTimer m_currentMoveShownAnimationTimer;


    Move findSelectedPieceMove();

    bool isSelectedPiecePartiallyOnBoard(CoordPoint offset) const;

    void setEmptyBoardDirty();

    void setDirty();

    void setSelectedPieceOffset(const QMouseEvent& event);

    void setSelectedPieceOffset(const CoordPoint& offset,
                                bool preferRightShift = false,
                                bool preferDownShift = false);

    void setSelectedPiecePoints();

private slots:
    void showMoveAnimation();
};

inline const Board& GuiBoard::getBoard() const
{
    return m_bd;
}

inline const Grid<QString>& GuiBoard::getLabels() const
{
    return m_labels;
}

inline Piece GuiBoard::getSelectedPiece() const
{
    return m_selectedPiece;
}

inline const Transform* GuiBoard::getSelectedPieceTransform() const
{
    return m_selectedPieceTransform;
}

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_GUI_BOARD_H
