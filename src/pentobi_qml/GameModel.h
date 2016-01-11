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

using namespace std;
using libboardgame_sgf::SgfNode;
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
    Q_PROPERTY(QString positionInfo MEMBER m_positionInfo
               NOTIFY positionInfoChanged)
    Q_PROPERTY(QString lastInputOutputError MEMBER m_lastInputOutputError)
    Q_PROPERTY(int nuColors MEMBER m_nuColors NOTIFY nuColorsChanged)
    Q_PROPERTY(int toPlay MEMBER m_toPlay NOTIFY toPlayChanged)
    Q_PROPERTY(int altPlayer MEMBER m_altPlayer NOTIFY altPlayerChanged)
    Q_PROPERTY(float points0 MEMBER m_points0 NOTIFY points0Changed)
    Q_PROPERTY(float points1 MEMBER m_points1 NOTIFY points1Changed)
    Q_PROPERTY(float points2 MEMBER m_points2 NOTIFY points2Changed)
    Q_PROPERTY(float points3 MEMBER m_points3 NOTIFY points3Changed)
    Q_PROPERTY(float bonus0 MEMBER m_bonus0 NOTIFY bonus0Changed)
    Q_PROPERTY(float bonus1 MEMBER m_bonus1 NOTIFY bonus1Changed)
    Q_PROPERTY(float bonus2 MEMBER m_bonus2 NOTIFY bonus2Changed)
    Q_PROPERTY(float bonus3 MEMBER m_bonus3 NOTIFY bonus3Changed)
    Q_PROPERTY(bool hasMoves0 MEMBER m_hasMoves0 NOTIFY hasMoves0Changed)
    Q_PROPERTY(bool hasMoves1 MEMBER m_hasMoves1 NOTIFY hasMoves1Changed)
    Q_PROPERTY(bool hasMoves2 MEMBER m_hasMoves2 NOTIFY hasMoves2Changed)
    Q_PROPERTY(bool hasMoves3 MEMBER m_hasMoves3 NOTIFY hasMoves3Changed)
    Q_PROPERTY(bool isGameOver MEMBER m_isGameOver NOTIFY isGameOverChanged)
    Q_PROPERTY(bool isGameEmpty MEMBER m_isGameEmpty NOTIFY isGameEmptyChanged)
    Q_PROPERTY(bool canUndo MEMBER m_canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canGoBackward MEMBER m_canGoBackward
               NOTIFY canGoBackwardChanged)
    Q_PROPERTY(bool canGoForward MEMBER m_canGoForward
               NOTIFY canGoForwardChanged)
    Q_PROPERTY(bool hasPrevVar MEMBER m_hasPrevVar NOTIFY hasPrevVarChanged)
    Q_PROPERTY(bool hasNextVar MEMBER m_hasNextVar NOTIFY hasNextVarChanged)
    Q_PROPERTY(bool isMainVar MEMBER m_isMainVar NOTIFY isMainVarChanged)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels0 READ pieceModels0)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels1 READ pieceModels1)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels2 READ pieceModels2)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels3 READ pieceModels3)

public:
    static Variant getInitialGameVariant();

    explicit GameModel(QObject* parent = nullptr);

    Q_INVOKABLE bool isLegalPos(PieceModel* pieceModel, QString state,
                                QPointF coord) const;

    Q_INVOKABLE void playPiece(PieceModel* pieceModel, QPointF coord);

    Q_INVOKABLE void playMove(int move);

    Q_INVOKABLE void newGame();

    Q_INVOKABLE void undo();

    Q_INVOKABLE void goBeginning();

    Q_INVOKABLE void goBackward();

    Q_INVOKABLE void goForward();

    Q_INVOKABLE void goEnd();

    Q_INVOKABLE void goNextVar();

    Q_INVOKABLE void goPrevVar();

    Q_INVOKABLE void backToMainVar();

    Q_INVOKABLE void initGameVariant(QString gameVariant);

    Q_INVOKABLE void autoSave();

    Q_INVOKABLE bool loadAutoSave();

    Q_INVOKABLE int getLastMoveColor();

    Q_INVOKABLE PieceModel* getLastMovePieceModel();

    /** Find the piece model for a given move and set its transform and game
        coordinates accordingly but do not set its status to played yet. */
    Q_INVOKABLE PieceModel* preparePiece(int color, int move);

    Q_INVOKABLE bool save(QString file);

    Q_INVOKABLE bool open(QString file);

    Q_INVOKABLE void makeMainVar();

    Q_INVOKABLE void moveDownVar();

    Q_INVOKABLE void moveUpVar();

    Q_INVOKABLE void truncate();

    Q_INVOKABLE void truncateChildren();

    QQmlListProperty<PieceModel> pieceModels0();

    QQmlListProperty<PieceModel> pieceModels1();

    QQmlListProperty<PieceModel> pieceModels2();

    QQmlListProperty<PieceModel> pieceModels3();

    const Board& getBoard() const { return m_game.get_board(); }

signals:
    /** Position changed due to new game or navigation or editing of the
        game tree.
        Does not include position changes due to newly played moves, in this
        case movePlayed() is emitted. */
    void positionChanged();

    void movePlayed();

    void toPlayChanged(int);

    void altPlayerChanged(int);

    void points0Changed(float);

    void points1Changed(float);

    void points2Changed(float);

    void points3Changed(float);

    void bonus0Changed(float);

    void bonus1Changed(float);

    void bonus2Changed(float);

    void bonus3Changed(float);

    void hasMoves0Changed(int);

    void hasMoves1Changed(int);

    void hasMoves2Changed(int);

    void hasMoves3Changed(int);

    void isGameOverChanged(bool);

    void isGameEmptyChanged(bool);

    void isMainVarChanged(bool);

    void canUndoChanged(bool);

    void canGoBackwardChanged(bool);

    void canGoForwardChanged(bool);

    void hasPrevVarChanged(bool);

    void hasNextVarChanged(bool);

    void gameVariantChanged(QString);

    void positionInfoChanged(QString);

    void nuColorsChanged(int);

private:
    Game m_game;

    QString m_gameVariant;

    QString m_positionInfo;

    QString m_lastInputOutputError;

    int m_nuColors;

    int m_toPlay = 0;

    int m_altPlayer = 0;

    float m_points0 = 0;

    float m_points1 = 0;

    float m_points2 = 0;

    float m_points3 = 0;

    float m_bonus0 = 0;

    float m_bonus1 = 0;

    float m_bonus2 = 0;

    float m_bonus3 = 0;

    bool m_hasMoves0 = true;

    bool m_hasMoves1 = true;

    bool m_hasMoves2 = true;

    bool m_hasMoves3 = true;

    bool m_isGameOver = false;

    bool m_isGameEmpty = true;

    bool m_canUndo = false;

    bool m_canGoForward = false;

    bool m_canGoBackward = false;

    bool m_hasPrevVar = false;

    bool m_hasNextVar = false;

    bool m_isMainVar = true;

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

    void gotoNode(const SgfNode& node);

    void gotoNode(const SgfNode* node);

    bool open(istream& in);

    void preparePieceGameCoord(PieceModel* pieceModel, Move mv);

    void preparePieceTransform(PieceModel* pieceModel, Move mv);

    PieceModel* updatePiece(Color c, Move mv,
                            array<bool, Board::max_pieces>& isPlayed);

    void updateProperties();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_GAME_MODEL_H
