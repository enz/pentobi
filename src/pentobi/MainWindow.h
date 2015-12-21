//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_MAIN_WINDOW_H
#define PENTOBI_MAIN_WINDOW_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QMainWindow>
#include "RatingHistory.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_mcts/Player.h"

class QActionGroup;
class QLabel;
class QPlainTextEdit;
class QSplitter;
class AnalyzeGameWindow;
class GuiBoard;
class HelpWindow;
class LeaveFullscreenButton;
class OrientationDisplay;
class PieceSelector;
class RatingDialog;
class ScoreDisplay;

using namespace std;
using libboardgame_sgf::SgfNode;
using libboardgame_base::Transform;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;
using libpentobi_base::Board;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Variant variant, const QString& initialFile = "",
               const QString& helpDir = "",
               unsigned maxLevel = Player::max_supported_level,
               const QString& booksDir = "", bool noBook = false,
               unsigned nuThreads = 0);

    ~MainWindow();

    bool eventFilter(QObject* object, QEvent* event) override;

    QSize sizeHint() const override;

public slots:
    void about();

    void analyzeGame();

    void backward();

    void backToMainVariation();

    void beginning();

    void beginningOfBranch();

    void clearPiece();

    void computerColors();

    void deleteAllVariations();

    void end();

    void exportAsciiArt();

    void exportImage();

    void findMove();

    void findNextComment();

    void flipHorizontally();

    void flipVertically();

    void forward();

    void gotoMove();

    /** Go to a node if a node with a position defined by a sequence of moves
        still exists. */
    void gotoPosition(Variant variant, const vector<ColorMove>& moves);

    void help();

    void gameInfo();

    /** Abort current move generation and don't play a move. */
    void interrupt();

    /** Abort current move generation and play best move found so far. */
    void interruptPlay();

    void keepOnlyPosition();

    void keepOnlySubtree();

    void makeMainVariation();

    void moveDownVariation();

    void moveUpVariation();

    void newGame();

    void nextVariation();

    void nextPiece();

    void nextTransform();

    void open();

    bool open(const QString& file, bool isTemporary = false);

    void placePiece(Color c, Move mv);

    void play();

    void playSingleMove();

    void pointClicked(Point p);

    void previousPiece();

    void previousTransform();

    void previousVariation();

    void ratedGame();

    void rotateAnticlockwise();

    void rotateClockwise();

    void save();

    void saveAs();

    void selectNextColor();

    void selectPiece(Color c, Piece piece);

    void selectPiece(Color c, Piece piece, const Transform* transform);

    void setLevel(unsigned level);

    void truncate();

    void truncateChildren();

    void undo();

    void showToolbar(bool checked);

    void showVariations(bool checked);

    void showRating();

    void setDeterministic();

    void setNoDelay();

protected:
    void closeEvent(QCloseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

private:
    struct GenMoveResult
    {
        bool playSingleMove;

        Color color;

        Move move;

        unsigned genMoveId;
    };

    static const int maxRecentFiles = 9;

    Game m_game;

    const Board& m_bd;

    Color m_currentColor;

    unique_ptr<Player> m_player;

    bool m_noDelay = false;

    /** Was window maximized before entering fullscreen. */
    bool m_wasMaximized = false;

    bool m_isGenMoveRunning = false;

    bool m_isAnalyzeRunning = false;

    /** Should the computer generate a move if it is its turn?
        Enabled on game start (if the computer plays at least one color)
        or after selecting Play. Disabled when navigating in the game. */
    bool m_autoPlay = false;

    /** Flag indicating that the position after the last move played was
        a terminal position. */
    bool m_gameFinished;

    bool m_isRated = false;

    /** Flag set while setting the text in m_comment for fast return in the
        textChanged() handler.
        Used because QPlainTextEdit does not have a textEdited() signal and
        we only need to handle edits. */
    bool m_ignoreCommentTextChanged = false;

    /** Color played by the user in a rated game.
        Only defined if m_isRated is true. In game variants with multiple
        colors per player, the user plays all colors of the player with
        this color. */
    Color m_ratedGameColor;

    /** Integer ID assigned to the currently running move generation.
        Used to ignore finished events from canceled move generations. */
    unsigned m_genMoveId = 0;

    /** Beginning of move number range of last moves played by the computer
        without pause.
        Used to mark all last moves played by the computer if it plays several
        moves in a row because the other players have no more moves. Otherwise,
        it would be hard to see what the last moves were if the computer plays
        quickly. A value of 0 means that there are no last moves played by
        the computer to be marked. */
    unsigned m_lastComputerMovesBegin = 0;

    /** End (inclusive) of move number range of last moves played by the
        computer without pause.
        Only defined if m_lastComputerMovesBegin is not 0.
        @see m_lastComputerMovesBegin */
    unsigned m_lastComputerMovesEnd = 0;

    unsigned m_maxLevel;

    /** Current playing level of m_player.
        Only use if m_useTimeLimit is false. Possible values for m_level are in
        1..maxLevel. Only used if m_timeLimit is zero. Stored independently of
        the player and set at the player before each move generation, such that
        setting a new level does not require to abort a running move
        generation. */
    unsigned m_level;

    RandomGenerator m_random;

    unique_ptr<RatingHistory> m_history;

    /** Local variable in findMove().
        Reused for efficiency. */
    unique_ptr<MoveMarker> m_marker;

    GuiBoard* m_guiBoard;

    QString m_helpDir;

    ColorMap<bool> m_computerColors;

    ColorMap<PieceSelector*> m_pieceSelector;

    OrientationDisplay* m_orientationDisplay;

    ScoreDisplay* m_scoreDisplay;

    QSplitter* m_splitter;

    QPlainTextEdit* m_comment;

    HelpWindow* m_helpWindow = nullptr;

    RatingDialog* m_ratingDialog = nullptr;

    AnalyzeGameWindow* m_analyzeGameWindow = nullptr;

    QAction* m_actionAbout;

    QAction* m_actionAnalyzeGame;

    QAction* m_actionBackward;

    QAction* m_actionBackToMainVariation;

    QAction* m_actionBadMove;

    QAction* m_actionBeginning;

    QAction* m_actionBeginningOfBranch;

    QAction* m_actionClearPiece;

    QAction* m_actionComputerColors;

    QAction* m_actionCoordinates;

    QAction* m_actionDeleteAllVariations;

    QAction* m_actionDoubtfulMove;

    QAction* m_actionEnd;

    QAction* m_actionExportAsciiArt;

    QAction* m_actionExportImage;

    QAction* m_actionFindMove;

    QAction* m_actionFindNextComment;

    QAction* m_actionFlipHorizontally;

    QAction* m_actionFlipVertically;

    QAction* m_actionForward;

    QAction* m_actionFullscreen;

    QAction* m_actionGameInfo;

    QAction* m_actionGoodMove;

    QAction* m_actionGotoMove;

    QAction* m_actionHelp;

    QAction* m_actionInterestingMove;

    QAction* m_actionInterrupt;

    QAction* m_actionInterruptPlay;

    QAction* m_actionKeepOnlyPosition;

    QAction* m_actionKeepOnlySubtree;

    QAction* m_actionLeaveFullscreen;

    vector<QAction*> m_actionLevel;

    QAction* m_actionMakeMainVariation;

    QAction* m_actionMoveDownVariation;

    QAction* m_actionMoveMarkingAllNumber;

    QAction* m_actionMoveMarkingLastDot;

    QAction* m_actionMoveMarkingLastNumber;

    QAction* m_actionMoveMarkingNone;

    QAction* m_actionMoveUpVariation;

    QAction* m_actionMovePieceLeft;

    QAction* m_actionMovePieceRight;

    QAction* m_actionMovePieceUp;

    QAction* m_actionMovePieceDown;

    QAction* m_actionNextPiece;

    QAction* m_actionNextTransform;

    QAction* m_actionNextVariation;

    QAction* m_actionNew;

    QAction* m_actionRatedGame;

    QAction* m_actionNoMoveAnnotation;

    QAction* m_actionOpen;

    QAction* m_actionPlacePiece;

    QAction* m_actionPlay;

    QAction* m_actionPlaySingleMove;

    QAction* m_actionPreviousPiece;

    QAction* m_actionPreviousTransform;

    QAction* m_actionPreviousVariation;

    QAction* m_actionQuit;

    QAction* m_actionRecentFile[maxRecentFiles];

    QAction* m_actionRotateAnticlockwise;

    QAction* m_actionRotateClockwise;

    QAction* m_actionSave;

    QAction* m_actionSaveAs;

    QAction* m_actionShowComment;

    QAction* m_actionRating;

    QAction* m_actionShowToolbar;

    QAction* m_actionSelectNextColor;

    QAction* m_actionSetupMode;

    QAction* m_actionToolBarNoText;

    QAction* m_actionToolBarTextBesideIcons;

    QAction* m_actionToolBarTextBelowIcons;

    QAction* m_actionToolBarTextOnly;

    QAction* m_actionToolBarTextSystem;

    QAction* m_actionTruncate;

    QAction* m_actionTruncateChildren;

    QAction* m_actionShowVariations;

    QAction* m_actionUndo;

    QAction* m_actionVariantClassic;

    QAction* m_actionVariantClassic2;

    QAction* m_actionVariantClassic3;

    QAction* m_actionVariantDuo;

    QAction* m_actionVariantJunior;

    QAction* m_actionVariantNexos;

    QAction* m_actionVariantNexos2;

    QAction* m_actionVariantTrigon;

    QAction* m_actionVariantTrigon2;

    QAction* m_actionVariantTrigon3;

    QAction* m_actionVeryGoodMove;

    QAction* m_actionVeryBadMove;

    QMenu* m_menuExport;

    QMenu* m_menuLevel;

    QMenu* m_menuMoveAnnotation;

    QMenu* m_menuOpenRecent;

    QMenu* m_menuToolBarText;

    QMenu* m_menuVariant;

    QLabel* m_setupModeLabel;

    QLabel* m_ratedGameLabelText;

    QFutureWatcher<GenMoveResult> m_genMoveWatcher;

    QString m_file;

    unique_ptr<MoveList> m_legalMoves;

    unsigned m_legalMoveIndex;

    QLabel* m_moveNumber;

    LeaveFullscreenButton* m_leaveFullscreenButton = nullptr;

    int m_lastRemainingSeconds;

    int m_lastRemainingMinutes;

    /** Is the current game a game loaded from the autosave file?
        If yes, we need it to save again on quit even if it was not modified.
        Note that the autosave game is deleted after loading to avoid that
        it is used twice if two instances of Pentobi are started. */
    bool m_isAutoSaveLoaded;


    GenMoveResult asyncGenMove(Color c, int genMoveId, bool playSingleMove);

    bool checkSave();

    bool checkQuit();

    void clearFile();

    QAction* createAction(const QString& text = "");

    void createActions();

    QWidget* createCentralWidget();

    QWidget* createLeftPanel();

    QAction* createLevelAction(QActionGroup* group, unsigned level,
                               const QString& text);

    void createMenu();

    QLayout* createOrientationButtonBoxLeft();

    QLayout* createOrientationButtonBoxRight();

    QLayout* createOrientationSelector();

    QLayout* createRightPanel();

    void createToolBar();

    void cancelThread();

    void checkComputerMove();

    void clearStatus();

    bool computerPlaysAll() const;

    void deleteAutoSaveFile();

    void enablePieceSelector(Color c);

    void gameOver();

    void genMove(bool playSingleMove = false);

    QString getFilter() const;

    QString getLastDir();

    QString getVersion() const;

    void gotoNode(const SgfNode& node);

    void gotoNode(const SgfNode* node);

    void initGame();

    void initVariantActions();

    void initPieceSelectors();

    bool isComputerToPlay() const;

    void leaveSetupMode();

    void play(Color c, Move mv);

    void restoreLevel(Variant variant);

    bool save(const QString& file);

    void searchCallback(double elapsedSeconds, double remainingSeconds);

    void setCommentText(const QString& text);

    void setVariant(Variant variant);

    void setPlayToolTip();

    void setRated(bool isRated);

    void setFile(const QString& file);

    void showError(const QString& message, const QString& infoText = "",
                   const QString& detailText = "");

    void showInfo(const QString& message, const QString& infoText = "",
                  const QString& detailText = "", bool withIcon = false);

    void showInvalidFile(QString file, const exception& e);

    void showStatus(const QString& text, bool temporary = false);

    void updateMoveNumber();

    void updateWindow(bool currentNodeChanged);

    void updateWindowModified();

    void updateComment();

    void updateMoveAnnotationActions();

    void loadHistory();

    void updateRecentFiles();

    void updateFlipActions();

    bool writeGame(const string& file);

private slots:
    void analyzeGameFinished();

    void badMove(bool checked);

    void commentChanged();

    void continueRatedGame();

    void coordinates(bool checked);

    void doubtfulMove(bool checked);

    void fullscreen();

    void genMoveFinished();

    void goodMove(bool checked);

    void interestingMove(bool checked);

    void leaveFullscreen();

    void levelTriggered(bool checked);

    void noMoveAnnotation(bool checked);

    void openCheckSave(const QString& file);

    void openRecentFile();

    void orientationDisplayColorClicked(Color c);

    void rememberFile(const QString& file);

    void rememberDir(const QString& file);

    void selectNamedPiece();

    void setMoveMarkingAllNumber(bool checked);

    void setMoveMarkingLastNumber(bool checked);

    void setMoveMarkingLastDot(bool checked);

    void setMoveMarkingNone(bool checked);

    void setSetupPlayer();

    void setupMode(bool checked);

    void showComment(bool checked);

    void toolBarNoText(bool checked);

    void toolBarText(const QString& key, Qt::ToolButtonStyle style);

    void toolBarTextBesideIcons(bool checked);

    void toolBarTextBelowIcons(bool checked);

    void toolBarTextOnly(bool checked);

    void toolBarTextSystem(bool checked);

    void veryBadMove(bool checked);

    void veryGoodMove(bool checked);

    void variantClassic(bool checked);

    void variantClassic2(bool checked);

    void variantClassic3(bool checked);

    void variantDuo(bool checked);

    void variantJunior(bool checked);

    void variantTrigon(bool checked);

    void variantTrigon2(bool checked);

    void variantTrigon3(bool checked);

    void variantNexos(bool checked);

    void variantNexos2(bool checked);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_MAIN_WINDOW_H
