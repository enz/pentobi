//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_MAIN_WINDOW_H
#define PENTOBI_MAIN_WINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>
#include <QMessageBox>
#include "AnalyzeGameWindow.h"
#include "RatingDialog.h"
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_mcts/Player.h"
#include "libpentobi_gui/GuiBoard.h"
#include "libpentobi_gui/HelpWindow.h"
#include "libpentobi_gui/OrientationDisplay.h"
#include "libpentobi_gui/PieceSelector.h"
#include "libpentobi_gui/ScoreDisplay.h"
#include "libpentobi_gui/StatusBarButton.h"

class QActionGroup;
class QPlainTextEdit;
class QSplitter;

using namespace std;
using libboardgame_sgf::Node;
using libboardgame_util::ArrayList;
using libboardgame_util::Exception;
using libpentobi_base::Board;
using libpentobi_base::ColorMap;
using libpentobi_base::Game;
using libpentobi_base::Variant;
using libpentobi_base::Move;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& initialFile = "", const QString& manualDir = "",
               const QString& booksDir = "", bool noBook = false,
               size_t memory = 0);

    bool eventFilter(QObject* object, QEvent* event);

    QSize sizeHint() const;

public slots:
    void about();

    void analyzeGame();

    void backward();

    void backward10();

    void backToMainVariation();

    void beginning();

    void beginningOfBranch();

    void clearSelectedPiece();

    void computerColors();

    void deleteAllVariations();

    void end();

    void exportAsciiArt();

    void exportImage();

    void findMove();

    void findNextComment();

    void flipPieceHorizontally();

    void flipPieceVertically();

    void forward();

    void forward10();

    void gotoMove();

    /** Go to a node if a node with a position defined by a sequence of moves
        still exists. */
    void gotoPosition(Variant variant, const vector<ColorMove>& moves);

    void help();

    void gameInfo();

    void interrupt();

    void keepOnlyPosition();

    void keepOnlySubtree();

    void makeMainVariation();

    void moveDownVariation();

    void moveUpVariation();

    void newGame();

    void newRatedGame();

    void nextVariation();

    void nextVariation10();

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

    void previousVariation10();

    void quit();

    void rotatePieceAnticlockwise();

    void rotatePieceClockwise();

    void save();

    void saveAs();

    void selectNextColor();

    void selectPiece1();

    void selectPiece2();

    void selectPieceA();

    void selectPieceC();

    void selectPieceF();

    void selectPieceG();

    void selectPieceI();

    void selectPieceL();

    void selectPieceN();

    void selectPieceO();

    void selectPieceP();

    void selectPieceS();

    void selectPieceT();

    void selectPieceU();

    void selectPieceV();

    void selectPieceW();

    void selectPieceX();

    void selectPieceY();

    void selectPieceZ();

    void selectPiece(Color c, Piece piece);

    void selectPiece(Color c, Piece piece, const Transform* transform);

    void setLevel(int level);

    void truncate();

    void truncateChildren();

    void undo();

    void showVariations(bool checked);

    void showRating();

    void setDeterministic();

protected:
    void closeEvent(QCloseEvent* event);

    void wheelEvent(QWheelEvent* event);

private:
    struct GenMoveResult
    {
        bool playSingleMove;

        Color color;

        Move move;

        unsigned genMoveId;
    };

    /** Possible values for m_level are in 1..maxLevel */
    static const int maxLevel = 7;

    static const int maxRecentFiles = 9;

    unique_ptr<Game> m_game;

    Color m_currentColor;

    unique_ptr<Player> m_player;

    bool m_isGenMoveRunning;

    bool m_isAnalyzeRunning;

    /** Flag indicating that the position after the last move played was
        a terminal position. */
    bool m_gameFinished;

    bool m_isRated;

    bool m_isRatedGameFinished;

    /** Flag set while setting the text in m_comment for fast return in the
        textChanged() handler.
        Used because QPlainTextEdit does not have a textEdited() signal and
        we only need to handle edits. */
    bool m_ignoreCommentTextChanged;

    /** Color played by the user in a rated game.
        Only defined if m_isRated is true. In game variants with multiple
        colors per player, the user plays all colors of the player with
        this color. */
    Color m_ratedGameColor;

    /** Integer ID assigned to the currently running move generation.
        Used to ignore finished events from canceled move generations. */
    unsigned m_genMoveId;

    /** Beginning of move number range of last moves played by the computer
        without pause.
        Used to mark all last moves played by the computer if it plays several
        moves in a row because the other players have no more moves. Otherwise,
        it would be hard to see what the last moves were if the computer plays
        quickly. A value of 0 means that there are no last moves played by
        the computer to be marked. */
    unsigned m_lastComputerMovesBegin;

    /** End (inclusive) of move number range of last moves played by the
        computer without pause.
        Only defined if m_lastComputerMovesBegin is not 0.
        @see m_lastComputerMovesBegin */
    unsigned m_lastComputerMovesEnd;

    /** Current playing level of m_player.
        Only use if m_useTimeLimit is false. Possible values for m_level are in
        1..maxLevel. Only used if m_timeLimit is zero. Stored independently of
        the player and set at the player before each move generation, such that
        setting a new level does not require to abort a running move
        generation. */
    int m_level;

    unique_ptr<RatingHistory> m_history;

    QToolBar* m_toolBar;

    GuiBoard* m_guiBoard;

    QString m_manualDir;

    ColorMap<bool> m_computerColors;

    ColorMap<PieceSelector*> m_pieceSelector;

    OrientationDisplay* m_orientationDisplay;

    ScoreDisplay* m_scoreDisplay;

    QSplitter* m_splitter;

    QPlainTextEdit* m_comment;

    HelpWindow* m_helpWindow;

    RatingDialog* m_ratingDialog;

    AnalyzeGameWindow* m_analyzeGameWindow;

    QAction* m_actionAbout;

    QAction* m_actionAnalyzeGame;

    QAction* m_actionBackward;

    QAction* m_actionBackward10;

    QAction* m_actionBackToMainVariation;

    QAction* m_actionBadMove;

    QAction* m_actionBeginning;

    QAction* m_actionBeginningOfBranch;

    QAction* m_actionClearSelectedPiece;

    QAction* m_actionComputerColors;

    QAction* m_actionCoordinates;

    QAction* m_actionDeleteAllVariations;

    QAction* m_actionDoubtfulMove;

    QAction* m_actionEnd;

    QAction* m_actionExportAsciiArt;

    QAction* m_actionExportImage;

    QAction* m_actionFindMove;

    QAction* m_actionFindNextComment;

    QAction* m_actionFlipPieceHorizontally;

    QAction* m_actionFlipPieceVertically;

    QAction* m_actionForward;

    QAction* m_actionForward10;

    QAction* m_actionFullscreen;

    QAction* m_actionGameInfo;

    QAction* m_actionVariantClassic;

    QAction* m_actionVariantClassic2;

    QAction* m_actionVariantDuo;

    QAction* m_actionVariantJunior;

    QAction* m_actionVariantTrigon;

    QAction* m_actionVariantTrigon2;

    QAction* m_actionVariantTrigon3;

    QAction* m_actionGoodMove;

    QAction* m_actionGotoMove;

    QAction* m_actionHelp;

    QAction* m_actionInterestingMove;

    QAction* m_actionInterrupt;

    QAction* m_actionKeepOnlyPosition;

    QAction* m_actionKeepOnlySubtree;

    QAction* m_actionLevel[maxLevel];

    QAction* m_actionMakeMainVariation;

    QAction* m_actionMoveDownVariation;

    QAction* m_actionMoveNumbersAll;

    QAction* m_actionMoveNumbersLast;

    QAction* m_actionMoveNumbersNone;

    QAction* m_actionMoveUpVariation;

    QAction* m_actionMoveSelectedPieceLeft;

    QAction* m_actionMoveSelectedPieceRight;

    QAction* m_actionMoveSelectedPieceUp;

    QAction* m_actionMoveSelectedPieceDown;

    QAction* m_actionNextPiece;

    QAction* m_actionNextTransform;

    QAction* m_actionNextVariation;

    QAction* m_actionNextVariation10;

    QAction* m_actionNewGame;

    QAction* m_actionNewRatedGame;

    QAction* m_actionNoMoveAnnotation;

    QAction* m_actionOpen;

    QAction* m_actionPlaceSelectedPiece;

    QAction* m_actionPlay;

    QAction* m_actionPlaySingleMove;

    QAction* m_actionPreviousPiece;

    QAction* m_actionPreviousTransform;

    QAction* m_actionPreviousVariation;

    QAction* m_actionPreviousVariation10;

    QAction* m_actionQuit;

    QAction* m_actionRecentFile[maxRecentFiles];

    QAction* m_actionRotatePieceAnticlockwise;

    QAction* m_actionRotatePieceClockwise;

    QAction* m_actionSave;

    QAction* m_actionSaveAs;

    QAction* m_actionShowComment;

    QAction* m_actionShowRating;

    QAction* m_actionShowToolbar;

    QAction* m_actionSelectNextColor;

    QAction* m_actionSelectPiece1;

    QAction* m_actionSelectPiece2;

    QAction* m_actionSelectPieceA;

    QAction* m_actionSelectPieceC;

    QAction* m_actionSelectPieceF;

    QAction* m_actionSelectPieceG;

    QAction* m_actionSelectPieceI;

    QAction* m_actionSelectPieceL;

    QAction* m_actionSelectPieceN;

    QAction* m_actionSelectPieceO;

    QAction* m_actionSelectPieceP;

    QAction* m_actionSelectPieceS;

    QAction* m_actionSelectPieceT;

    QAction* m_actionSelectPieceU;

    QAction* m_actionSelectPieceV;

    QAction* m_actionSelectPieceW;

    QAction* m_actionSelectPieceX;

    QAction* m_actionSelectPieceY;

    QAction* m_actionSelectPieceZ;

    QAction* m_actionSetupMode;

    QAction* m_actionTruncate;

    QAction* m_actionTruncateChildren;

    QAction* m_actionShowVariations;

    QAction* m_actionUndo;

    QAction* m_actionVeryGoodMove;

    QAction* m_actionVeryBadMove;

    QMenu* m_menuMoveAnnotation;

    QMenu* m_menuOpenRecent;

    QLabel* m_setupModeLabel;

    QLabel* m_ratedGameLabel;

    StatusBarButton* m_buttonFullscreen;

    QFutureWatcher<GenMoveResult> m_genMoveWatcher;

    QString m_file;

    unique_ptr<ArrayList<Move, Move::range>> m_legalMoves;

    unsigned m_legalMoveIndex;

    QLabel* m_moveNumber;

    GenMoveResult asyncGenMove(Color c, int genMoveId, bool playSingleMove);

    bool checkSave();

    bool checkQuit();

    void clearFile();

    void createActions();

    QWidget* createCentralWidget();

    QWidget* createLeftPanel();

    QAction* createLevelAction(QActionGroup* group, int level,
                               const QString& text);

    void createMenu();

    QWidget* createOrientationButtonBoxLeft();

    QWidget* createOrientationButtonBoxRight();

    QWidget* createOrientationSelector();

    QWidget* createRightPanel();

    void createToolBar();

    void cancelThread();

    void checkComputerMove();

    void clearStatus();

    bool computerPlaysAll() const;

    void deleteAutoSaveFile();

    void enablePieceSelector(Color c);

    void genMove(bool playSingleMove = false);

    const Board& getBoard() const;

    Variant getVariant() const;

    QString getFilter() const;

    QString getLastDir();

    QString getVersion() const;

    void gotoNode(const Node& node);

    void initGame();

    void initVariantActions();

    void initPieceSelectors();

    void initQuestion(QMessageBox& msgBox, const QString& text,
                      const QString& infoText = "");

    void leaveSetupMode();

    void play(Color c, Move mv, bool checkComputerMove);

    bool save(const QString& file);

    void setCommentText(const QString& text);

    void setVariant(Variant variant);

    void setPlayToolTip();

    void setRated(bool isRated);

    void setFile(const QString& file);

    void showError(const QString& message, const QString& infoText = "",
                   const QString& detailText = "");

    void gameOver();

    void showInfo(const QString& message, const QString& infoText = "",
                  const QString& detailText = "");

    void showInvalidFile(QString file, const Exception& e);

    void showMessage(QMessageBox::Icon icon, const QString& text,
                     const QString& infoText, const QString& detailText);

    void updateMoveNumber();

    void updateWindow(bool currentNodeChanged);

    void updateWindowModified();

    void updateComment();

    void updateMoveAnnotationActions();

    void loadHistory();

    void updateRecentFiles();

    void updateFlipActions();

    bool writeGame(const path& file);

private slots:
    void analyzeGameFinished();

    void badMove(bool checked);

    void commentChanged();

    void coordinates(bool checked);

    void doubtfulMove(bool checked);

    void fullscreen(bool checked);

    void variantClassic(bool checked);

    void variantClassic2(bool checked);

    void variantDuo(bool checked);

    void variantJunior(bool checked);

    void variantTrigon(bool checked);

    void variantTrigon2(bool checked);

    void variantTrigon3(bool checked);

    void genMoveFinished();

    void goodMove(bool checked);

    void interestingMove(bool checked);

    void noMoveAnnotation(bool checked);

    void openRecentFile();

    void orientationDisplayColorClicked(Color c);

    void rememberFile(const QString& file);

    void rememberDir(const QString& file);

    void selectNamedPiece(const char* name1, const char* name2 = 0,
                          const char* name3 = 0, const char* name4 = 0);

    void setLevel(bool checked);

    void setMoveNumbersAll(bool checked);

    void setMoveNumbersLast(bool checked);

    void setMoveNumbersNone(bool checked);

    void setSetupPlayer();

    void setupMode(bool checked);

    void showComment(bool checked);

    void showStatus(const QString& text, bool temporary = false);

    void showThinking();

    void veryBadMove(bool checked);

    void veryGoodMove(bool checked);
};

inline const Board& MainWindow::getBoard() const
{
    return m_game->get_board();
}

inline Variant MainWindow::getVariant() const
{
    return m_game->get_variant();
}

//-----------------------------------------------------------------------------

#endif // PENTOBI_MAIN_WINDOW_H
