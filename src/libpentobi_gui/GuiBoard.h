//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoard.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_GUI_BOARD_H
#define LIBPENTOBI_GUI_GUI_BOARD_H

#include <QTimer>
#include <QWidget>
#include "BoardPainter.h"
#include "libboardgame_base/CoordPoint.h"
#include "libpentobi_base/Board.h"

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

    ~GuiBoard();

    void setCoordinates(bool enable);

    const Board& getBoard() const;

    const Grid<QString>& getLabels() const;

    Piece getSelectedPiece() const;

    const Transform* getSelectedPieceTransform() const;

    void setSelectedPieceTransform(const Transform* transform);

    void showMove(Color c, Move mv);

    int heightForWidth(int width) const;

    void copyFromBoard(const Board& bd);

    void setLabel(Point p, const QString& text);

    void clearMarkup();

    void setFreePlacement(bool enable);

public slots:
    void clearSelectedPiece();

    void selectPiece(Color color, Piece piece);

    void moveSelectedPieceLeft();

    void moveSelectedPieceRight();

    void moveSelectedPieceUp();

    void moveSelectedPieceDown();

    void placeSelectedPiece();

signals:
    void play(Color color, Move mv);

    void pointClicked(Point p);

protected:
    void changeEvent(QEvent* event);

    void leaveEvent(QEvent* event);

    void mouseMoveEvent(QMouseEvent* event);

    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

private:
    const Board& m_bd;

    bool m_isInitialized;

    bool m_freePlacement;

    /** Does the empty board need redrawing? */
    bool m_emptyBoardDirty;

    /** Do the pieces and markup on the board need redrawing?
        If true, the cached board pixmap needs to be repainted. This does not
        include the selected piece (the selected piece is always painted). */
    bool m_dirty;

    Variant m_variant;

    Board::PointStateGrid m_pointState;

    Piece m_selectedPiece;

    Color m_selectedPieceColor;

    const Transform* m_selectedPieceTransform;

    CoordPoint m_selectedPieceOffset;

    MovePoints m_selectedPiecePoints;

    Grid<QString> m_labels;

    BoardPainter m_boardPainter;

    QPixmap* m_emptyBoardPixmap;

    QPixmap* m_boardPixmap;

    bool m_isMoveShown;

    Color m_currentMoveShownColor;

    MovePoints m_currentMoveShownPoints;

    int m_currentMoveShownAnimationIndex;

    QTimer m_currentMoveShownAnimationTimer;

    Move findSelectedPieceMove();

    void setEmptyBoardDirty();

    void setDirty();

    void setSelectedPieceOffset(const QMouseEvent& event);

    void setSelectedPieceOffset(const CoordPoint& offset);

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
