//-----------------------------------------------------------------------------
/** @file pentobi_qml/GameModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_GAME_MODEL_H
#define PENTOBI_QML_GAME_MODEL_H

#include <QQmlListProperty>
#include "PieceModel.h"
#include "libpentobi_base/Game.h"

using libpentobi_base::Board;
using libpentobi_base::Game;
using libpentobi_base::Move;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class GameModel
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString gameVariant MEMBER m_gameVariant
               NOTIFY gameVariantChanged)
    Q_PROPERTY(int nuColors MEMBER m_nuColors NOTIFY nuColorsChanged)
    Q_PROPERTY(int nuPieces MEMBER m_nuPieces NOTIFY nuPiecesChanged)
    Q_PROPERTY(int toPlay MEMBER m_toPlay NOTIFY toPlayChanged)
    Q_PROPERTY(int altPlayer MEMBER m_altPlayer NOTIFY altPlayerChanged)
    Q_PROPERTY(int points0 MEMBER m_points0 NOTIFY points0Changed)
    Q_PROPERTY(int points1 MEMBER m_points1 NOTIFY points1Changed)
    Q_PROPERTY(int points2 MEMBER m_points2 NOTIFY points2Changed)
    Q_PROPERTY(int points3 MEMBER m_points3 NOTIFY points3Changed)
    Q_PROPERTY(bool hasMoves0 MEMBER m_hasMoves0 NOTIFY hasMoves0Changed)
    Q_PROPERTY(bool hasMoves1 MEMBER m_hasMoves1 NOTIFY hasMoves1Changed)
    Q_PROPERTY(bool hasMoves2 MEMBER m_hasMoves2 NOTIFY hasMoves2Changed)
    Q_PROPERTY(bool hasMoves3 MEMBER m_hasMoves3 NOTIFY hasMoves3Changed)
    Q_PROPERTY(bool isGameOver MEMBER m_isGameOver NOTIFY isGameOverChanged)
    Q_PROPERTY(bool isBoardEmpty MEMBER m_isBoardEmpty
               NOTIFY isBoardEmptyChanged)
    Q_PROPERTY(bool canUndo MEMBER m_canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels0 READ pieceModels0)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels1 READ pieceModels1)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels2 READ pieceModels2)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels3 READ pieceModels3)

public:
    static Variant getInitialGameVariant();

    explicit GameModel(QObject* parent = nullptr);

    Q_INVOKABLE bool isLegalPos(PieceModel* pieceModel, QString state,
                                QPointF coord) const;

    Q_INVOKABLE void play(PieceModel* pieceModel, QPointF coord);

    Q_INVOKABLE void playMove(int move);

    Q_INVOKABLE void newGame();

    Q_INVOKABLE void undo();

    Q_INVOKABLE void initGameVariant(QString gameVariant);

    Q_INVOKABLE void autoSave();

    Q_INVOKABLE void clearAutoSave();

    Q_INVOKABLE bool loadAutoSave();

    Q_INVOKABLE int getLastMoveColor();

    Q_INVOKABLE PieceModel* getLastMovePieceModel();

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

    void altPlayerChanged(int);

    void points0Changed(int);

    void points1Changed(int);

    void points2Changed(int);

    void points3Changed(int);

    void hasMoves0Changed(int);

    void hasMoves1Changed(int);

    void hasMoves2Changed(int);

    void hasMoves3Changed(int);

    void isGameOverChanged(bool);

    void isBoardEmptyChanged(bool);

    void canUndoChanged(bool);

    void gameVariantChanged(QString);

    void nuColorsChanged(int);

    void nuPiecesChanged(int);

private:
    Game m_game;

    QString m_gameVariant;

    int m_nuColors;

    int m_nuPieces;

    int m_toPlay = 0;

    int m_altPlayer = 0;

    int m_points0 = 0;

    int m_points1 = 0;

    int m_points2 = 0;

    int m_points3 = 0;

    bool m_hasMoves0 = true;

    bool m_hasMoves1 = true;

    bool m_hasMoves2 = true;

    bool m_hasMoves3 = true;

    bool m_isGameOver = false;

    bool m_isBoardEmpty = true;

    bool m_canUndo = false;

    QList<PieceModel*> m_pieceModels0;

    QList<PieceModel*> m_pieceModels1;

    QList<PieceModel*> m_pieceModels2;

    QList<PieceModel*> m_pieceModels3;

    PieceModel* m_lastMovePieceModel;

    void createPieceModels();

    void createPieceModels(Color c, QList<PieceModel*>& pieceModels);

    bool findMove(const PieceModel& piece, QString state, QPointF coord,
                  Move& mv) const;

    QList<PieceModel*>& getPieceModels(Color c);

    void preparePieceGameCoord(PieceModel* pieceModel, Move mv);

    void preparePieceTransform(PieceModel* pieceModel, Move mv);

    void updateProperties();
};

inline const Board& GameModel::getBoard() const
{
    return m_game.get_board();
}

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_GAME_MODEL_H
