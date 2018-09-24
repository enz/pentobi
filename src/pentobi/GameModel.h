//-----------------------------------------------------------------------------
/** @file pentobi/GameModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_GAME_MODEL_H
#define PENTOBI_GAME_MODEL_H

#include <QDateTime>
#include <QUrl>
#include "PieceModel.h"
#include "libpentobi_base/Game.h"

class QTextCodec;

using namespace std;
using libboardgame_sgf::SgfNode;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::Board;
using libpentobi_base::Game;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::ScoreType;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class GameMove
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int color READ color CONSTANT)

public:
    GameMove(QObject* parent, ColorMove mv)
        : QObject(parent),
          m_move(mv)
    { }


    Q_INVOKABLE bool isNull() const { return m_move.is_null(); }


    int color() const { return static_cast<int>(m_move.color.to_int()); }

    ColorMove get() const { return m_move; }

private:
    ColorMove m_move;
};

//-----------------------------------------------------------------------------

class GameModel
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString gameVariant READ gameVariant NOTIFY gameVariantChanged)
    Q_PROPERTY(QString positionInfo READ positionInfo NOTIFY positionInfoChanged)
    Q_PROPERTY(QString positionInfoShort READ positionInfoShort NOTIFY positionInfoShortChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
    Q_PROPERTY(QString file READ file NOTIFY fileChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)
    Q_PROPERTY(unsigned nuColors READ nuColors NOTIFY nuColorsChanged)
    Q_PROPERTY(unsigned nuPlayers READ nuPlayers NOTIFY nuPlayersChanged)
    Q_PROPERTY(unsigned toPlay READ toPlay NOTIFY toPlayChanged)
    Q_PROPERTY(unsigned altPlayer READ altPlayer NOTIFY altPlayerChanged)
    Q_PROPERTY(int moveNumber READ moveNumber NOTIFY moveNumberChanged)
    Q_PROPERTY(int movesLeft READ movesLeft NOTIFY movesLeftChanged)
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
    Q_PROPERTY(bool isBoardEmpty READ isBoardEmpty NOTIFY isBoardEmptyChanged)
    Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY isGameOverChanged)
    Q_PROPERTY(bool isModified READ isModified WRITE setIsModified NOTIFY isModifiedChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canGoBackward READ canGoBackward NOTIFY canGoBackwardChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY canGoForwardChanged)
    Q_PROPERTY(bool hasPrevVar READ hasPrevVar NOTIFY hasPrevVarChanged)
    Q_PROPERTY(bool hasNextVar READ hasNextVar NOTIFY hasNextVarChanged)
    Q_PROPERTY(bool hasVariations READ hasVariations NOTIFY hasVariationsChanged)
    Q_PROPERTY(bool hasEarlierVar READ hasEarlierVar NOTIFY hasEarlierVarChanged)
    Q_PROPERTY(bool isMainVar READ isMainVar NOTIFY isMainVarChanged)
    Q_PROPERTY(bool showVariations MEMBER m_showVariations WRITE setShowVariations NOTIFY showVariationsChanged)
    Q_PROPERTY(QVariantList startingPoints0 READ startingPoints0 NOTIFY startingPoints0Changed)
    Q_PROPERTY(QVariantList startingPoints1 READ startingPoints1 NOTIFY startingPoints1Changed)
    Q_PROPERTY(QVariantList startingPoints2 READ startingPoints2 NOTIFY startingPoints2Changed)
    Q_PROPERTY(QVariantList startingPoints3 READ startingPoints3 NOTIFY startingPoints3Changed)
    Q_PROPERTY(QVariantList startingPointsAny READ startingPointsAny NOTIFY startingPointsAnyChanged)
    Q_PROPERTY(QString playerName0 READ playerName0 WRITE setPlayerName0 NOTIFY playerName0Changed)
    Q_PROPERTY(QString playerName1 READ playerName1 WRITE setPlayerName1 NOTIFY playerName1Changed)
    Q_PROPERTY(QString playerName2 READ playerName2 WRITE setPlayerName2 NOTIFY playerName2Changed)
    Q_PROPERTY(QString playerName3 READ playerName3 WRITE setPlayerName3 NOTIFY playerName3Changed)
    Q_PROPERTY(QString date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(QString event READ getEvent WRITE setEvent NOTIFY eventChanged)
    Q_PROPERTY(QString round READ getRound WRITE setRound NOTIFY roundChanged)

public:
    static const int maxRecentFiles = 9;

    static Variant getInitialGameVariant();


    explicit GameModel(QObject* parent = nullptr);

    ~GameModel() override;


    Q_INVOKABLE void addSetup(PieceModel* pieceModel, QPointF coord);

    /** Remove a piece from the board.
        Updates setup properties in the current node.
        @param pos The point on the board in game coordinates.
        @return The PieceModel corresponding to the removed piece or null if
        there is no piece at this location. */
    Q_INVOKABLE PieceModel* addEmpty(const QPoint& pos);

    Q_INVOKABLE void clearRecentFiles();

    Q_INVOKABLE bool createFolder(const QUrl& folder);

    Q_INVOKABLE void deleteAllVar();

    Q_INVOKABLE bool findNextComment();

    Q_INVOKABLE bool findNextCommentContinueFromRoot();

    Q_INVOKABLE int getMoveNumberAt(const QPoint& pos);

    Q_INVOKABLE QString getPlayerString(int player);

    Q_INVOKABLE QString getVariationInfo() const;

    Q_INVOKABLE bool isLegalPos(PieceModel* pieceModel, const QString& state,
                                QPointF coord) const;

    Q_INVOKABLE bool isLegalSetupPos(PieceModel* pieceModel,
                                     const QString& state,
                                     QPointF coord) const;

    Q_INVOKABLE void keepOnlyPosition();

    Q_INVOKABLE void keepOnlySubtree();

    Q_INVOKABLE void nextColor();

    Q_INVOKABLE bool openByteArray(const QByteArray& byteArray);

    Q_INVOKABLE bool openClipboard();

    Q_INVOKABLE bool openFile(const QString& file);

    Q_INVOKABLE PieceModel* preparePiece(GameMove* move);

    Q_INVOKABLE void playPiece(PieceModel* pieceModel, QPointF coord);

    Q_INVOKABLE void playMove(GameMove* move);

    Q_INVOKABLE void newGame();

    Q_INVOKABLE void undo();

    Q_INVOKABLE QString getMoveAnnotation(int moveNumber);

    Q_INVOKABLE void goBeginning();

    Q_INVOKABLE void goBackward();

    Q_INVOKABLE void goBackward10();

    Q_INVOKABLE void goForward();

    Q_INVOKABLE void goForward10();

    Q_INVOKABLE void goEnd();

    Q_INVOKABLE void goNextVar();

    Q_INVOKABLE void goPrevVar();

    Q_INVOKABLE void backToMainVar();

    Q_INVOKABLE void gotoBeginningOfBranch();

    Q_INVOKABLE void gotoMove(int n);

    Q_INVOKABLE void changeGameVariant(const QString& gameVariant);

    Q_INVOKABLE void autoSave();

    Q_INVOKABLE bool loadAutoSave();

    Q_INVOKABLE bool save(const QString& file);

    Q_INVOKABLE bool saveAsciiArt(const QString& file);

    Q_INVOKABLE void setMoveAnnotation(int moveNumber,
                                       const QString& annotation);

    Q_INVOKABLE void makeMainVar();

    Q_INVOKABLE void moveDownVar();

    Q_INVOKABLE void moveUpVar();

    Q_INVOKABLE void truncate();

    Q_INVOKABLE void truncateChildren();

    Q_INVOKABLE QString getResultMessage();

    Q_INVOKABLE bool checkFileExists(const QString& file);

    Q_INVOKABLE bool checkFileModifiedOutside();

    Q_INVOKABLE bool checkAutosaveModifiedOutside();

    Q_INVOKABLE GameMove* findMoveNext();

    Q_INVOKABLE GameMove* findMovePrevious();

    Q_INVOKABLE PieceModel* pickNamedPiece(const QString& name,
                                           PieceModel* currentPickedPiece);

    Q_INVOKABLE PieceModel* nextPiece(PieceModel* currentPickedPiece);

    Q_INVOKABLE PieceModel* previousPiece(PieceModel* currentPickedPiece);

    Q_INVOKABLE QString suggestFileName(const QUrl& folder,
                                        const QString& fileEnding);

    Q_INVOKABLE QString suggestGameFileName(const QUrl& folder);

    Q_INVOKABLE QString suggestNewFolderName(const QUrl& folder);

    Q_INVOKABLE QString getError() const { return m_error; }

    Q_INVOKABLE QVariantList getPieceModels(int color);


    QByteArray getSgf() const;

    void setComment(const QString& comment);

    const QString& gameVariant() const { return m_gameVariant; }

    const QString& positionInfo() const { return m_positionInfo; }

    const QString& positionInfoShort() const { return m_positionInfoShort; }

    const QString& file() const { return m_file; }

    const QString& comment() const { return m_comment; }

    unsigned nuColors() const { return m_nuColors; }

    unsigned nuPlayers() const { return m_nuPlayers; }

    unsigned toPlay() const { return m_toPlay; }

    unsigned altPlayer() const { return m_altPlayer; }

    int moveNumber() const { return m_moveNumber; }

    int movesLeft() const { return m_movesLeft; }

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

    bool isBoardEmpty() const { return m_isBoardEmpty; }

    bool isGameOver() const { return m_isGameOver; }

    bool isModified() const { return m_isModified; }

    bool canUndo() const { return m_canUndo; }

    bool canGoBackward() const { return m_canGoBackward; }

    bool canGoForward() const { return m_canGoForward; }

    bool hasEarlierVar() const { return m_hasEarlierVar; }

    bool hasPrevVar() const { return m_hasPrevVar; }

    bool hasNextVar() const { return m_hasNextVar; }

    bool hasVariations() const { return m_hasVariations; }

    bool isMainVar() const { return m_isMainVar; }

    const QStringList& recentFiles() const { return m_recentFiles; }

    const QVariantList& startingPoints0() const { return m_startingPoints0; }

    const QVariantList& startingPoints1() const { return m_startingPoints1; }

    const QVariantList& startingPoints2() const { return m_startingPoints2; }

    const QVariantList& startingPoints3() const { return m_startingPoints3; }

    const QVariantList& startingPointsAny() const { return m_startingPointsAny; }

    const QString& playerName0() const { return m_playerName0; }

    const QString& playerName1() const { return m_playerName1; }

    const QString& playerName2() const { return m_playerName2; }

    const QString& playerName3() const { return m_playerName3; }

    const QString& date() const { return m_date; }

    const QString& time() const { return m_time; }

    // Avoid conflict with QObject::event()
    const QString& getEvent() const { return m_event; }

    // Avoid conflict with round(), which cannot be resolved by using fully
    // qualified std::round(), because with many GCC versions it's in the
    // global namespace (http://stackoverflow.com/questions/1882689)
    const QString& getRound() const { return m_round; }

    void setIsModified(bool isModified);

    void setMoveAnnotationAtNode(const SgfNode& node,
                                 const QString& annotation);

    void setPlayerName0(const QString& name);

    void setPlayerName1(const QString& name);

    void setPlayerName2(const QString& name);

    void setPlayerName3(const QString& name);

    void setDate(const QString& date);

    void setShowVariations(bool showVariations);

    void setTime(const QString& time);

    void setEvent(const QString& event);

    void setRound(const QString& round);

    const Game& getGame() const { return m_game; }

    const Board& getBoard() const { return m_game.get_board(); }

    void gotoNode(const SgfNode& node);

    void gotoNode(const SgfNode* node);

signals:
    /** Loaded Blokus SGF file has invalid syntax.
        Triggered when a loaded SGF file causes a problem later than at load
        time (e.g. invalid move property value in a side variation). The
        reason can be retrieved with in getError().*/
    void invalidSgfFile();

    /** Position is about to change due to new game or navigation or editing of
        the game tree. */
    void positionAboutToChange();

    /** Position changed due to new game or navigation or editing of the
        game tree. */
    void positionChanged();

    void toPlayChanged();

    void altPlayerChanged();

    void fileChanged();

    void points0Changed();

    void points1Changed();

    void points2Changed();

    void points3Changed();

    void bonus0Changed();

    void bonus1Changed();

    void bonus2Changed();

    void bonus3Changed();

    void hasEarlierVarChanged();

    void hasMoves0Changed();

    void hasMoves1Changed();

    void hasMoves2Changed();

    void hasMoves3Changed();

    void hasVariationsChanged();

    void isBoardEmptyChanged();

    void isGameOverChanged();

    void isModifiedChanged();

    void isMainVarChanged();

    void canUndoChanged();

    void canGoBackwardChanged();

    void canGoForwardChanged();

    void hasPrevVarChanged();

    void hasNextVarChanged();

    void gameVariantChanged();

    void positionInfoChanged();

    void positionInfoShortChanged();

    void commentChanged();

    void moveNumberChanged();

    void movesLeftChanged();

    void nuColorsChanged();

    void nuPlayersChanged();

    void recentFilesChanged();

    void startingPoints0Changed();

    void startingPoints1Changed();

    void startingPoints2Changed();

    void startingPoints3Changed();

    void startingPointsAnyChanged();

    void playerName0Changed();

    void playerName1Changed();

    void playerName2Changed();

    void playerName3Changed();

    void dateChanged();

    void showVariationsChanged();

    void timeChanged();

    void eventChanged();

    void roundChanged();

private:
    Game m_game;

    QString m_gameVariant;

    QString m_positionInfo;

    QString m_positionInfoShort;

    QString m_comment;

    QString m_error;

    QString m_file;

    QString m_playerName0;

    QString m_playerName1;

    QString m_playerName2;

    QString m_playerName3;

    QString m_date;

    QString m_time;

    QString m_event;

    QString m_round;

    QStringList m_recentFiles;

    QDateTime m_fileDate;

    QDateTime m_autosaveDate;

    unsigned m_nuColors;

    unsigned m_nuPlayers;

    unsigned m_toPlay = 0;

    unsigned m_altPlayer = 0;

    int m_moveNumber = 0;

    int m_movesLeft = 0;

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

    bool m_isBoardEmpty = true;

    bool m_isGameOver = false;

    bool m_isModified = false;

    bool m_canUndo = false;

    bool m_canGoForward = false;

    bool m_canGoBackward = false;

    bool m_hasEarlierVar = false;

    bool m_hasPrevVar = false;

    bool m_hasNextVar = false;

    bool m_isMainVar = true;

    bool m_showVariations = true;

    ColorMap<QVariantList> m_pieceModels;

    PieceModel* m_lastMovePieceModel = nullptr;

    QVariantList m_startingPoints0;

    QVariantList m_startingPoints1;

    QVariantList m_startingPoints2;

    QVariantList m_startingPoints3;

    QVariantList m_startingPointsAny;

    QVariantList m_tmpPoints;

    QTextCodec* m_textCodec;

    unique_ptr<MoveList> m_legalMoves;

    unsigned m_legalMoveIndex;

    /** Local variable reused for efficiency. */
    unique_ptr<MoveMarker> m_marker;


    void addRecentFile(const QString& file);

    bool checkSetupAllowed() const;

    void clearFile();

    void createPieceModels();

    void createPieceModels(Color c);

    QString decode(const string& s) const;

    QByteArray encode(const QString& s) const;

    bool findMove(const PieceModel& pieceModel, const QString& state,
                  QPointF coord, Move& mv) const;

    PieceModel* findUnplayedPieceModel(Color c, Piece piece);

    QString getMoveAnnotationAtNode(const SgfNode& node) const;

    ColorMove getMoveAt(const QPoint& pos) const;

    void initGame(Variant variant);

    void initGameVariant(Variant variant);

    void loadRecentFiles();

    bool openStream(istream& in);

    void prepareFindMove();

    void preparePieceGameCoord(PieceModel* pieceModel, Move mv);

    void preparePieceTransform(PieceModel* pieceModel, Move mv);

    void preparePositionChange();

    void restoreAutoSaveLocation();

    template<typename T>
    bool set(T& target, const T& value, void (GameModel::*changedSignal)());

    void setFile(const QString& file);

    void setSetupPlayer();

    void setUtf8();

    void updateFileInfo(const QString& file);

    void updateGameInfo();

    void updateIsModified();

    PieceModel* updatePiece(Color c, Move mv,
                            array<bool, Board::max_pieces>& isPlayed);

    void updatePieces();

    void updatePositionInfo();

    void updateProperties();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_GAME_MODEL_H
