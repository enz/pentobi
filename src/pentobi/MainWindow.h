//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_MAIN_WINDOW_H
#define PENTOBI_MAIN_WINDOW_H

#include <QtGui>
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_mcts/Player.h"
#include "libpentobi_gui/GuiBoard.h"
#include "libpentobi_gui/HelpWindow.h"
#include "libpentobi_gui/OrientationDisplay.h"
#include "libpentobi_gui/PieceSelector.h"
#include "libpentobi_gui/ScoreDisplay.h"

using namespace std;
using libpentobi_base::ColorMap;
using libboardgame_sgf::Node;
using libpentobi_base::Board;
using libpentobi_base::Game;
using libpentobi_base::GameVariant;
using libpentobi_base::Move;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& initialFile = QString());

    QSize sizeHint() const;

public slots:
    void about();

    void backward();

    void backToMainVariation();

    void beginning();

    void clearSelectedPiece();

    void computerColor();

    void end();

    void exportAsciiArt();

    void exportImage();

    void findMove();

    void flipPieceHorizontally();

    void flipPieceVertically();

    void forward();

    void gotoMove();

    void help();

    void humanPlay(Color c, Move mv);

    void gameInfo();

    void interrupt();

    void makeMainVariation();

    void newGame();

    void nextVariation();

    void nextPiece();

    void nextTransform();

    void open();

    void open(const QString& file, bool isTemporary = false);

    void play();

    void previousPiece();

    void previousTransform();

    void previousVariation();

    void quit();

    void rotatePieceAnticlockwise();

    void rotatePieceClockwise();

    void save();

    void saveAs();

    void selectPiece1();

    void selectPiece2();

    void selectPieceF();

    void selectPieceI();

    void selectPieceL();

    void selectPieceN();

    void selectPieceO();

    void selectPieceP();

    void selectPieceT();

    void selectPieceU();

    void selectPieceV();

    void selectPieceW();

    void selectPieceX();

    void selectPieceY();

    void selectPieceZ();

    void selectPiece(Color c, const Piece& piece);

    void selectPiece(Color c, const Piece& piece, Transform transform);

    void setLevel(int level);

    void truncate();

protected:
    void closeEvent(QCloseEvent* event);

    void wheelEvent(QWheelEvent* event);

private:
    struct GenMoveResult
    {
        Color color;

        Move move;

        unsigned int genMoveId;
    };

    /** Possible values for m_level are in 1..maxLevel */
    static const int maxLevel = 6;

    static const int maxRecentFiles = 9;

    unique_ptr<Game> m_game;

    Color m_toPlay;

    unique_ptr<Player> m_player;

    bool m_isGenMoveRunning;

    bool m_lastMoveByComputer;

    /** Flag indicating that the position after the last move played was
        a terminal position. */
    bool m_gameFinished;

    /** Integer ID assigned to the currently running move generation.
        Used to ignore finished events from canceled move generations. */
    unsigned int m_genMoveId;

    bool m_useTimeLimit;

    /** Current playing level of m_player.
        Only use if m_useTimeLimit is false. Possible values for m_level are in
        1..maxLevel. Only used if m_timeLimit is zero. Stored independently of
        the player and set at the player before each move generation, such that
        setting a new level does not require to abort a running move
        generation. */
    int m_level;

    /** Current time limit for m_player.
        Only use if m_useTimeLimit is true. This number means the maximum time
        in seconds per move. Stored independently of the player and set at the
        player before each move generation, such that setting a new level does
        not require to abort a running move generation. */
    int m_timeLimit;

    /** Make m_player use opening book.
        Stored independently of the player and set at the player before each
        move generation, such that setting a new level does not require to
        abort a running move generation. */
    bool m_useBook;

    QToolBar* m_toolBar;

    GuiBoard* m_guiBoard;

    ColorMap<bool> m_computerColor;

    ColorMap<bool> m_noMovesAvailableShown;

    ColorMap<PieceSelector*> m_pieceSelector;

    OrientationDisplay* m_orientationDisplay;

    ScoreDisplay* m_scoreDisplay;

    QSplitter* m_splitter;

    QPlainTextEdit* m_comment;

    HelpWindow* m_help_window;

    QAction* m_actionAbout;

    QAction* m_actionBackward;

    QAction* m_actionBackToMainVariation;

    QAction* m_actionBadMove;

    QAction* m_actionBeginning;

    QAction* m_actionClearSelectedPiece;

    QAction* m_actionComputerColor;

    QAction* m_actionCoordinateLabels;

    QAction* m_actionCustomLevel;

    QAction* m_actionDoubtfulMove;

    QAction* m_actionEnd;

    QAction* m_actionExportAsciiArt;

    QAction* m_actionExportImage;

    QAction* m_actionFindMove;

    QAction* m_actionFlipPieceHorizontally;

    QAction* m_actionFlipPieceVertically;

    QAction* m_actionForward;

    QAction* m_actionFullscreen;

    QAction* m_actionGameInfo;

    QAction* m_actionGameVariantClassic;

    QAction* m_actionGameVariantClassic2;

    QAction* m_actionGameVariantDuo;

    QAction* m_actionGoodMove;

    QAction* m_actionGotoMove;

    QAction* m_actionHelp;

    QAction* m_actionInterestingMove;

    QAction* m_actionInterrupt;

    QAction* m_actionLevel[maxLevel];

    QAction* m_actionMakeMainVariation;

    QAction* m_actionMoveNumbersAll;

    QAction* m_actionMoveNumbersLast;

    QAction* m_actionMoveNumbersNone;

    QAction* m_actionMoveSelectedPieceLeft;

    QAction* m_actionMoveSelectedPieceRight;

    QAction* m_actionMoveSelectedPieceUp;

    QAction* m_actionMoveSelectedPieceDown;

    QAction* m_actionNextPiece;

    QAction* m_actionNextTransform;

    QAction* m_actionNextVariation;

    QAction* m_actionNewGame;

    QAction* m_actionNoMoveAnnotation;

    QAction* m_actionOpen;

    QAction* m_actionPlaceSelectedPiece;

    QAction* m_actionPlay;

    QAction* m_actionPreviousPiece;

    QAction* m_actionPreviousTransform;

    QAction* m_actionPreviousVariation;

    QAction* m_actionQuit;

    QAction* m_actionRecentFile[maxRecentFiles];

    QAction* m_actionRotatePieceAnticlockwise;

    QAction* m_actionRotatePieceClockwise;

    QAction* m_actionSave;

    QAction* m_actionSaveAs;

    QAction* m_actionShowComment;

    QAction* m_actionShowToolbar;

    QAction* m_actionSelectPiece1;

    QAction* m_actionSelectPiece2;

    QAction* m_actionSelectPieceF;

    QAction* m_actionSelectPieceI;

    QAction* m_actionSelectPieceL;

    QAction* m_actionSelectPieceN;

    QAction* m_actionSelectPieceO;

    QAction* m_actionSelectPieceP;

    QAction* m_actionSelectPieceT;

    QAction* m_actionSelectPieceU;

    QAction* m_actionSelectPieceV;

    QAction* m_actionSelectPieceW;

    QAction* m_actionSelectPieceX;

    QAction* m_actionSelectPieceY;

    QAction* m_actionSelectPieceZ;

    QAction* m_actionTruncate;

    QAction* m_actionUseBook;

    QAction* m_actionVeryGoodMove;

    QAction* m_actionVeryBadMove;

    QMenu* m_menuMoveAnnotation;

    QMenu* m_menuOpenRecent;

    QFutureWatcher<GenMoveResult> m_genMoveWatcher;

    QString m_file;

    vector<Move> m_legalMoves;

    unsigned int m_legalMoveIndex;

    GenMoveResult asyncGenMove(Color c, int genMoveId);

    void boardChanged(bool currentNodeChanged);

    bool checkSave();

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

    void cancelGenMove();

    void checkComputerMove();

    void clearStatus();

    bool computerPlaysAll() const;

    void genMove();

    QString getFilter() const;

    QString getLastDir();

    QString getVersion() const;

    void gotoNode(const Node& node);

    void initGame();

    void initGameVariantActions();

    void initPieceSelectors();

    void play(Color c, Move mv);

    void setGameVariant(GameVariant gameVariant);

    void setFile(const QString& file);

    void showError(const QString& message,
                   const QString& infoText = QString());

    void showGameOver();

    void showInfo(const QString& message, const QString& infoText = QString());

    void showMessage(QMessageBox::Icon icon, const QString& text,
                     const QString& infoText);

    void showNoMovesAvailable(Color c);

    QMessageBox::StandardButton showQuestion(const QString& text,
                                         QMessageBox::StandardButtons buttons);

    void updateComment();

    void updateMoveAnnotationActions();

    void updateRecentFiles();

    void updateFlipActions();

private slots:
    void badMove(bool checked);

    void commentChanged();

    void coordinateLabels(bool checked);

    void customLevel(bool checked);

    void doubtfulMove(bool checked);

    void fullscreen(bool checked);

    void gameVariantClassic(bool checked);

    void gameVariantClassic2(bool checked);

    void gameVariantDuo(bool checked);

    void genMoveFinished();

    void goodMove(bool checked);

    void interestingMove(bool checked);

    void noMoveAnnotation(bool checked);

    void openRecentFile();

    void selectNamedPiece(const char* name1, const char* name2 = 0,
                          const char* name3 = 0);

    void setLevel(bool checked);

    void setMoveNumbersAll(bool checked);

    void setMoveNumbersLast(bool checked);

    void setMoveNumbersNone(bool checked);

    void showComment(bool checked);

    void showStatus(const QString& text, bool temporary = false);

    void splitterMoved(int pos, int index);

    void useBook(bool checked);

    void veryBadMove(bool checked);

    void veryGoodMove(bool checked);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_MAIN_WINDOW_H
