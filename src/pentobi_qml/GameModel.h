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

class QTextCodec;

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
    Q_PROPERTY(QString gameVariant READ gameVariant NOTIFY gameVariantChanged)
    Q_PROPERTY(QString positionInfo READ positionInfo NOTIFY positionInfoChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
    Q_PROPERTY(QString lastInputOutputError READ lastInputOutputError)
    Q_PROPERTY(int nuColors READ nuColors NOTIFY nuColorsChanged)
    Q_PROPERTY(int toPlay READ toPlay NOTIFY toPlayChanged)
    Q_PROPERTY(int altPlayer READ altPlayer NOTIFY altPlayerChanged)
    Q_PROPERTY(float points0 READ points0 NOTIFY points0Changed)
    Q_PROPERTY(float points1 READ points1 NOTIFY points1Changed)
    Q_PROPERTY(float points2 READ points2 NOTIFY points2Changed)
    Q_PROPERTY(float points3 READ points3 NOTIFY points3Changed)
    Q_PROPERTY(float bonus0 READ bonus0 NOTIFY bonus0Changed)
    Q_PROPERTY(float bonus1 READ bonus1 NOTIFY bonus1Changed)
    Q_PROPERTY(float bonus2 READ bonus2 NOTIFY bonus2Changed)
    Q_PROPERTY(float bonus3 READ bonus3 NOTIFY bonus3Changed)
    Q_PROPERTY(bool hasMoves0 READ hasMoves0 NOTIFY hasMoves0Changed)
    Q_PROPERTY(bool hasMoves1 READ hasMoves1 NOTIFY hasMoves1Changed)
    Q_PROPERTY(bool hasMoves2 READ hasMoves2 NOTIFY hasMoves2Changed)
    Q_PROPERTY(bool hasMoves3 READ hasMoves3 NOTIFY hasMoves3Changed)
    Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY isGameOverChanged)
    Q_PROPERTY(bool isGameEmpty READ isGameEmpty NOTIFY isGameEmptyChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canGoBackward READ canGoBackward NOTIFY canGoBackwardChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY canGoForwardChanged)
    Q_PROPERTY(bool hasPrevVar READ hasPrevVar NOTIFY hasPrevVarChanged)
    Q_PROPERTY(bool hasNextVar READ hasNextVar NOTIFY hasNextVarChanged)
    Q_PROPERTY(bool hasVariations READ hasVariations NOTIFY hasVariationsChanged)
    Q_PROPERTY(bool isMainVar READ isMainVar NOTIFY isMainVarChanged)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels0 READ pieceModels0)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels1 READ pieceModels1)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels2 READ pieceModels2)
    Q_PROPERTY(QQmlListProperty<PieceModel> pieceModels3 READ pieceModels3)
    Q_PROPERTY(QVariantList startingPoints0 READ startingPoints0 NOTIFY startingPoints0Changed)
    Q_PROPERTY(QVariantList startingPoints1 READ startingPoints1 NOTIFY startingPoints1Changed)
    Q_PROPERTY(QVariantList startingPoints2 READ startingPoints2 NOTIFY startingPoints2Changed)
    Q_PROPERTY(QVariantList startingPoints3 READ startingPoints3 NOTIFY startingPoints3Changed)
    Q_PROPERTY(QVariantList startingPointsAll READ startingPointsAll NOTIFY startingPointsAllChanged)

public:
    static Variant getInitialGameVariant();

    explicit GameModel(QObject* parent = nullptr);

    Q_INVOKABLE void deleteAllVar();

    Q_INVOKABLE bool isLegalPos(PieceModel* pieceModel, const QString& state,
                                QPointF coord) const;

    Q_INVOKABLE void keepOnlyPosition();

    Q_INVOKABLE void keepOnlySubtree();

    Q_INVOKABLE void nextColor();

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

    Q_INVOKABLE void initGameVariant(const QString& gameVariant);

    Q_INVOKABLE void autoSave();

    Q_INVOKABLE bool loadAutoSave();

    /** Find the piece model for a given move and set its transform and game
        coordinates accordingly but do not set its status to played yet. */
    Q_INVOKABLE PieceModel* preparePiece(int color, int move);

    Q_INVOKABLE bool save(const QString& file);

    Q_INVOKABLE bool saveAsciiArt(const QString& file);

    Q_INVOKABLE bool open(const QString& file);

    Q_INVOKABLE void makeMainVar();

    Q_INVOKABLE void moveDownVar();

    Q_INVOKABLE void moveUpVar();

    Q_INVOKABLE void truncate();

    Q_INVOKABLE void truncateChildren();

    Q_INVOKABLE QString getResultMessage();

    void setComment(const QString& comment);

    QQmlListProperty<PieceModel> pieceModels0();

    QQmlListProperty<PieceModel> pieceModels1();

    QQmlListProperty<PieceModel> pieceModels2();

    QQmlListProperty<PieceModel> pieceModels3();

    const QString& gameVariant() const { return m_gameVariant; }

    const QString& positionInfo() const { return m_positionInfo; }

    const QString& lastInputOutputError() const { return m_lastInputOutputError; }

    const QString& comment() const { return m_comment; }

    int nuColors() const { return m_nuColors; }

    int toPlay() const { return m_toPlay; }

    int altPlayer() const { return m_altPlayer; }

    float points0() const { return m_points0; }

    float points1() const { return m_points1; }

    float points2() const { return m_points2; }

    float points3() const { return m_points3; }

    float bonus0() const { return m_bonus0; }

    float bonus1() const { return m_bonus1; }

    float bonus2() const { return m_bonus2; }

    float bonus3() const { return m_bonus3; }

    bool hasMoves0() const { return m_hasMoves0; }

    bool hasMoves1() const { return m_hasMoves1; }

    bool hasMoves2() const { return m_hasMoves2; }

    bool hasMoves3() const { return m_hasMoves3; }

    bool isGameOver() const { return m_isGameOver; }

    bool isGameEmpty() const { return m_isGameEmpty; }

    bool canUndo() const { return m_canUndo; }

    bool canGoBackward() const { return m_canGoBackward; }

    bool canGoForward() const { return m_canGoForward; }

    bool hasPrevVar() const { return m_hasPrevVar; }

    bool hasNextVar() const { return m_hasNextVar; }

    bool hasVariations() const { return m_hasVariations; }

    bool isMainVar() const { return m_isMainVar; }

    const QVariantList& startingPoints0() const { return m_startingPoints0; }

    const QVariantList& startingPoints1() const { return m_startingPoints1; }

    const QVariantList& startingPoints2() const { return m_startingPoints2; }

    const QVariantList& startingPoints3() const { return m_startingPoints3; }

    const QVariantList& startingPointsAll() const { return m_startingPointsAll; }

    const Board& getBoard() const { return m_game.get_board(); }

signals:
    /** Position is about to change due to new game or navigation or editing of
        the game tree. */
    void positionAboutToChange();

    /** Position changed due to new game or navigation or editing of the
        game tree. */
    void positionChanged();

    void toPlayChanged();

    void altPlayerChanged();

    void points0Changed();

    void points1Changed();

    void points2Changed();

    void points3Changed();

    void bonus0Changed();

    void bonus1Changed();

    void bonus2Changed();

    void bonus3Changed();

    void hasMoves0Changed();

    void hasMoves1Changed();

    void hasMoves2Changed();

    void hasMoves3Changed();

    void hasVariationsChanged();

    void isGameOverChanged();

    void isGameEmptyChanged();

    void isMainVarChanged();

    void canUndoChanged();

    void canGoBackwardChanged();

    void canGoForwardChanged();

    void hasPrevVarChanged();

    void hasNextVarChanged();

    void gameVariantChanged();

    void positionInfoChanged();

    void commentChanged();

    void nuColorsChanged();

    void startingPoints0Changed();

    void startingPoints1Changed();

    void startingPoints2Changed();

    void startingPoints3Changed();

    void startingPointsAllChanged();

private:
    Game m_game;

    QString m_gameVariant;

    QString m_positionInfo;

    QString m_comment;

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

    bool m_hasVariations = false;

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

    PieceModel* m_lastMovePieceModel = nullptr;

    QVariantList m_startingPoints0;

    QVariantList m_startingPoints1;

    QVariantList m_startingPoints2;

    QVariantList m_startingPoints3;

    QVariantList m_startingPointsAll;

    QVariantList m_tmpPoints;

    QTextCodec* m_textCodec;


    void createPieceModels();

    void createPieceModels(Color c, QList<PieceModel*>& pieceModels);

    QString decode(const string& s) const;

    QByteArray encode(const QString& s) const;

    bool findMove(const PieceModel& pieceModel, const QString& state,
                  QPointF coord, Move& mv) const;

    QList<PieceModel*>& getPieceModels(Color c);

    void gotoNode(const SgfNode& node);

    void gotoNode(const SgfNode* node);

    bool open(istream& in);

    void preparePieceGameCoord(PieceModel* pieceModel, Move mv);

    void preparePieceTransform(PieceModel* pieceModel, Move mv);

    template<typename T>
    void set(T& target, const T& value, void (GameModel::*changedSignal)());

    void setUtf8();

    void updateIsGameEmpty();

    PieceModel* updatePiece(Color c, Move mv,
                            array<bool, Board::max_pieces>& isPlayed);

    void updatePieces();

    void updateProperties();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_GAME_MODEL_H
