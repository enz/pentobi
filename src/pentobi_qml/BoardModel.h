//-----------------------------------------------------------------------------
/** @file pentobi_qml/BoardModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_BOARD_MODEL_H
#define PENTOBI_QML_BOARD_MODEL_H

#include <QQmlListProperty>
#include "PieceModel.h"
#include "libpentobi_base/Board.h"

using libpentobi_base::Board;
using libpentobi_base::Move;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class BoardModel
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString gameVariant MEMBER m_gameVariant
               NOTIFY gameVariantChanged)
    Q_PROPERTY(int nuColors MEMBER m_nuColors NOTIFY nuColorsChanged)
    Q_PROPERTY(int nuPieces MEMBER m_nuPieces NOTIFY nuPiecesChanged)
    Q_PROPERTY(int toPlay MEMBER m_toPlay NOTIFY toPlayChanged)
    Q_PROPERTY(int points0 MEMBER m_points0 NOTIFY points0Changed)
    Q_PROPERTY(int points1 MEMBER m_points1 NOTIFY points1Changed)
    Q_PROPERTY(int points2 MEMBER m_points2 NOTIFY points2Changed)
    Q_PROPERTY(int points3 MEMBER m_points3 NOTIFY points3Changed)
    Q_PROPERTY(int nuPiecesLeft0 MEMBER m_nuPiecesLeft0 NOTIFY nuPiecesLeft0Changed)
    Q_PROPERTY(int nuPiecesLeft1 MEMBER m_nuPiecesLeft1 NOTIFY nuPiecesLeft1Changed)
    Q_PROPERTY(int nuPiecesLeft2 MEMBER m_nuPiecesLeft2 NOTIFY nuPiecesLeft2Changed)
    Q_PROPERTY(int nuPiecesLeft3 MEMBER m_nuPiecesLeft3 NOTIFY nuPiecesLeft3Changed)
    Q_PROPERTY(bool hasMoves0 MEMBER m_hasMoves0 NOTIFY hasMoves0Changed)
    Q_PROPERTY(bool hasMoves1 MEMBER m_hasMoves1 NOTIFY hasMoves1Changed)
    Q_PROPERTY(bool hasMoves2 MEMBER m_hasMoves2 NOTIFY hasMoves2Changed)
    Q_PROPERTY(bool hasMoves3 MEMBER m_hasMoves3 NOTIFY hasMoves3Changed)
    Q_PROPERTY(bool isGameOver MEMBER m_isGameOver NOTIFY isGameOverChanged)
    Q_PROPERTY(bool canUndo MEMBER m_canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels0 READ pieceModels0)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels1 READ pieceModels1)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels2 READ pieceModels2)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels3 READ pieceModels3)

public:
    static Variant getInitialGameVariant();

    explicit BoardModel(QObject* parent = nullptr);

    Q_INVOKABLE bool isLegalPos(PieceModel* pieceModel, QPointF coord) const;

    Q_INVOKABLE void play(PieceModel* pieceModel, QPointF coord);

    Q_INVOKABLE void playMove(int move);

    Q_INVOKABLE void newGame();

    Q_INVOKABLE void undo();

    Q_INVOKABLE void initGameVariant(QString gameVariant);

    Q_INVOKABLE void autoSave();

    Q_INVOKABLE bool loadAutoSave();

    Q_INVOKABLE int getLastMoveColor();

    /** Find the piece model for a given move and set its transform and game
        coordinates accordingly but do not set its status to played yet. */
    Q_INVOKABLE PieceModel* preparePiece(int color, int move);

    QQmlListProperty<PieceModel> pieceModels0();

    QQmlListProperty<PieceModel> pieceModels1();

    QQmlListProperty<PieceModel> pieceModels2();

    QQmlListProperty<PieceModel> pieceModels3();

    const Board& getBoard() const;

signals:
    void toPlayChanged(int);

    void points0Changed(int);

    void points1Changed(int);

    void points2Changed(int);

    void points3Changed(int);

    void nuPiecesLeft0Changed(int);

    void nuPiecesLeft1Changed(int);

    void nuPiecesLeft2Changed(int);

    void nuPiecesLeft3Changed(int);

    void hasMoves0Changed(int);

    void hasMoves1Changed(int);

    void hasMoves2Changed(int);

    void hasMoves3Changed(int);

    void isGameOverChanged(bool);

    void canUndoChanged(bool);

    void gameVariantChanged(QString);

    void nuColorsChanged(int);

    void nuPiecesChanged(int);

private:
    Board m_bd;

    QString m_gameVariant;

    int m_nuColors;

    int m_nuPieces;

    int m_toPlay;

    int m_points0;

    int m_points1;

    int m_points2;

    int m_points3;

    int m_nuPiecesLeft0;

    int m_nuPiecesLeft1;

    int m_nuPiecesLeft2;

    int m_nuPiecesLeft3;

    bool m_hasMoves0;

    bool m_hasMoves1;

    bool m_hasMoves2;

    bool m_hasMoves3;

    bool m_isGameOver;

    bool m_canUndo;

    QList<PieceModel*> m_pieceModels0;

    QList<PieceModel*> m_pieceModels1;

    QList<PieceModel*> m_pieceModels2;

    QList<PieceModel*> m_pieceModels3;

    void createPieceModels();

    void createPieceModels(Color c, QList<PieceModel*>& pieceModels);

    bool findMove(const PieceModel& piece, QPointF coord, Move& mv) const;

    QList<PieceModel*>& getPieceModels(Color c);

    void preparePieceGameCoord(PieceModel* pieceModel, Move mv);

    void preparePieceTransform(PieceModel* pieceModel, Move mv);

    void updateProperties();
};

inline const Board& BoardModel::getBoard() const
{
    return m_bd;
}

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_BOARD_MODEL_H
