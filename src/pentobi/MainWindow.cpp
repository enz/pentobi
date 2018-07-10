//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MainWindow.h"

#include <algorithm>
#include <fstream>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextCodec>
#include <QToolBar>
#include <QToolButton>
#include <QtConcurrentRun>
#include "AnalyzeGameWindow.h"
#include "AnalyzeSpeedDialog.h"
#include "RatingDialog.h"
#include "ShowMessage.h"
#include "Util.h"
#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_util/Assert.h"
#include "libpentobi_base/TreeUtil.h"
#include "libpentobi_base/PentobiTreeWriter.h"
#include "libpentobi_gui/ComputerColorDialog.h"
#include "libpentobi_gui/GameInfoDialog.h"
#include "libpentobi_gui/GuiBoard.h"
#include "libpentobi_gui/GuiBoardUtil.h"
#include "libpentobi_gui/HelpWindow.h"
#include "libpentobi_gui/InitialRatingDialog.h"
#include "libpentobi_gui/LeaveFullscreenButton.h"
#include "libpentobi_gui/OrientationDisplay.h"
#include "libpentobi_gui/PieceSelector.h"
#include "libpentobi_gui/SameHeightLayout.h"
#include "libpentobi_gui/ScoreDisplay.h"
#include "libpentobi_gui/Util.h"

using Util::getPlayerString;
using libboardgame_sgf::SgfError;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::back_to_main_variation;
using libboardgame_sgf::beginning_of_branch;
using libboardgame_sgf::find_next_comment;
using libboardgame_sgf::get_last_node;
using libboardgame_sgf::get_variation_string;
using libboardgame_sgf::has_comment;
using libboardgame_sgf::has_earlier_variation;
using libboardgame_sgf::is_main_variation;
using libboardgame_util::clear_abort;
using libboardgame_util::get_abort;
using libboardgame_util::set_abort;
using libboardgame_util::trim_right;
using libpentobi_base::BoardType;
using libpentobi_base::PieceSet;
using libpentobi_base::PentobiTree;
using libpentobi_base::PentobiTreeWriter;
using libpentobi_base::ScoreType;
using libpentobi_base::get_move_node;
using libpentobi_base::get_move_number;
using libpentobi_base::get_moves_left;
using libpentobi_base::get_position_info;

//-----------------------------------------------------------------------------

namespace {

/** Create a button for manipulating piece orientation. */
QToolButton* createOBoxToolButton(QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setAutoRaise(true);
    // No focus, there are faster keyboard shortcuts for manipulating pieces
    button->setFocusPolicy(Qt::NoFocus);
    // For some reason, toolbuttons are very small in Ubuntu Unity if outside
    // a toolbar (tested with Ubuntu 15.10)
    button->setMinimumSize(28, 28);
    return button;
}

/** Return auto-save file name as a native path name. */
QString getAutoSaveFile()
{
    return Util::getDataDir() + QDir::separator() + "autosave.blksgf";
}

bool hasCurrentVariationOtherMoves(const PentobiTree& tree,
                                   const SgfNode& current)
{
    auto node = current.get_parent_or_null();
    while (node != nullptr)
    {
        if (tree.has_move(*node))
            return true;
        node = node->get_parent_or_null();
    }
    node = current.get_first_child_or_null();
    while (node != nullptr)
    {
        if (tree.has_move(*node))
            return true;
        node = node->get_first_child_or_null();
    }
    return false;
}

void initToolBarText(QToolBar* toolBar)
{
    QSettings settings;
    auto toolBarText = settings.value("toolbar_text", "system").toString();
    if (toolBarText == "no_text")
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    else if (toolBarText == "beside_icons")
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    else if (toolBarText == "below_icons")
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    else if (toolBarText == "text_only")
        toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    else
        toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
}

void setIcon(QAction* action, const QString& name)
{
    QIcon icon(QString(":/pentobi/icons/%1.png").arg(name));
    QString file16 = QString(":/pentobi/icons/%1-16.png").arg(name);
    if (QFile::exists(file16))
        icon.addFile(file16, QSize(16, 16));
    action->setIcon(icon);
}

/** Simple heuristic that prefers moves with more score points.
    Used for sorting the list used in Find Move. */
ScoreType getHeuristic(const Board& bd, Move mv)
{
    return bd.get_piece_info(bd.get_move_piece(mv)).get_score_points();
}

} // namespace

//-----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent, Variant variant,
                       const QString& initialFile, const QString& helpDir,
                       unsigned maxLevel, const QString& booksDir, bool noBook,
                       unsigned nuThreads)
    : QMainWindow(parent),
      m_game(variant),
      m_bd(m_game.get_board()),
      m_helpDir(helpDir)
{
    if (maxLevel > Player::max_supported_level)
        maxLevel = Player::max_supported_level;
    if (maxLevel < 1)
        maxLevel = 1;
    m_maxLevel = maxLevel;
    Util::initDataDir();
    QSettings settings;
    m_history = make_unique<RatingHistory>(variant);
    createActions();
    restoreLevel(variant);
    setCentralWidget(createCentralWidget());
    initPieceSelectors();
    m_moveNumber = new QLabel;
    statusBar()->addPermanentWidget(m_moveNumber);
    m_setupModeLabel = new QLabel(tr("Setup mode"));
    statusBar()->addWidget(m_setupModeLabel);
    m_setupModeLabel->hide();
    m_ratedGameLabelText = new QLabel(tr("Rated game"));
    statusBar()->addWidget(m_ratedGameLabelText);
    m_ratedGameLabelText->hide();
    initGame();
    m_player =
            make_unique<Player>(variant, maxLevel,
                                booksDir.toLocal8Bit().constData(), nuThreads);
    m_player->get_search().set_callback(bind(&MainWindow::searchCallback,
                                             this, placeholders::_1,
                                             placeholders::_2));
    m_player->set_use_book(! noBook);
    createToolBar();
    connect(&m_genMoveWatcher, &QFutureWatcher<GenMoveResult>::finished,
            this, &MainWindow::genMoveFinished);
    connect(m_guiBoard, &GuiBoard::play, this, &MainWindow::placePiece);
    connect(m_guiBoard, &GuiBoard::pointClicked, this, &MainWindow::pointClicked);
    connect(m_actionMovePieceLeft, &QAction::triggered,
            m_guiBoard, &GuiBoard::movePieceLeft);
    connect(m_actionMovePieceRight, &QAction::triggered,
            m_guiBoard, &GuiBoard::movePieceRight);
    connect(m_actionMovePieceUp, &QAction::triggered,
            m_guiBoard, &GuiBoard::movePieceUp);
    connect(m_actionMovePieceDown, &QAction::triggered,
            m_guiBoard, &GuiBoard::movePieceDown);
    connect(m_actionPlacePiece, &QAction::triggered,
            m_guiBoard, &GuiBoard::placePiece);
    createMenu();
    qApp->installEventFilter(this);
    updateRecentFiles();
    auto marking = settings.value("move_marking", "last_dot").toString();
    if (marking == "all_number")
        m_actionMoveMarkingAllNumber->setChecked(true);
    else if (marking == "last_dot")
        m_actionMoveMarkingLastDot->setChecked(true);
    else if (marking == "last_number")
        m_actionMoveMarkingLastNumber->setChecked(true);
    else
        m_actionMoveMarkingNone->setChecked(true);
    auto coordinates = settings.value("coordinates", false).toBool();
    m_guiBoard->setCoordinates(coordinates);
    m_actionCoordinates->setChecked(coordinates);
    auto showToolbar = settings.value("toolbar", true).toBool();
    findChild<QToolBar*>()->setVisible(showToolbar);
    m_menuToolBarText->setEnabled(showToolbar);
    m_actionShowToolbar->setChecked(showToolbar);
    auto showVariations = settings.value("show_variations", true).toBool();
    m_actionShowVariations->setChecked(showVariations);
    initVariantActions();
    QIcon icon;
    icon.addFile(":/pentobi/icons/pentobi.png");
    icon.addFile(":/pentobi/icons/pentobi-16.png");
    icon.addFile(":/pentobi/icons/pentobi-32.png");
    setWindowIcon(icon);

    restoreGeometry(settings.value("geometry").toByteArray());
    auto showComment = settings.value("show_comment", false).toBool();
    m_comment->setVisible(showComment);
    if (showComment)
        m_splitter->restoreState(
                               settings.value("splitter_state").toByteArray());
    m_actionShowComment->setChecked(showComment);
    updateWindow(true);
    clearFile();
    if (! initialFile.isEmpty())
    {
        if (openFile(initialFile))
            rememberFile(initialFile);
    }
    else
    {
        QString autoSaveFile = getAutoSaveFile();
        if (QFile(autoSaveFile).exists())
        {
            openFile(autoSaveFile, true);
            m_isAutoSaveLoaded = true;
            deleteAutoSaveFile();
            m_gameFinished = m_bd.is_game_over();
            updateWindow(true);
            if (settings.value("autosave_rated", false).toBool())
                QMetaObject::invokeMethod(this, "continueRatedGame",
                                          Qt::QueuedConnection);
        }
    }
}

MainWindow::~MainWindow() = default; // Non-inline to avoid GCC -Winline warning

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Pentobi"),
                       "<style type=\"text/css\">"
                       ":link { text-decoration: none; }"
                       "</style>"
                       "<h2>" + tr("Pentobi") + "</h2>"
                       "<p>" + tr("Version %1").arg(getVersion()) + "</p>"
                       "<p>" +
                       tr("Computer opponent for the board game Blokus.")
                       + "<br>" +
                       tr("&copy; 2011&ndash;%1 Markus Enzenberger").arg(2018) +
                       + "<br>" +
                       "<a href=\"https://pentobi.sourceforge.io\">https://pentobi.sourceforge.io</a>"
                       "</p>");
}

void MainWindow::analyzeGame()
{
    if (! is_main_variation(m_game.get_current()))
    {
        showInfo(tr("Game analysis is only possible in the main variation."));
        return;
    }
    AnalyzeSpeedDialog dialog(this, tr("Analyze Game"));
    if (dialog.exec() == 0)
        return;
    int speed = dialog.getSpeedValue();
    cancelThread();
    delete m_analyzeGameWindow;
    m_analyzeGameWindow = new AnalyzeGameWindow(this);
    // Make sure all action shortcuts work when the analyze dialog has the
    // focus apart from m_actionLeaveFullscreen because the Esc key is used
    // to close the dialog
    m_analyzeGameWindow->addActions(actions());
    m_analyzeGameWindow->removeAction(m_actionLeaveFullscreen);
    m_analyzeGameWindow->show();
    m_isAnalyzeRunning = true;
    connect(m_analyzeGameWindow->analyzeGameWidget, &AnalyzeGameWidget::finished,
            this, &MainWindow::analyzeGameFinished);
    connect(m_analyzeGameWindow->analyzeGameWidget, &AnalyzeGameWidget::gotoPosition,
            this, &MainWindow::gotoPosition);
    size_t nuSimulations;
    switch (speed)
    {
    case 0:
        nuSimulations = 6000;
        break;
    case 1:
        nuSimulations = 24000;
        break;
    default:
        nuSimulations = 96000;
    }
    m_analyzeGameWindow->analyzeGameWidget->start(
                m_game, m_player->get_search(), nuSimulations);
}

void MainWindow::analyzeGameFinished()
{
    m_analyzeGameWindow->analyzeGameWidget->setCurrentPosition(
                m_game, m_game.get_current());
    m_isAnalyzeRunning = false;
}

/** Call to Player::genmove() that runs in a different thread. */
MainWindow::GenMoveResult MainWindow::asyncGenMove(Color c, int genMoveId,
                                                   bool playSingleMove)
{
    QElapsedTimer timer;
    timer.start();
    GenMoveResult result;
    result.playSingleMove = playSingleMove;
    result.color = c;
    result.genMoveId = genMoveId;
    result.move = m_player->genmove(m_bd, c);
    auto elapsed = timer.elapsed();
    // Enforce minimum thinking time of 0.8 sec
    if (elapsed < 800 && ! m_noDelay)
        QThread::msleep(800 - elapsed);
    return result;
}

void MainWindow::badMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_bad_move();
    updateWindow(false);
}

void MainWindow::backward()
{
    gotoNode(m_game.get_current().get_parent_or_null());
}

void MainWindow::backToMainVariation()
{
    gotoNode(back_to_main_variation(m_game.get_current()));
}

void MainWindow::beginning()
{
    gotoNode(m_game.get_root());
}

void MainWindow::beginningOfBranch()
{
    gotoNode(beginning_of_branch(m_game.get_current()));
}

void MainWindow::cancelThread()
{
    if (m_isAnalyzeRunning)
    {
        // This should never happen because AnalyzeGameWindow protects the
        // parent with a modal progress dialog while it is running. However,
        // due to bugs in Unity 2D (tested with Ubuntu 11.04 and 11.10), the
        // global menu can still trigger menu item events.
        m_analyzeGameWindow->analyzeGameWidget->cancel();
    }
    if (! m_isGenMoveRunning)
        return;
    // After waitForFinished() returns, we can be sure that the move generation
    // is no longer running, but we will still receive the finished event.
    // Increasing m_genMoveId will make genMoveFinished() ignore the event.
    ++m_genMoveId;
    set_abort();
    m_genMoveWatcher.waitForFinished();
    m_isGenMoveRunning = false;
    clearStatus();
    setCursor(QCursor(Qt::ArrowCursor));
    m_actionInterrupt->setEnabled(false);
    m_actionNextPiece->setEnabled(true);
    m_actionPlay->setEnabled(true);
    m_actionPlaySingleMove->setEnabled(true);
    m_actionPreviousPiece->setEnabled(true);
}

void MainWindow::checkComputerMove()
{
    if (m_autoPlay && isComputerToPlay() && ! m_bd.is_game_over()
            && ! m_isGenMoveRunning)
        genMove();
}

bool MainWindow::checkSave()
{
    if (! m_file.isEmpty())
    {
        if (! m_game.is_modified())
            return true;
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The file has been modified."),
                     tr("Do you want to save your changes?"));
        // Don't use QMessageBox::Discard because on some platforms it uses the
        // text "Close without saving" which implies that the window would be
        // closed
        auto discardButton =
            msgBox.addButton(tr("&Don't Save"), QMessageBox::DestructiveRole);
        auto saveButton = msgBox.addButton(QMessageBox::Save);
        auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        auto result = msgBox.clickedButton();
        if (result == saveButton)
        {
            save();
            return true;
        }
        return result == discardButton;
    }
    // Don't ask if game should be saved if it was finished because the user
    // might only want to play and never save games.
    if (m_game.get_root().has_children() && ! m_gameFinished)
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The current game is not finished."),
                     tr("Do you want to abort the game?"));
        auto abortGameButton =
            msgBox.addButton(tr("&Abort Game"), QMessageBox::DestructiveRole);
        auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        return msgBox.clickedButton() == abortGameButton;
    }
    return true;
}

bool MainWindow::checkQuit()
{
    if (! m_file.isEmpty() && m_game.is_modified())
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The file has been modified."),
                     tr("Do you want to save your changes?"));
        auto discardButton = msgBox.addButton(QMessageBox::Discard);
        auto saveButton = msgBox.addButton(QMessageBox::Save);
        auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        auto result = msgBox.clickedButton();
        if (result == saveButton)
        {
            save();
            return true;
        }
        return result == discardButton;
    }
    cancelThread();
    QSettings settings;
    if (m_file.isEmpty() && ! m_gameFinished
            && (m_game.is_modified() || m_isAutoSaveLoaded))
    {
        writeGame(getAutoSaveFile().toLocal8Bit().constData());
        settings.setValue("autosave_rated", m_isRated);
        if (m_isRated)
            settings.setValue("autosave_rated_color",
                              m_ratedGameColor.to_int());
    }
    if (! isFullScreen())
        settings.setValue("geometry", saveGeometry());
    if (m_comment->isVisible())
        settings.setValue("splitter_state", m_splitter->saveState());
    return true;
}

void MainWindow::clearFile()
{
    setFile("");
}

void MainWindow::clearPiece()
{
    m_actionRotateClockwise->setEnabled(false);
    m_actionRotateAnticlockwise->setEnabled(false);
    m_actionFlipHorizontally->setEnabled(false);
    m_actionFlipVertically->setEnabled(false);
    m_actionClearPiece->setEnabled(false);
    m_guiBoard->clearPiece();
    m_orientationDisplay->clearPiece();
}

void MainWindow::clearStatus()
{
    statusBar()->clearMessage();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (checkQuit())
        event->accept();
    else
        event->ignore();
}

void MainWindow::commentChanged()
{
    if (m_ignoreCommentTextChanged)
        return;
    QString comment = m_comment->toPlainText();
    if (comment.isEmpty())
        m_game.set_comment("");
    else
    {
        string value = encode(comment).constData();
        value = trim_right(value);
        m_game.set_comment(value);
    }
    updateWindow(false);
}

void MainWindow::computerColors()
{
    ColorMap<bool> oldComputerColors = m_computerColors;
    ComputerColorDialog dialog(this, m_bd.get_variant(), m_computerColors);
    dialog.exec();
    auto nu_colors = m_bd.get_nu_nonalt_colors();

    bool computerNone = true;
    for (Color c : Color::Range(nu_colors))
        if (m_computerColors[c])
        {
            computerNone = false;
            break;
        }
    QSettings settings;
    settings.setValue("computer_color_none", computerNone);

    // Enable autoplay only if any color has changed because that means that
    // the user probably wants to continue playing, otherwise the user could
    // have only opened the dialog to check the current settings
    for (Color c : Color::Range(nu_colors))
        if (m_computerColors[c] != oldComputerColors[c])
        {
            m_autoPlay = true;
            break;
        }

    if (m_isGenMoveRunning && ! isComputerToPlay())
        cancelThread();
    else
        checkComputerMove();
}

bool MainWindow::computerPlaysAll() const
{
    for (Color c : Color::Range(m_bd.get_nu_nonalt_colors()))
        if (! m_computerColors[c])
            return false;
    return true;
}

void MainWindow::continueRatedGame()
{
    auto nuColors = m_bd.get_nu_colors();
    QSettings settings;
    auto color =
            static_cast<Color::IntType>(
                settings.value("autosave_rated_color", 0).toUInt());
    if (color >= nuColors)
        return;
    m_ratedGameColor = Color(color);
    m_computerColors.fill(true);
    for (Color c : Color::Range(nuColors))
        if (m_bd.is_same_player(c, m_ratedGameColor))
            m_computerColors[c] = false;
    setRated(true);
    updateWindow(false);
    showInfo(tr("Continuing unfinished rated game."),
             tr("You play %1 in this game.")
             .arg(getPlayerString(m_bd.get_variant(), m_ratedGameColor)));
    m_autoPlay = true;
    checkComputerMove();
}

void MainWindow::coordinates(bool checked)
{
    m_guiBoard->setCoordinates(checked);
    QSettings settings;
    settings.setValue("coordinates", checked);
}

QAction* MainWindow::createAction(const QString& text)
{
    auto action = new QAction(text, this);
    // Add all actions also to main window. if an action is only added to
    // the menu bar, shortcuts won't work in fullscreen mode because the menu
    // is not visible in fullscreen mode
    addAction(action);
    return action;
}

QAction* MainWindow::createActionLevel(unsigned level, const QString& text)
{
    auto action = createAction(text);
    action->setCheckable(true);
    action->setActionGroup(m_actionGroupLevel);
    action->setData(level);
    connect(action, &QAction::triggered, this, &MainWindow::levelTriggered);
    return action;
}

void MainWindow::createActions()
{
    m_actionGroupVariant = new QActionGroup(this);
    m_actionGroupLevel = new QActionGroup(this);
    auto groupMoveMarking = new QActionGroup(this);
    auto groupMoveAnnotation = new QActionGroup(this);
    auto groupToolBarText = new QActionGroup(this);

    m_actionAbout = createAction(tr("&About Pentobi"));
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::about);

    m_actionAnalyzeGame = createAction(tr("&Analyze Game..."));
    connect(m_actionAnalyzeGame, &QAction::triggered, this, &MainWindow::analyzeGame);

    m_actionBackward = createAction(tr("B&ackward"));
    m_actionBackward->setToolTip(tr("Go one move backward"));
    m_actionBackward->setPriority(QAction::LowPriority);
    setIcon(m_actionBackward, "pentobi-backward");
    m_actionBackward->setShortcut(QKeySequence::MoveToPreviousWord);
    connect(m_actionBackward, &QAction::triggered, this, &MainWindow::backward);

    m_actionBackToMainVariation = createAction(tr("Back to &Main Variation"));
    m_actionBackToMainVariation->setShortcut(QString("Ctrl+M"));
    connect(m_actionBackToMainVariation, &QAction::triggered, this, &MainWindow::backToMainVariation);

    m_actionBadMove = createAction(tr("&Bad"));
    m_actionBadMove->setActionGroup(groupMoveAnnotation);
    m_actionBadMove->setCheckable(true);
    connect(m_actionBadMove, &QAction::triggered, this, &MainWindow::badMove);

    m_actionBeginning = createAction(tr("&Beginning"));
    m_actionBeginning->setToolTip(tr("Go to beginning of game"));
    m_actionBeginning->setPriority(QAction::LowPriority);
    setIcon(m_actionBeginning, "pentobi-beginning");
    m_actionBeginning->setShortcut(QKeySequence::MoveToStartOfDocument);
    connect(m_actionBeginning, &QAction::triggered, this, &MainWindow::beginning);

    m_actionBeginningOfBranch = createAction(tr("Beginning of Bran&ch"));
    m_actionBeginningOfBranch->setShortcut(QString("Ctrl+B"));
    connect(m_actionBeginningOfBranch, &QAction::triggered, this, &MainWindow::beginningOfBranch);

    m_actionClearPiece = createAction(tr("Clear Piece"));
    setIcon(m_actionClearPiece, "pentobi-piece-clear");
    m_actionClearPiece->setShortcut(QString("0"));
    connect(m_actionClearPiece, &QAction::triggered, this, &MainWindow::clearPiece);

    m_actionComputerColors = createAction(tr("&Computer Colors"));
    m_actionComputerColors->setShortcut(QString("Ctrl+U"));
    m_actionComputerColors->setToolTip(
                                  tr("Set the colors played by the computer"));
    setIcon(m_actionComputerColors, "pentobi-computer-colors");
    connect(m_actionComputerColors, &QAction::triggered, this, &MainWindow::computerColors);

    m_actionCoordinates = createAction(tr("C&oordinates"));
    m_actionCoordinates->setCheckable(true);
    connect(m_actionCoordinates, &QAction::triggered, this, &MainWindow::coordinates);

    m_actionDeleteAllVariations = createAction(tr("&Delete All Variations"));
    connect(m_actionDeleteAllVariations, &QAction::triggered, this, &MainWindow::deleteAllVariations);

    m_actionDoubtfulMove = createAction(tr("&Doubtful"));
    m_actionDoubtfulMove->setActionGroup(groupMoveAnnotation);
    m_actionDoubtfulMove->setCheckable(true);
    connect(m_actionDoubtfulMove, &QAction::triggered, this, &MainWindow::doubtfulMove);

    m_actionEnd = createAction(tr("&End"));
    m_actionEnd->setToolTip(tr("Go to end of moves"));
    m_actionEnd->setPriority(QAction::LowPriority);
    m_actionEnd->setShortcut(QKeySequence::MoveToEndOfDocument);
    setIcon(m_actionEnd, "pentobi-end");
    connect(m_actionEnd, &QAction::triggered, this, &MainWindow::end);

    m_actionExportAsciiArt = createAction(tr("&ASCII Art"));
    connect(m_actionExportAsciiArt, &QAction::triggered, this, &MainWindow::exportAsciiArt);

    m_actionExportImage = createAction(tr("I&mage"));
    connect(m_actionExportImage, &QAction::triggered, this, &MainWindow::exportImage);

    m_actionFindMove = createAction(tr("&Find Move"));
    m_actionFindMove->setShortcut(QString("F6"));
    connect(m_actionFindMove, &QAction::triggered, this, &MainWindow::findMove);

    m_actionFindNextComment = createAction(tr("Find Next &Comment"));
    m_actionFindNextComment->setShortcut(QString("F3"));
    connect(m_actionFindNextComment, &QAction::triggered, this, &MainWindow::findNextComment);

    m_actionFlipHorizontally = createAction(tr("Flip Horizontally"));
    setIcon(m_actionFlipHorizontally, "pentobi-flip-horizontal");
    connect(m_actionFlipHorizontally, &QAction::triggered, this, &MainWindow::flipHorizontally);

    m_actionFlipVertically = createAction(tr("Flip Vertically"));
    setIcon(m_actionFlipVertically, "pentobi-flip-vertical");

    m_actionForward = createAction(tr("&Forward"));
    m_actionForward->setToolTip(tr("Go one move forward"));
    m_actionForward->setPriority(QAction::LowPriority);
    m_actionForward->setShortcut(QKeySequence::MoveToNextWord);
    setIcon(m_actionForward, "pentobi-forward");
    connect(m_actionForward, &QAction::triggered, this, &MainWindow::forward);

    m_actionFullscreen = createAction(tr("&Fullscreen"));
    // Don't use QKeySequence::Fullscreen, it is Ctrl-F11 on Linux but that
    // doesn't work in Xfce
    m_actionFullscreen->setShortcut(QString("F11"));
    connect(m_actionFullscreen, &QAction::triggered, this, &MainWindow::fullscreen);

    m_actionGameInfo = createAction(tr("Ga&me Info"));
    m_actionGameInfo->setShortcut(QString("Ctrl+I"));
    connect(m_actionGameInfo, &QAction::triggered, this, &MainWindow::gameInfo);

    m_actionGoodMove = createAction(tr("&Good"));
    m_actionGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionGoodMove->setCheckable(true);
    connect(m_actionGoodMove, &QAction::triggered, this, &MainWindow::goodMove);

    m_actionGotoMove = createAction(tr("&Go to Move..."));
    m_actionGotoMove->setShortcut(QString("Ctrl+G"));
    connect(m_actionGotoMove, &QAction::triggered, this, &MainWindow::gotoMove);

    m_actionHelp = createAction(tr("Pentobi &Help"));
    m_actionHelp->setShortcut(QKeySequence::HelpContents);
    connect(m_actionHelp, &QAction::triggered, this, &MainWindow::help);

    m_actionInterestingMove = createAction(tr("I&nteresting"));
    m_actionInterestingMove->setActionGroup(groupMoveAnnotation);
    m_actionInterestingMove->setCheckable(true);
    connect(m_actionInterestingMove, &QAction::triggered, this, &MainWindow::interestingMove);

    m_actionInterrupt = createAction(tr("St&op"));
    m_actionInterrupt->setEnabled(false);
    connect(m_actionInterrupt, &QAction::triggered, this, &MainWindow::interrupt);

    m_actionInterruptPlay = createAction();
    m_actionInterruptPlay->setShortcut(QString("Shift+Esc"));
    connect(m_actionInterruptPlay, &QAction::triggered, this, &MainWindow::interruptPlay);

    m_actionKeepOnlyPosition = createAction(tr("&Keep Only Position"));
    connect(m_actionKeepOnlyPosition, &QAction::triggered, this, &MainWindow::keepOnlyPosition);

    m_actionKeepOnlySubtree = createAction(tr("Keep Only &Subtree"));
    connect(m_actionKeepOnlySubtree, &QAction::triggered, this, &MainWindow::keepOnlySubtree);

    m_actionLeaveFullscreen = createAction(tr("Leave Fullscreen"));
    m_actionLeaveFullscreen->setShortcut(QString("Esc"));
    connect(m_actionLeaveFullscreen, &QAction::triggered, this, &MainWindow::leaveFullscreen);

    m_actionMakeMainVariation = createAction(tr("M&ake Main Variation"));
    connect(m_actionMakeMainVariation, &QAction::triggered, this, &MainWindow::makeMainVariation);

    m_actionMoveDownVariation = createAction(tr("Move Variation D&own"));
    connect(m_actionMoveDownVariation, &QAction::triggered, this, &MainWindow::moveDownVariation);

    m_actionMoveUpVariation = createAction(tr("Move Variation &Up"));
    connect(m_actionMoveUpVariation, &QAction::triggered, this, &MainWindow::moveUpVariation);

    static_assert(Player::max_supported_level <= 9, "");
    QString levelText[Player::max_supported_level] =
        { tr("&1"), tr("&2"), tr("&3"), tr("&4"), tr("&5"), tr("&6"),
          tr("&7"), tr("&8"), tr("&9") };
    for (unsigned i = 0; i < m_maxLevel; ++i)
        createActionLevel(i + 1, levelText[i]);
    connect(m_actionFlipVertically, &QAction::triggered, this, &MainWindow::flipVertically);

    m_actionMoveMarkingAllNumber = createAction(tr("&All with Number"));
    m_actionMoveMarkingAllNumber->setActionGroup(groupMoveMarking);
    m_actionMoveMarkingAllNumber->setCheckable(true);
    connect(m_actionMoveMarkingAllNumber, &QAction::triggered, this, &MainWindow::setMoveMarkingAllNumber);

    m_actionMoveMarkingLastDot = createAction(tr("Last with &Dot"));
    m_actionMoveMarkingLastDot->setActionGroup(groupMoveMarking);
    m_actionMoveMarkingLastDot->setCheckable(true);
    m_actionMoveMarkingLastDot->setChecked(true);
    connect(m_actionMoveMarkingLastDot, &QAction::triggered, this, &MainWindow::setMoveMarkingLastDot);

    m_actionMoveMarkingLastNumber = createAction(tr("&Last with Number"));
    m_actionMoveMarkingLastNumber->setActionGroup(groupMoveMarking);
    m_actionMoveMarkingLastNumber->setCheckable(true);
    m_actionMoveMarkingLastNumber->setChecked(true);
    connect(m_actionMoveMarkingLastNumber, &QAction::triggered, this, &MainWindow::setMoveMarkingLastNumber);

    m_actionMoveMarkingNone = createAction(tr("&None", "move numbers"));
    m_actionMoveMarkingNone->setActionGroup(groupMoveMarking);
    m_actionMoveMarkingNone->setCheckable(true);
    connect(m_actionMoveMarkingNone, &QAction::triggered, this, &MainWindow::setMoveMarkingNone);

    m_actionMovePieceLeft = createAction();
    m_actionMovePieceLeft->setShortcut(QKeySequence::MoveToPreviousChar);

    m_actionMovePieceRight = createAction();
    m_actionMovePieceRight->setShortcut(QKeySequence::MoveToNextChar);

    m_actionMovePieceUp = createAction();
    m_actionMovePieceUp->setShortcut(QKeySequence::MoveToPreviousLine);

    m_actionMovePieceDown = createAction();
    m_actionMovePieceDown->setShortcut(QKeySequence::MoveToNextLine);

    m_actionNextPiece = createAction(tr("Next Piece"));
    setIcon(m_actionNextPiece, "pentobi-next-piece");
    m_actionNextPiece->setShortcut(QString("+"));
    connect(m_actionNextPiece, &QAction::triggered, this, &MainWindow::nextPiece);

    m_actionNextTransform = createAction();
    m_actionNextTransform->setShortcut(QString("Space"));
    connect(m_actionNextTransform, &QAction::triggered, this, &MainWindow::nextTransform);

    m_actionNextVariation = createAction(tr("&Next Variation"));
    m_actionNextVariation->setToolTip(tr("Go to next variation"));
    m_actionNextVariation->setPriority(QAction::LowPriority);
    m_actionNextVariation->setShortcut(QKeySequence::MoveToNextPage);
    setIcon(m_actionNextVariation, "pentobi-next-variation");
    connect(m_actionNextVariation, &QAction::triggered, this, &MainWindow::nextVariation);

    m_actionNew = createAction(tr("&New"));
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setToolTip(tr("Start a new game"));
    setIcon(m_actionNew, "pentobi-newgame");
    connect(m_actionNew, &QAction::triggered, this, &MainWindow::newGame);

    m_actionNoMoveAnnotation = createAction(tr("N&one", "move annotation"));
    m_actionNoMoveAnnotation->setActionGroup(groupMoveAnnotation);
    m_actionNoMoveAnnotation->setCheckable(true);
    connect(m_actionNoMoveAnnotation, &QAction::triggered, this, &MainWindow::noMoveAnnotation);

    m_actionOpen = createAction(tr("&Open..."));
    m_actionOpen->setShortcut(QKeySequence::Open);
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::open);

    m_actionOpenFromClipboard = createAction(tr("Open from &Clipboard"));
    connect(m_actionOpenFromClipboard, &QAction::triggered, this, &MainWindow::openFromClipboard);

    m_actionPlacePiece = createAction();
    m_actionPlacePiece->setShortcut(QString("Return"));

    m_actionPlay = createAction(tr("&Play"));
    m_actionPlay->setShortcut(QString("Ctrl+L"));
    setIcon(m_actionPlay, "pentobi-play");
    connect(m_actionPlay, &QAction::triggered, this, &MainWindow::play);

    m_actionPlaySingleMove = createAction(tr("Play &Single Move"));
    m_actionPlaySingleMove->setShortcut(QString("Ctrl+Shift+L"));
    connect(m_actionPlaySingleMove, &QAction::triggered, this, &MainWindow::playSingleMove);

    m_actionPreviousPiece = createAction(tr("Previous Piece"));
    setIcon(m_actionPreviousPiece, "pentobi-previous-piece");
    m_actionPreviousPiece->setShortcut(QString("-"));
    connect(m_actionPreviousPiece, &QAction::triggered, this, &MainWindow::previousPiece);

    m_actionPreviousTransform = createAction();
    m_actionPreviousTransform->setShortcut(QString("Shift+Space"));
    connect(m_actionPreviousTransform, &QAction::triggered, this, &MainWindow::previousTransform);

    m_actionPreviousVariation = createAction(tr("&Previous Variation"));
    m_actionPreviousVariation->setToolTip(tr("Go to previous variation"));
    m_actionPreviousVariation->setPriority(QAction::LowPriority);
    m_actionPreviousVariation->setShortcut(QKeySequence::MoveToPreviousPage);
    setIcon(m_actionPreviousVariation, "pentobi-previous-variation");
    connect(m_actionPreviousVariation, &QAction::triggered, this, &MainWindow::previousVariation);

    m_actionRatedGame = createAction(tr("&Rated Game"));
    m_actionRatedGame->setToolTip(tr("Start a rated game"));
    m_actionRatedGame->setShortcut(QString("Ctrl+Shift+N"));
    setIcon(m_actionRatedGame, "pentobi-rated-game");
    connect(m_actionRatedGame, &QAction::triggered, this, &MainWindow::ratedGame);

    for (auto& action : m_actionRecentFile)
    {
         action = createAction();
         action->setVisible(false);
         connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    m_actionRotateAnticlockwise = createAction(tr("Rotate Anticlockwise"));
    setIcon(m_actionRotateAnticlockwise, "pentobi-rotate-left");
    connect(m_actionRotateAnticlockwise, &QAction::triggered, this, &MainWindow::rotateAnticlockwise);

    m_actionRotateClockwise = createAction(tr("Rotate Clockwise"));
    setIcon(m_actionRotateClockwise, "pentobi-rotate-right");
    connect(m_actionRotateClockwise, &QAction::triggered, this, &MainWindow::rotateClockwise);

    m_actionQuit = createAction(tr("&Quit"));
    m_actionQuit->setShortcuts({QKeySequence::Quit, QKeySequence::Close});
    connect(m_actionQuit, &QAction::triggered, this, &MainWindow::close);

    m_actionSave = createAction(tr("&Save"));
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::save);

    m_actionSaveAs = createAction(tr("Save &As..."));
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);

    m_actionNextColor = createAction(tr("Next &Color"));
    connect(m_actionNextColor, &QAction::triggered, this, &MainWindow::nextColor);

    for (auto name : { "1", "2", "A", "C", "E", "F", "G", "H", "I", "J", "L",
                       "N", "O", "P", "S", "T", "U", "V", "W", "X", "Y", "Z" })
    {
        auto action = createAction();
        action->setData(name);
        action->setShortcut(QString(name));
        connect(action, &QAction::triggered, this, &MainWindow::selectNamedPiece);
    }

    m_actionSetupMode = createAction(tr("S&etup Mode"));
    m_actionSetupMode->setCheckable(true);
    connect(m_actionSetupMode, &QAction::triggered, this, &MainWindow::setupMode);

    m_actionShowComment = createAction(tr("&Comment"));
    m_actionShowComment->setCheckable(true);
    m_actionShowComment->setShortcut(QString("Ctrl+T"));
    connect(m_actionShowComment, &QAction::triggered, this, &MainWindow::showComment);

    //: Menu label for opening the rating dialog.
    m_actionRating = createAction(tr("&Rating"));
    m_actionRating->setShortcut(QString("F7"));
    connect(m_actionRating, &QAction::triggered, this, &MainWindow::showRating);

    m_actionToolBarNoText = createAction(tr("&No Text"));
    m_actionToolBarNoText->setActionGroup(groupToolBarText);
    m_actionToolBarNoText->setCheckable(true);
    connect(m_actionToolBarNoText, &QAction::triggered, [this](bool checked) {
        if (checked)
            toolBarText("no_text", Qt::ToolButtonIconOnly);
    });

    m_actionToolBarTextBesideIcons = createAction(tr("Text &Beside Icons"));
    m_actionToolBarTextBesideIcons->setActionGroup(groupToolBarText);
    m_actionToolBarTextBesideIcons->setCheckable(true);
    connect(m_actionToolBarTextBesideIcons, &QAction::triggered, [this](bool checked) {
        if (checked)
            toolBarText("beside_icons", Qt::ToolButtonTextBesideIcon);
    });

    m_actionToolBarTextBelowIcons = createAction(tr("Text Bel&ow Icons"));
    m_actionToolBarTextBelowIcons->setActionGroup(groupToolBarText);
    m_actionToolBarTextBelowIcons->setCheckable(true);
    connect(m_actionToolBarTextBelowIcons, &QAction::triggered, [this](bool checked) {
        if (checked)
            toolBarText("below_icons", Qt::ToolButtonTextUnderIcon);
    });

    m_actionToolBarTextOnly = createAction(tr("&Text Only"));
    m_actionToolBarTextOnly->setActionGroup(groupToolBarText);
    m_actionToolBarTextOnly->setCheckable(true);
    connect(m_actionToolBarTextOnly, &QAction::triggered, [this](bool checked) {
        if (checked)
            toolBarText("text_only", Qt::ToolButtonTextOnly);
    });

    m_actionToolBarTextSystem = createAction(tr("&System Default"));
    m_actionToolBarTextSystem->setActionGroup(groupToolBarText);
    m_actionToolBarTextSystem->setCheckable(true);
    connect(m_actionToolBarTextSystem, &QAction::triggered, [this](bool checked) {
        if (checked)
            toolBarText("system", Qt::ToolButtonFollowStyle);
    });

    m_actionTruncate = createAction(tr("&Truncate"));
    connect(m_actionTruncate, &QAction::triggered, this, &MainWindow::truncate);

    m_actionTruncateChildren = createAction(tr("Truncate C&hildren"));
    connect(m_actionTruncateChildren, &QAction::triggered, this, &MainWindow::truncateChildren);

    m_actionShowToolbar = createAction(tr("&Toolbar"));
    m_actionShowToolbar->setCheckable(true);
    connect(m_actionShowToolbar, &QAction::triggered, this, &MainWindow::showToolbar);

    m_actionShowVariations = createAction(tr("Show &Variations"));
    m_actionShowVariations->setCheckable(true);
    connect(m_actionShowVariations, &QAction::triggered, this, &MainWindow::showVariations);

    m_actionUndo = createAction(tr("&Undo Move"));
    setIcon(m_actionUndo, "pentobi-undo");
    connect(m_actionUndo, &QAction::triggered, this, &MainWindow::undo);

    m_actionVariantCallisto2 =
            createActionVariant(Variant::callisto_2,
                                tr("Callisto (&2 Players, 2 Colors)"));
    m_actionVariantCallisto24 =
            createActionVariant(Variant::callisto_2_4,
                                tr("Callisto (2 &Players, 4 Colors)"));
    m_actionVariantCallisto3 =
            createActionVariant(Variant::callisto_3,
                                tr("Callisto (&3 Players)"));
    m_actionVariantCallisto =
            createActionVariant(Variant::callisto,
                                tr("Callisto (&4 Players)"));
    m_actionVariantClassic2 =
            createActionVariant(Variant::classic_2,
                                tr("Classic (&2 Players)"));
    m_actionVariantClassic3 =
            createActionVariant(Variant::classic_3,
                                tr("Classic (&3 Players)"));
    m_actionVariantClassic =
            createActionVariant(Variant::classic, tr("Classic (&4 Players)"));
    m_actionVariantDuo = createActionVariant(Variant::duo, tr("&Duo"));
    m_actionVariantGembloQ2 =
            createActionVariant(Variant::gembloq_2, tr("GembloQ (&2 Players, 2 Colors)"));
    m_actionVariantGembloQ24 =
            createActionVariant(Variant::gembloq_2_4,
                                tr("GembloQ (2 &Players, 4 Colors)"));
    m_actionVariantGembloQ3 =
            createActionVariant(Variant::gembloq_3, tr("GembloQ (&3 Players)"));
    m_actionVariantGembloQ =
            createActionVariant(Variant::gembloq, tr("GembloQ (&4 Players)"));
    m_actionVariantJunior =
            createActionVariant(Variant::junior, tr("J&unior"));
    m_actionVariantTrigon2 =
            createActionVariant(Variant::trigon_2, tr("Trigon (&2 Players)"));
    m_actionVariantTrigon3 =
            createActionVariant(Variant::trigon_3, tr("Trigon (&3 Players)"));
    m_actionVariantTrigon =
            createActionVariant(Variant::trigon, tr("Trigon (&4 Players)"));
    m_actionVariantNexos2 =
            createActionVariant(Variant::nexos_2, tr("Nexos (&2 Players)"));
    m_actionVariantNexos =
            createActionVariant(Variant::nexos, tr("Nexos (&4 Players)"));

    m_actionVeryBadMove = createAction(tr("V&ery Bad"));
    m_actionVeryBadMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryBadMove->setCheckable(true);
    connect(m_actionVeryBadMove, &QAction::triggered, this, &MainWindow::veryBadMove);

    m_actionVeryGoodMove = createAction(tr("&Very Good"));
    m_actionVeryGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryGoodMove->setCheckable(true);
    connect(m_actionVeryGoodMove, &QAction::triggered, this, &MainWindow::veryGoodMove);
}

QAction* MainWindow::createActionVariant(Variant variant, const QString& text)
{
    auto action = createAction(text);
    action->setCheckable(true);
    action->setActionGroup(m_actionGroupVariant);
    action->setData(static_cast<int>(variant));
    connect(action, &QAction::triggered, this, &MainWindow::variantTriggered);
    return action;
}

QWidget* MainWindow::createCentralWidget()
{
    auto widget = new QWidget;
    // We add spacing around and between the two panels using streches (such
    // that the spacing grows with the window size)
    auto outerLayout = new QVBoxLayout;
    widget->setLayout(outerLayout);
    auto innerLayout = new QHBoxLayout;
    outerLayout->addStretch(1);
    outerLayout->addLayout(innerLayout, 100);
    outerLayout->addStretch(1);
    innerLayout->addStretch(1);
    innerLayout->addWidget(createLeftPanel(), 60);
    innerLayout->addStretch(1);
    innerLayout->addLayout(createRightPanel(), 40);
    innerLayout->addStretch(1);
    // The central widget doesn't do anything with the focus right now, but we
    // allow it to receive the focus such that the user can switch away the
    // focus from the comment field and its blinking cursor.
    widget->setFocusPolicy(Qt::StrongFocus);
    return widget;
}

QWidget* MainWindow::createLeftPanel()
{
    m_splitter = new QSplitter(Qt::Vertical);
    m_guiBoard = new GuiBoard(nullptr, m_bd);
    m_splitter->addWidget(m_guiBoard);
    m_comment = new QPlainTextEdit;
    m_comment->setTabChangesFocus(true);
    connect(m_comment, &QPlainTextEdit::textChanged, this, &MainWindow::commentChanged);
    m_splitter->addWidget(m_comment);
    m_splitter->setStretchFactor(0, 85);
    m_splitter->setStretchFactor(1, 15);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    return m_splitter;
}

void MainWindow::createMenu()
{
    auto menuGame = menuBar()->addMenu(tr("&Game"));
    menuGame->addAction(m_actionNew);
    menuGame->addAction(m_actionRatedGame);
    menuGame->addSeparator();
    m_menuVariant = menuGame->addMenu(tr("Game &Variant"));
    auto menuClassic = m_menuVariant->addMenu(tr("&Classic"));
    menuClassic->addAction(m_actionVariantClassic2);
    menuClassic->addAction(m_actionVariantClassic3);
    menuClassic->addAction(m_actionVariantClassic);
    m_menuVariant->addAction(m_actionVariantDuo);
    m_menuVariant->addAction(m_actionVariantJunior);
    auto menuTrigon = m_menuVariant->addMenu(tr("&Trigon"));
    menuTrigon->addAction(m_actionVariantTrigon2);
    menuTrigon->addAction(m_actionVariantTrigon3);
    menuTrigon->addAction(m_actionVariantTrigon);
    auto menuNexos = m_menuVariant->addMenu(tr("&Nexos"));
    menuNexos->addAction(m_actionVariantNexos2);
    menuNexos->addAction(m_actionVariantNexos);
    auto menuGembloQ = m_menuVariant->addMenu(tr("&GembloQ"));
    menuGembloQ->addAction(m_actionVariantGembloQ2);
    menuGembloQ->addAction(m_actionVariantGembloQ24);
    menuGembloQ->addAction(m_actionVariantGembloQ3);
    menuGembloQ->addAction(m_actionVariantGembloQ);
    auto menuCallisto = m_menuVariant->addMenu(tr("C&allisto"));
    menuCallisto->addAction(m_actionVariantCallisto2);
    menuCallisto->addAction(m_actionVariantCallisto24);
    menuCallisto->addAction(m_actionVariantCallisto3);
    menuCallisto->addAction(m_actionVariantCallisto);
    menuGame->addAction(m_actionGameInfo);
    menuGame->addSeparator();
    menuGame->addAction(m_actionUndo);
    menuGame->addAction(m_actionFindMove);
    menuGame->addSeparator();
    menuGame->addAction(m_actionOpen);
    m_menuOpenRecent = menuGame->addMenu(tr("Open R&ecent"));
    for (auto& action : m_actionRecentFile)
        m_menuOpenRecent->addAction(action);
    menuGame->addAction(m_actionOpenFromClipboard);
    menuGame->addSeparator();
    menuGame->addAction(m_actionSave);
    menuGame->addAction(m_actionSaveAs);
    m_menuExport = menuGame->addMenu(tr("E&xport"));
    m_menuExport->addAction(m_actionExportImage);
    m_menuExport->addAction(m_actionExportAsciiArt);
    menuGame->addSeparator();
    menuGame->addAction(m_actionQuit);

    auto menuGo = menuBar()->addMenu(tr("G&o"));
    menuGo->addAction(m_actionBeginning);
    menuGo->addAction(m_actionBackward);
    menuGo->addAction(m_actionForward);
    menuGo->addAction(m_actionEnd);
    menuGo->addSeparator();
    menuGo->addAction(m_actionNextVariation);
    menuGo->addAction(m_actionPreviousVariation);
    menuGo->addSeparator();
    menuGo->addAction(m_actionGotoMove);
    menuGo->addAction(m_actionBackToMainVariation);
    menuGo->addAction(m_actionBeginningOfBranch);
    menuGo->addSeparator();
    menuGo->addAction(m_actionFindNextComment);

    auto menuEdit = menuBar()->addMenu(tr("&Edit"));
    m_menuMoveAnnotation = menuEdit->addMenu(tr("&Move Annotation"));
    m_menuMoveAnnotation->addAction(m_actionNoMoveAnnotation);
    m_menuMoveAnnotation->addAction(m_actionVeryGoodMove);
    m_menuMoveAnnotation->addAction(m_actionGoodMove);
    m_menuMoveAnnotation->addAction(m_actionInterestingMove);
    m_menuMoveAnnotation->addAction(m_actionDoubtfulMove);
    m_menuMoveAnnotation->addAction(m_actionBadMove);
    m_menuMoveAnnotation->addAction(m_actionVeryBadMove);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionMakeMainVariation);
    menuEdit->addAction(m_actionMoveUpVariation);
    menuEdit->addAction(m_actionMoveDownVariation);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionDeleteAllVariations);
    menuEdit->addAction(m_actionTruncate);
    menuEdit->addAction(m_actionTruncateChildren);
    menuEdit->addAction(m_actionKeepOnlyPosition);
    menuEdit->addAction(m_actionKeepOnlySubtree);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionSetupMode);
    menuEdit->addAction(m_actionNextColor);

    auto menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(m_actionShowToolbar);
    m_menuToolBarText = menuView->addMenu(tr("Toolbar T&ext"));
    m_menuToolBarText->addAction(m_actionToolBarNoText);
    m_menuToolBarText->addAction(m_actionToolBarTextBesideIcons);
    m_menuToolBarText->addAction(m_actionToolBarTextBelowIcons);
    m_menuToolBarText->addAction(m_actionToolBarTextOnly);
    m_menuToolBarText->addAction(m_actionToolBarTextSystem);
    menuView->addAction(m_actionShowComment);
    menuView->addSeparator();
    auto menuMoveNumbers = menuView->addMenu(tr("&Move Marking"));
    menuMoveNumbers->addAction(m_actionMoveMarkingLastDot);
    menuMoveNumbers->addAction(m_actionMoveMarkingLastNumber);
    menuMoveNumbers->addAction(m_actionMoveMarkingAllNumber);
    menuMoveNumbers->addAction(m_actionMoveMarkingNone);
    menuView->addAction(m_actionCoordinates);
    menuView->addAction(m_actionShowVariations);
    menuView->addSeparator();
    menuView->addAction(m_actionFullscreen);

    auto menuComputer = menuBar()->addMenu(tr("&Computer"));
    menuComputer->addAction(m_actionComputerColors);
    menuComputer->addAction(m_actionPlay);
    menuComputer->addSeparator();
    menuComputer->addAction(m_actionPlaySingleMove);
    menuComputer->addAction(m_actionInterrupt);
    menuComputer->addSeparator();
    m_menuLevel = menuComputer->addMenu(QString());
    m_menuLevel->addActions(m_actionGroupLevel->actions());

    auto menuTools = menuBar()->addMenu(tr("&Tools"));
    menuTools->addAction(m_actionRating);
    menuTools->addAction(m_actionAnalyzeGame);

    auto menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(m_actionHelp);
    menuHelp->addAction(m_actionAbout);
}

QLayout* MainWindow::createOrientationButtonBoxLeft()
{
    auto outerLayout = new QVBoxLayout;
    auto layout = new QGridLayout;
    layout->addWidget(createOBoxToolButton(m_actionRotateAnticlockwise), 0, 0);
    layout->addWidget(createOBoxToolButton(m_actionRotateClockwise), 0, 1);
    layout->addWidget(createOBoxToolButton(m_actionFlipHorizontally), 1, 0);
    layout->addWidget(createOBoxToolButton(m_actionFlipVertically), 1, 1);
    outerLayout->addStretch();
    outerLayout->addLayout(layout);
    outerLayout->addStretch();
    return outerLayout;
}

QLayout* MainWindow::createOrientationButtonBoxRight()
{
    auto outerLayout = new QVBoxLayout;
    auto layout = new QGridLayout;
    layout->addWidget(createOBoxToolButton(m_actionPreviousPiece), 0, 0);
    layout->addWidget(createOBoxToolButton(m_actionNextPiece), 0, 1);
    layout->addWidget(createOBoxToolButton(m_actionClearPiece), 1, 0,
                      1, 2, Qt::AlignHCenter);
    outerLayout->addStretch();
    outerLayout->addLayout(layout);
    outerLayout->addStretch();
    return outerLayout;
}

QLayout* MainWindow::createOrientationSelector()
{
    auto layout = new QHBoxLayout;
    layout->addStretch();
    layout->addLayout(createOrientationButtonBoxLeft());
    layout->addSpacing(8);
    m_orientationDisplay = new OrientationDisplay(nullptr, m_bd);
    connect(m_orientationDisplay, &OrientationDisplay::colorClicked, [this](Color) {
        if (m_actionSetupMode->isChecked())
            nextColor();
    });
    m_orientationDisplay->setSizePolicy(QSizePolicy::MinimumExpanding,
                                        QSizePolicy::MinimumExpanding);
    layout->addWidget(m_orientationDisplay);
    layout->addSpacing(8);
    layout->addLayout(createOrientationButtonBoxRight());
    layout->addStretch();
    return layout;
}

QLayout* MainWindow::createRightPanel()
{
    auto layout = new QVBoxLayout;
    layout->addLayout(createOrientationSelector(), 20);
    m_scoreDisplay = new ScoreDisplay;
    layout->addWidget(m_scoreDisplay, 5);
    auto pieceSelectorLayout = new SameHeightLayout;
    layout->addLayout(pieceSelectorLayout, 80);
    for (Color c : Color::Range(Color::range))
    {
        m_pieceSelector[c] = new PieceSelector(nullptr, m_bd, c);
        connect(m_pieceSelector[c], &PieceSelector::pieceSelected,
                this, &MainWindow::selectTransformedPiece);
        pieceSelectorLayout->addWidget(m_pieceSelector[c]);
    }
    return layout;
}

QString MainWindow::decode(const string& s) const
{
    return m_textCodec->toUnicode(s.c_str());
}

void MainWindow::deleteAllVariations()
{
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Delete all variations?"),
                 tr("All variations but the main variation will be"
                    " removed from the game tree."));
    auto deleteButton =
        msgBox.addButton(tr("Delete Variations"), QMessageBox::DestructiveRole);
    auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();
    if (msgBox.clickedButton() != deleteButton)
        return;
    bool currentNodeChanges = ! is_main_variation(m_game.get_current());
    if (currentNodeChanges)
        cancelThread();
    m_game.delete_all_variations();
    updateWindow(currentNodeChanges);
}

void MainWindow::doubtfulMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_doubtful_move();
    updateWindow(false);
}

void MainWindow::createToolBar()
{
    auto toolBar = new QToolBar;
    toolBar->setMovable(false);
    toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    toolBar->addAction(m_actionNew);
    toolBar->addAction(m_actionRatedGame);
    toolBar->addAction(m_actionUndo);
    toolBar->addSeparator();
    toolBar->addAction(m_actionComputerColors);
    toolBar->addAction(m_actionPlay);
    toolBar->addSeparator();
    toolBar->addAction(m_actionBeginning);
    toolBar->addAction(m_actionBackward);
    toolBar->addAction(m_actionForward);
    toolBar->addAction(m_actionEnd);
    toolBar->addSeparator();
    toolBar->addAction(m_actionNextVariation);
    toolBar->addAction(m_actionPreviousVariation);
    // Is this the right way to enable autorepeat buttons? Using
    // QAction::autoRepeat applies only to keyboard and adding a QToolButton
    // with QToolBar::addWidget() makes the tool button not respect the
    // toolButtonStyle.
    const auto buttons = toolBar->findChildren<QToolButton*>();
    for (auto button : buttons)
    {
        auto action = button->defaultAction();
        if (action == m_actionBackward || action == m_actionForward)
            button->setAutoRepeat(true);
    }
    addToolBar(toolBar);
    initToolBarText(toolBar);
    QSettings settings;
    auto toolBarText = settings.value("toolbar_text", "system").toString();
    if (toolBarText == "no_text")
        m_actionToolBarNoText->setChecked(true);
    else if (toolBarText == "beside_icons")
        m_actionToolBarTextBesideIcons->setChecked(true);
    else if (toolBarText == "below_icons")
        m_actionToolBarTextBelowIcons->setChecked(true);
    else if (toolBarText == "text_only")
        m_actionToolBarTextOnly->setChecked(true);
    else
        m_actionToolBarTextSystem->setChecked(true);
}

void MainWindow::deleteAutoSaveFile()
{
    QString autoSaveFile = getAutoSaveFile();
    QFile file(autoSaveFile);
    if (file.exists() && ! file.remove())
        showError(tr("Could not delete %1").arg(autoSaveFile));
}

void MainWindow::enablePieceSelector(Color c)
{
    for (Color i : m_bd.get_colors())
    {
        m_pieceSelector[i]->checkUpdate();
        m_pieceSelector[i]->setEnabled(i == c);
    }
}

QByteArray MainWindow::encode(const QString& s) const
{
    return m_textCodec->fromUnicode(s);
}

void MainWindow::end()
{
    gotoNode(get_last_node(m_game.get_current()));
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    // Empty status tips can clear the status bar if the mouse goes over a
    // menu. We don't want that because it deletes our "computer is thinking"
    // message. This still happens with Qt 5.6 on some platforms.
    if (event->type() == QEvent::StatusTip)
        return true;
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::exportAsciiArt()
{
    QString file = QFileDialog::getSaveFileName(this, "", getLastDir(),
                                      tr("Text files (*.txt);;All files (*)"));
    if (file.isEmpty())
        return;
    rememberDir(file);
    ofstream out(file.toLocal8Bit().constData());
    m_bd.write(out, false);
    if (! out)
        showError(QString::fromLocal8Bit(strerror(errno)));
}

void MainWindow::exportImage()
{
    QSettings settings;
    auto size = settings.value("export_image_size", 420).toInt();
    QInputDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags()
                          & ~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle(tr("Export Image"));
    dialog.setLabelText(tr("Image size:"));
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(0, 2147483647);
    dialog.setIntStep(40);
    dialog.setIntValue(size);
    if (dialog.exec() == 0)
        return;
    size = dialog.intValue();
    settings.setValue("export_image_size", size);
    bool coordinates = m_actionCoordinates->isChecked();
    BoardPainter boardPainter;
    boardPainter.setCoordinates(coordinates);
    boardPainter.setCoordinateColor(QColor(100, 100, 100));
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    if (coordinates)
        painter.fillRect(0, 0, size, size, QColor(216, 216, 216));
    boardPainter.paintEmptyBoard(painter, size, size, m_bd.get_variant(),
                                 m_bd.get_geometry());
    Grid<unsigned> pieceId;
    if (m_bd.get_board_type() == BoardType::nexos)
    {
        pieceId.fill(0, m_bd.get_geometry());
        unsigned n = 0;
        for (Color c : m_bd.get_colors())
            for (Move mv : m_bd.get_setup().placements[c])
            {
                ++n;
                for (Point p : m_bd.get_move_points(mv))
                    pieceId[p] = n;
            }
        for (auto mv : m_bd.get_moves())
        {
            ++n;
            for (Point p : m_bd.get_move_points(mv.move))
                pieceId[p] = n;
        }
    }
    boardPainter.paintPieces(painter, m_bd.get_point_state(), pieceId,
                             &m_guiBoard->getLabels());
    painter.end();
    QString file;
    while (true)
    {
        file = QFileDialog::getSaveFileName(this, file, getLastDir());
        if (file.isEmpty())
            break;
        rememberDir(file);
        QImageWriter writer(file);
        if (writer.write(image))
            break;
        showError(writer.errorString());
    }
}

void MainWindow::findMove()
{
    if (m_bd.is_game_over())
        return;
    if (! m_legalMoves)
        m_legalMoves = make_unique<MoveList>();
    Color to_play = m_bd.get_to_play();
    if (m_legalMoves->empty())
    {
        if (! m_marker)
            m_marker = make_unique<MoveMarker>();
        m_bd.gen_moves(to_play, *m_marker, *m_legalMoves);
        m_marker->clear(*m_legalMoves);
        sort(m_legalMoves->begin(), m_legalMoves->end(),
             [&](Move mv1, Move mv2) {
                 return getHeuristic(m_bd, mv1) > getHeuristic(m_bd, mv2);
             });
    }
    if (m_legalMoves->empty())
        return;
    if (m_legalMoveIndex >= m_legalMoves->size())
        m_legalMoveIndex = 0;
    auto mv = (*m_legalMoves)[m_legalMoveIndex];
    selectPiece(to_play, m_bd.get_move_piece(mv));
    m_guiBoard->showMove(to_play, mv);
    ++m_legalMoveIndex;
}

void MainWindow::findNextComment()
{
    auto& root = m_game.get_root();
    auto& current = m_game.get_current();
    auto node = find_next_comment(current);
    if ((node == nullptr) && &current != &root)
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The end of the tree was reached."),
                     tr("Continue the search from the start of the tree?"));
        auto continueButton =
            msgBox.addButton(tr("Continue From Start"),
                             QMessageBox::AcceptRole);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(continueButton);
        msgBox.exec();
        if (msgBox.clickedButton() == continueButton)
        {
            node = &root;
            if (! has_comment(*node))
                node = find_next_comment(*node);
        }
        else
            return;
    }
    if (node == nullptr)
    {
        showInfo(tr("No comment found"));
        return;
    }
    showComment(true);
    gotoNode(*node);
}

void MainWindow::flipHorizontally()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = m_bd.get_transforms().get_mirrored_horizontally(transform);
    transform = m_bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::flipVertically()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = m_bd.get_transforms().get_mirrored_vertically(transform);
    transform = m_bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::forward()
{
    gotoNode(m_game.get_current().get_first_child_or_null());
}

void MainWindow::fullscreen()
{
    if (isFullScreen())
    {
        // If F11 is pressed in fullscreen, we switch to normal
        leaveFullscreen();
        return;
    }
    QSettings settings;
    menuBar()->hide();
    findChild<QToolBar*>()->hide();
    settings.setValue("geometry", saveGeometry());
    m_wasMaximized = isMaximized();
    showFullScreen();
    if (m_leaveFullscreenButton == nullptr)
        m_leaveFullscreenButton =
            new LeaveFullscreenButton(this, m_actionLeaveFullscreen);
    m_leaveFullscreenButton->showButton();
}

void MainWindow::gameInfo()
{
    GameInfoDialog dialog(this, m_game, m_textCodec);
    dialog.exec();
    updateWindow(false);
}

void MainWindow::gameOver()
{
    auto variant = m_bd.get_variant();
    auto nuColors = get_nu_colors(variant);
    auto nuPlayers = get_nu_players(variant);
    bool breakTies = (m_bd.get_piece_set() == PieceSet::callisto);
    QString info;
    if (variant == Variant::duo)
    {
        auto score = m_bd.get_score_twocolor(Color(0));
        if (score == 1)
            info = tr("Purple wins with 1 point.");
        else if (score > 0)
            info = tr("Purple wins with %L1 points.").arg(score);
        else if (score == -1)
            info = tr("Orange wins with 1 point.");
        else if (score < 0)
            info = tr("Orange wins with %L1 points.").arg(-score);
        else
            info = tr("The game ends in a tie.");
    }
    else if (variant == Variant::junior)
    {
        auto score = m_bd.get_score_twocolor(Color(0));
        if (score == 1)
            info = tr("Green wins with 1 point.");
        else if (score > 0)
            info = tr("Green wins with %L1 points.").arg(score);
        else if (score == -1)
            info = tr("Orange wins with 1 point.");
        else if (score < 0)
            info = tr("Orange wins with %L1 points.").arg(-score);
        else
            info = tr("The game ends in a tie.");
    }
    else if (nuColors == 2)
    {
        auto score = m_bd.get_score_twocolor(Color(0));
        if (score == 1)
            info = tr("Blue wins with 1 point.");
        else if (score > 0)
            info = tr("Blue wins with %L1 points.").arg(score);
        else if (score == -1)
            info = tr("Green wins with 1 point.");
        else if (score < 0)
            info = tr("Green wins with %L1 points.").arg(-score);
        else if (breakTies)
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Green wins (tie resolved).");
        else
            info = tr("The game ends in a tie.");
    }
    else if (nuPlayers == 2)
    {
        LIBBOARDGAME_ASSERT(nuColors == 4);
        auto score = m_bd.get_score_multicolor(Color(0));
        if (score == 1)
            info = tr("Blue/Red wins with 1 point.");
        else if (score > 0)
            info = tr("Blue/Red wins with %L1 points.").arg(score);
        else if (score == -1)
            info = tr("Yellow/Green wins with 1 point.");
        else if (score < 0)
            info = tr("Yellow/Green wins with %L1 points.").arg(-score);
        else if (breakTies)
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Yellow/Green wins (tie resolved).");
        else
            info = tr("The game ends in a tie.");
    }
    else if (nuPlayers == 3)
    {
        auto blue = m_bd.get_points(Color(0));
        auto yellow = m_bd.get_points(Color(1));
        auto red = m_bd.get_points(Color(2));
        auto maxPoints = max(blue, max(yellow, red));
        if (breakTies && red == maxPoints
                && (blue == maxPoints || yellow == maxPoints))
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Red wins (tie resolved).");
        else if (breakTies && yellow == maxPoints && blue == maxPoints)
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Yellow wins (tie resolved).");
        else if (blue == yellow && yellow == red)
            info = tr("The game ends in a tie between all colors.");
        else if (blue == maxPoints && blue == yellow)
            info = tr("The game ends in a tie between Blue and Yellow.");
        else if (blue == maxPoints && blue == red)
            info = tr("The game ends in a tie between Blue and Red.");
        else if (yellow == maxPoints && yellow == red)
            info = tr("The game ends in a tie between Yellow and Red.");
        else if (blue == maxPoints)
            info = tr("Blue wins.");
        else if (yellow == maxPoints)
            info = tr("Yellow wins.");
        else
            info = tr("Red wins.");
    }
    else
    {
        LIBBOARDGAME_ASSERT(nuPlayers == 4);
        auto blue = m_bd.get_points(Color(0));
        auto yellow = m_bd.get_points(Color(1));
        auto red = m_bd.get_points(Color(2));
        auto green = m_bd.get_points(Color(3));
        auto maxPoints = max(blue, max(yellow, max(red, green)));
        if (breakTies && green == maxPoints
                && (red == maxPoints || blue == maxPoints
                    || yellow == maxPoints))
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Green wins (tie resolved).");
        else if (breakTies && red == maxPoints
                && (blue == maxPoints || yellow == maxPoints))
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Red wins (tie resolved).");
        else if (breakTies && yellow == maxPoints && blue == maxPoints)
            //: Game variant with tie-breaker rule made later player win.
            info = tr("Yellow wins (tie resolved).");
        else if (blue == yellow && yellow == red && red == green)
            info = tr("The game ends in a tie between all colors.");
        else if (blue == maxPoints && blue == yellow && yellow == red)
            info = tr("The game ends in a tie between Blue, Yellow and Red.");
        else if (blue == maxPoints && blue == yellow && yellow == green)
            info =
                tr("The game ends in a tie between Blue, Yellow and Green.");
        else if (blue == maxPoints && blue == red && red == green)
            info = tr("The game ends in a tie between Blue, Red and Green.");
        else if (yellow == maxPoints && yellow == red && red == green)
            info = tr("The game ends in a tie between Yellow, Red and Green.");
        else if (blue == maxPoints && blue == yellow)
            info = tr("The game ends in a tie between Blue and Yellow.");
        else if (blue == maxPoints && blue == red)
            info = tr("The game ends in a tie between Blue and Red.");
        else if (blue == maxPoints && blue == green)
            info = tr("The game ends in a tie between Blue and Green.");
        else if (yellow == maxPoints && yellow == red)
            info = tr("The game ends in a tie between Yellow and Red.");
        else if (yellow == maxPoints && yellow == green)
            info = tr("The game ends in a tie between Yellow and Green.");
        else if (red == maxPoints && red == green)
            info = tr("The game ends in a tie between Red and Green.");
        else if (blue == maxPoints)
            info = tr("Blue wins.");
        else if (yellow == maxPoints)
            info = tr("Yellow wins.");
        else if (red == maxPoints)
            info = tr("Red wins.");
        else
            info = tr("Green wins.");
    }
    if (m_isRated)
    {
        QString detailText;
        int oldRating = m_history->getRating().to_int();
        unsigned place;
        bool isPlaceShared;
        m_bd.get_place(m_ratedGameColor, place, isPlaceShared);
        float gameResult;
        if (place == 0 && !isPlaceShared)
            gameResult = 1;
        else if (place == 0 && isPlaceShared)
            gameResult = 0.5;
        else
            gameResult = 0;
        unsigned nuOpp = get_nu_players(variant) - 1;
        Rating oppRating = m_player->get_rating(variant);
        QString date = QString(PentobiTree::get_date_today().c_str());
        m_history->addGame(gameResult, oppRating, nuOpp, m_ratedGameColor,
                           gameResult, date, m_level, m_game.get_tree());
        if (m_ratingDialog != nullptr)
            m_ratingDialog->updateContent();
        int newRating = m_history->getRating().to_int();
        if (newRating > oldRating)
            detailText = tr("Your rating has increased from %1 to %2.")
                  .arg(QString::number(oldRating), QString::number(newRating));
        else if (newRating == oldRating)
            detailText = tr("Your rating stays at %1.").arg(oldRating);
        else
            detailText =
                  tr("Your rating has decreased from %1 to %2.")
                  .arg(QString::number(oldRating), QString::number(newRating));
        setRated(false);
        QSettings settings;
        auto key = QString("next_rated_random_") + to_string_id(variant);
        settings.remove(key);
        QMessageBox msgBox(this);
        Util::setNoTitle(msgBox);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(info);
        msgBox.setInformativeText(detailText);
        auto showRatingButton =
            msgBox.addButton(tr("Show &Rating"), QMessageBox::AcceptRole);
        msgBox.addButton(QMessageBox::Close);
        msgBox.setDefaultButton(showRatingButton);
        msgBox.exec();
        auto result = msgBox.clickedButton();
        if (result == showRatingButton)
            showRating();
    }
    else
        showInfo(info);
}

void MainWindow::genMove(bool playSingleMove)
{
    cancelThread();
    ++m_genMoveId;
    setCursor(QCursor(Qt::BusyCursor));
    m_actionNextPiece->setEnabled(false);
    m_actionPreviousPiece->setEnabled(false);
    m_actionPlay->setEnabled(false);
    m_actionPlaySingleMove->setEnabled(false);
    m_actionInterrupt->setEnabled(true);
    showStatus(tr("Computer is thinking..."));
    clearPiece();
    clear_abort();
    m_lastRemainingSeconds = 0;
    m_lastRemainingMinutes = 0;
    m_player->set_level(m_level);
    QFuture<GenMoveResult> future =
        QtConcurrent::run(this, &MainWindow::asyncGenMove, m_bd.get_to_play(),
                          m_genMoveId, playSingleMove);
    m_genMoveWatcher.setFuture(future);
    m_isGenMoveRunning = true;
}

void MainWindow::genMoveFinished()
{
    m_actionInterrupt->setEnabled(false);
    clearStatus();
    GenMoveResult result = m_genMoveWatcher.future().result();
    if (result.genMoveId != m_genMoveId)
    {
        // Callback from a move generation canceled with cancelThread()
        return;
    }
    LIBBOARDGAME_ASSERT(m_isGenMoveRunning);
    m_isGenMoveRunning = false;
    setCursor(QCursor(Qt::ArrowCursor));
    if (get_abort() && computerPlaysAll())
        m_computerColors.fill(false);
    Color c = result.color;
    auto mv = result.move;
    if (mv.is_null())
    {
        // No need to translate, should never happen if program is correct
        showError("Computer failed to generate a move");
        return;
    }
    if (! m_bd.is_legal(c, mv))
    {
        // No need to translate, should never happen if program is correct
        showError("Computer generated illegal    move");
        return;
    }
    playMove(c, mv);
    // Call updateWindow() before checkComputerMove() because checkComputerMove
    // resets m_lastComputerMovesBegin if computer doesn't play current color
    // and updateWindow needs m_lastComputerMovesBegin
    updateWindow(true);
    if (! result.playSingleMove)
        checkComputerMove();
}

QString MainWindow::getFilter() const
{
    return tr("Blokus games (*.blksgf);;All files (*)");
}

QString MainWindow::getLastDir()
{
    QSettings settings;
    auto dir = settings.value("last_dir", "").toString();
    if (dir.isEmpty() || ! QFileInfo::exists(dir))
        dir = QStandardPaths::writableLocation(
                                          QStandardPaths::DocumentsLocation);
    return dir;
}

QString MainWindow::getVersion() const
{
    QString version;
#ifdef VERSION
    version = VERSION;
#endif
    if (version.isEmpty())
        version = "UNKNOWN";
    return version;
}

void MainWindow::goodMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_good_move();
    updateWindow(false);
}

void MainWindow::gotoMove()
{
    auto& tree = m_game.get_tree();
    auto& current = m_game.get_current();
    auto moveNumber = get_move_number(tree, current);
    auto maxMoves = moveNumber + get_moves_left(tree, current);
    if (maxMoves == 0)
        return;
    auto defaultValue = moveNumber;
    if (defaultValue == 0)
        defaultValue = maxMoves;
    QInputDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags()
                          & ~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle(tr("Go to Move"));
    dialog.setLabelText(tr("Move number:"));
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(1, static_cast<int>(maxMoves));
    dialog.setIntStep(1);
    dialog.setIntValue(static_cast<int>(defaultValue));
    if (dialog.exec() != 0)
        gotoNode(get_move_node(tree, current, dialog.intValue()));
}

void MainWindow::gotoNode(const SgfNode& node)
{
    cancelThread();
    leaveSetupMode();
    try
    {
        m_game.goto_node(node);
    }
    catch (const SgfError& e)
    {
        showInvalidFile(m_file, e);
        return;
    }
    if ((m_analyzeGameWindow != nullptr) && m_analyzeGameWindow->isVisible())
        m_analyzeGameWindow->analyzeGameWidget
            ->setCurrentPosition(m_game, node);
    m_autoPlay = false;
    updateWindow(true);
}

void MainWindow::gotoNode(const SgfNode* node)
{
    if (node != nullptr)
        gotoNode(*node);
}

void MainWindow::gotoPosition(Variant variant,
                              const vector<ColorMove>& moves)
{
    if (m_bd.get_variant() != variant)
        return;
    auto& tree = m_game.get_tree();
    auto node = &tree.get_root();
    if (tree.has_move(*node))
        // Move in root node not supported.
        return;
    for (ColorMove mv : moves)
    {
        bool found = false;
        for (auto& i : node->get_children())
            if (tree.get_move(i) == mv)
            {
                found = true;
                node = &i;
                break;
            }
        if (! found)
            return;
    }
    gotoNode(*node);
}

void MainWindow::help()
{
    if (m_helpWindow == nullptr)
    {
        QString path = HelpWindow::findMainPage(m_helpDir, "pentobi");
        m_helpWindow = new HelpWindow(nullptr, tr("Pentobi Help"), path);
    }
    m_helpWindow->show();
    m_helpWindow->raise();
}

void MainWindow::initGame()
{
    setRated(false);
    if (m_analyzeGameWindow != nullptr)
    {
        delete m_analyzeGameWindow;
        m_analyzeGameWindow = nullptr;
    }
    m_game.init();
    setUtf8();
#ifdef VERSION
    m_game.set_application("Pentobi", VERSION);
#else
    m_game.set_application("Pentobi");
#endif
    m_game.set_date_today();
    m_game.clear_modified();
    QSettings settings;
    if (! settings.value("computer_color_none").toBool())
    {
        for (Color c : Color::Range(m_bd.get_nu_nonalt_colors()))
            m_computerColors[c] = ! m_bd.is_same_player(c, Color(0));
        m_autoPlay = true;
    }
    else
    {
        m_computerColors.fill(false);
        m_autoPlay = false;
    }
    leaveSetupMode();
    m_gameFinished = false;
    m_isAutoSaveLoaded = false;
    setFile("");
}

void MainWindow::initVariantActions()
{
    // Use a temporary const variable to avoid that QList detaches in for loop
    const auto actions = m_actionGroupVariant->actions();
    for (auto action : actions)
        if (Variant(action->data().toInt()) == m_bd.get_variant())
        {
            action->setChecked(true);
            return;
        }
}

void MainWindow::initPieceSelectors()
{
    for (Color::IntType i = 0; i < Color::range; ++i)
    {
        bool isVisible = (i < m_bd.get_nu_colors());
        m_pieceSelector[Color(i)]->setVisible(isVisible);
        if (isVisible)
            m_pieceSelector[Color(i)]->init();
    }
}

void MainWindow::interestingMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_interesting_move();
    updateWindow(false);
}

void MainWindow::interrupt()
{
    cancelThread();
    m_autoPlay = false;
}

void MainWindow::interruptPlay()
{
    if (! m_isGenMoveRunning)
        return;
    set_abort();
    m_autoPlay = false;
}

bool MainWindow::isComputerToPlay() const
{
    Color to_play = m_bd.get_to_play();
    if (m_game.get_variant() != Variant::classic_3 || to_play != Color(3))
        return m_computerColors[to_play];
    return m_computerColors[Color(m_bd.get_alt_player())];
}

void MainWindow::keepOnlyPosition()
{
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Keep only position?"),
                 tr("All previous and following moves and variations will"
                    " be removed from the game tree."));
    auto keepOnlyPositionButton =
        msgBox.addButton(tr("Keep Only Position"),
                         QMessageBox::DestructiveRole);
    auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();
    if (msgBox.clickedButton() != keepOnlyPositionButton)
        return;
    cancelThread();
    m_game.keep_only_position();
    updateWindow(true);
}

void MainWindow::keepOnlySubtree()
{
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Keep only subtree?"),
                 tr("All previous moves and variations will be removed"
                    " from the game tree."));
    auto keepOnlySubtreeButton =
        msgBox.addButton(tr("Keep Only Subtree"),
                         QMessageBox::DestructiveRole);
    auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();
    if (msgBox.clickedButton() != keepOnlySubtreeButton)
        return;
    cancelThread();
    m_game.keep_only_subtree();
    updateWindow(true);
}

void MainWindow::leaveFullscreen()
{
    if (! isFullScreen())
        return;
    QSettings settings;
    auto showToolbar = settings.value("toolbar", true).toBool();
    menuBar()->show();
    findChild<QToolBar*>()->setVisible(showToolbar);
    // m_leaveFullscreenButton can be null if the window was put in fullscreen
    // mode by a "generic" method by the window manager (e.g. the title bar
    // menu on KDE) and not by MainWindow::fullscreen()
    if (m_leaveFullscreenButton != nullptr)
        m_leaveFullscreenButton->hideButton();
    // Call showNormal() even if m_wasMaximized otherwise restoring the
    // maximized window state does not work correctly on Xfce
    showNormal();
    if (m_wasMaximized)
        showMaximized();
}

void MainWindow::leaveSetupMode()
{
    if (! m_actionSetupMode->isChecked())
        return;
    setupMode(false);
}

void MainWindow::levelTriggered(bool checked)
{
    if (checked)
        setLevel(qobject_cast<QAction*>(sender())->data().toUInt());
}

void MainWindow::loadHistory()
{
    auto variant = m_game.get_variant();
    if (m_history->getVariant() == variant)
        return;
    m_history->load(variant);
    if (m_ratingDialog != nullptr)
        m_ratingDialog->updateContent();
}

void MainWindow::makeMainVariation()
{
    m_game.make_main_variation();
    updateWindow(false);
}

void MainWindow::moveDownVariation()
{
    m_game.move_down_variation();
    updateWindow(false);
}

void MainWindow::moveUpVariation()
{
    m_game.move_up_variation();
    updateWindow(false);
}

void MainWindow::nextColor()
{
    m_game.set_to_play(m_bd.get_next(m_bd.get_to_play()));
    auto to_play = m_bd.get_to_play();
    m_orientationDisplay->selectColor(to_play);
    clearPiece();
    for (Color c : m_bd.get_colors())
        m_pieceSelector[c]->setEnabled(to_play == c);
    if (m_actionSetupMode->isChecked())
        setSetupPlayer();
    updateWindow(false);
}

void MainWindow::nextPiece()
{
    auto c = m_bd.get_to_play();
    if (m_bd.get_pieces_left(c).empty())
        return;
    auto nuUniqPieces = m_bd.get_nu_uniq_pieces();
    Piece::IntType i;
    Piece selectedPiece = m_guiBoard->getSelectedPiece();
    if (! selectedPiece.is_null())
        i = static_cast<Piece::IntType>(selectedPiece.to_int() + 1);
    else
        i = 0;
    while (true)
    {
        if (i >= nuUniqPieces)
            i = 0;
        if (m_bd.is_piece_left(c, Piece(i)))
            break;
        ++i;
    }
    selectPiece(c, Piece(i));
}

void MainWindow::nextTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = m_bd.get_piece_info(piece).get_next_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::nextVariation()
{
    gotoNode(m_game.get_current().get_sibling());
}

void MainWindow::newGame()
{
    if (! checkSave())
        return;
    cancelThread();
    initGame();
    deleteAutoSaveFile();
    updateWindow(true);
}

void MainWindow::noMoveAnnotation(bool checked)
{
    if (! checked)
        return;
    m_game.remove_move_annotation();
    updateWindow(false);
}

void MainWindow::open()
{
    if (! checkSave())
        return;
    //: Window title for open dialog
    QString file = QFileDialog::getOpenFileName(this, tr("Open"), getLastDir(),
                                                getFilter());
    if (file.isEmpty())
        return;
    rememberDir(file);
    if (openFile(file))
        rememberFile(file);
}

bool MainWindow::openStream(istream& in)
{
    cancelThread();
    TreeReader reader;
    try
    {
        reader.read(in);
    }
    catch (const TreeReader::ReadError& e)
    {
        showInvalidSgf(e);
        return false;
    }
    setFile("");
    m_isAutoSaveLoaded = false;
    delete m_analyzeGameWindow;
    m_analyzeGameWindow = nullptr;
    setRated(false);
    try
    {
        auto tree = reader.get_tree_transfer_ownership();
        m_game.init(tree);
        auto charSet = m_game.get_charset();
        if (charSet.empty())
            m_textCodec = QTextCodec::codecForName("ISO 8859-1");
        else
            m_textCodec = QTextCodec::codecForName(m_game.get_charset().c_str());
        if (m_textCodec == nullptr)
        {
            qWarning() << "GameModel: unknown codec '"
                       << QString::fromLocal8Bit(charSet.c_str()) << "'";
            m_textCodec = QTextCodec::codecForName("ISO 8859-1");
        }
        if (! libpentobi_base::has_setup(m_game.get_root()))
            m_game.goto_node(get_last_node(m_game.get_root()));
        initPieceSelectors();
    }
    catch (const SgfError& e)
    {
        showInvalidSgf(e);
    }
    m_computerColors.fill(false);
    m_autoPlay = false;
    leaveSetupMode();
    initVariantActions();
    restoreLevel(m_bd.get_variant());
    updateWindow(true);
    m_orientationDisplay->update();
    loadHistory();
    return true;
}

bool MainWindow::openFile(const QString& file, bool isTemporary)
{
    if (file.isEmpty())
        return false;
    cancelThread();
    TreeReader reader;
    ifstream in(file.toLocal8Bit().constData());
    if (! in)
    {
        QString text =
            tr("Could not read file '%1'").arg(QFileInfo(file).fileName());
        showError(text, QString::fromLocal8Bit(strerror(errno)));
        return false;
    }
    if (! openStream(in))
        return false;
    if (! isTemporary)
    {
        setFile(file);
        deleteAutoSaveFile();
    }
    return true;
}

void MainWindow::openFromClipboard()
{
    auto text = qApp->clipboard()->text();
    if (text.isEmpty())
    {
        showInfo(tr("Clipboard is empty."));
        return;
    }
    istringstream in(text.toLocal8Bit().constData());
    openStream(in);
}

void MainWindow::openRecentFile()
{
     auto action = qobject_cast<QAction*>(sender());
     if (action != nullptr && checkSave())
         openFile(action->data().toString());
}

void MainWindow::placePiece(Color c, Move mv)
{
    cancelThread();
    bool isSetupMode = m_actionSetupMode->isChecked();
    bool isAltColor =
            (m_bd.get_variant() == Variant::classic_3 && c.to_int() == 3);
    if ((! isAltColor && m_computerColors[c])
            || (isAltColor && m_computerColors[Color(m_bd.get_alt_player())])
            || isSetupMode)
        // If the user enters a move previously played by the computer (e.g.
        // after undoing moves) then it is unlikely that the user wants to keep
        // the computer color settings.
        m_computerColors.fill(false);
    if (isSetupMode)
    {
        m_game.add_setup(c, mv);
        setSetupPlayer();
        updateWindow(true);
    }
    else
    {
        playMove(c, mv);
        updateWindow(true);
        checkComputerMove();
    }
}

void MainWindow::play()
{
    cancelThread();
    leaveSetupMode();
    if (! isComputerToPlay())
    {
        m_computerColors.fill(false);
        auto c = m_bd.get_to_play();
        if (m_bd.get_variant() == Variant::classic_3 && c == Color(3))
            m_computerColors[Color(m_bd.get_alt_player())] = true;
        else
        {
            m_computerColors[c] = true;
            m_computerColors[m_bd.get_second_color(c)] = true;
        }
        QSettings settings;
        settings.setValue("computer_color_none", false);
    }
    m_autoPlay = true;
    genMove();
}

void MainWindow::playMove(Color c, Move mv)
{
    m_game.play(c, mv, false);
    m_gameFinished = false;
    if (m_bd.is_game_over())
    {
        updateWindow(true);
        repaint();
        gameOver();
        m_gameFinished = true;
        deleteAutoSaveFile();
        return;
    }
}

void MainWindow::playSingleMove()
{
    cancelThread();
    leaveSetupMode();
    m_autoPlay = false;
    genMove(true);
}

void MainWindow::pointClicked(Point p)
{
    // If a piece on the board is clicked on in setup mode, remove it and make
    // it the selected piece without changing its orientation.
    if (! m_actionSetupMode->isChecked())
        return;
    PointState s = m_bd.get_point_state(p);
    if (s.is_empty())
        return;
    Color c = s.to_color();
    Move mv = m_bd.get_move_at(p);
    m_game.remove_setup(c, mv);
    setSetupPlayer();
    updateWindow(true);
    selectTransformedPiece(c, m_bd.get_move_piece(mv), m_bd.find_transform(mv));
    m_guiBoard->setSelectedPiecePoints(mv);
}

void MainWindow::previousPiece()
{
    auto c = m_bd.get_to_play();
    if (m_bd.get_pieces_left(c).empty())
        return;
    auto nuUniqPieces = m_bd.get_nu_uniq_pieces();
    Piece::IntType i;
    Piece selectedPiece = m_guiBoard->getSelectedPiece();
    if (! selectedPiece.is_null())
        i = selectedPiece.to_int();
    else
        i = 0;
    while (true)
    {
        if (i == 0)
            i = static_cast<Piece::IntType>(nuUniqPieces - 1);
        else
            --i;
        if (m_bd.is_piece_left(c, Piece(i)))
            break;
    }
    selectPiece(c, Piece(i));
}

void MainWindow::previousTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform =
        m_bd.get_piece_info(piece).get_previous_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::previousVariation()
{
    gotoNode(m_game.get_current().get_previous_sibling());
}

void MainWindow::ratedGame()
{
    if (! checkSave())
        return;
    cancelThread();
    if (m_history->getNuGames() == 0)
    {
        InitialRatingDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted)
            return;
        m_history->init(Rating(static_cast<float>(dialog.getRating())));
    }
    int level;
    QSettings settings;
    unsigned random;
    auto variant = m_bd.get_variant();
    auto key = QString("next_rated_random_") + to_string_id(variant);
    if (settings.contains(key))
        random = settings.value(key).toUInt();
    else
    {
        // RandomGenerator::ResultType may be larger than unsigned
        random = static_cast<unsigned>(m_random.generate() % 1000);
        settings.setValue(key, random);
    }
    m_history->getNextRatedGameSettings(m_maxLevel, random,
                                        level, m_ratedGameColor);
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Start rated game?"),
                 "<html>" +
                 tr("In this game, you play %1 against Pentobi level&nbsp;%2.")
                 .arg(getPlayerString(variant, m_ratedGameColor),
                      QString::number(level)));
    auto startGameButton =
        msgBox.addButton(tr("&Start Game"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(startGameButton);
    msgBox.exec();
    auto result = msgBox.clickedButton();
    if (result != startGameButton)
        return;
    setLevel(level);
    initGame();
    setFile("");
    setRated(true);
    m_computerColors.fill(true);
    for (Color c : Color::Range(m_bd.get_nu_nonalt_colors()))
        if (m_bd.is_same_player(c, m_ratedGameColor))
            m_computerColors[c] = false;
    m_autoPlay = true;
    QString computerPlayerName =
        //: The first argument is the version of Pentobi
        tr("Pentobi %1 (level %2)").arg(getVersion(), QString::number(level));
    string computerPlayerNameStdStr = encode(computerPlayerName).constData();
    string humanPlayerNameStdStr = encode(tr("Human")).constData();
    for (Color c : Color::Range(m_bd.get_nu_nonalt_colors()))
        if (m_computerColors[c])
            m_game.set_player_name(c, computerPlayerNameStdStr);
        else
            m_game.set_player_name(c, humanPlayerNameStdStr);
    // Setting the player names marks the game as modified but there is nothing
    // important that would need to be saved yet
    m_game.clear_modified();
    deleteAutoSaveFile();
    updateWindow(true);
    checkComputerMove();
}

void MainWindow::rememberDir(const QString& file)
{
    if (file.isEmpty())
        return;
    QString canonicalFile = file;
    QString canonicalFilePath = QFileInfo(file).canonicalFilePath();
    if (! canonicalFilePath.isEmpty())
        canonicalFile = canonicalFilePath;
    QFileInfo info(canonicalFile);
    QSettings settings;
    settings.setValue("last_dir", info.dir().path());
}

void MainWindow::rememberFile(const QString& file)
{
    if (file.isEmpty())
        return;
    QString canonicalFile = file;
    QString canonicalFilePath = QFileInfo(file).canonicalFilePath();
    if (! canonicalFilePath.isEmpty())
        canonicalFile = canonicalFilePath;
    QSettings settings;
    auto files = settings.value("recent_files").toStringList();
    files.removeAll(canonicalFile);
    files.prepend(canonicalFile);
    while (files.size() > maxRecentFiles)
        files.removeLast();
    settings.setValue("recent_files", files);
    settings.sync(); // updateRecentFiles() needs the new settings
    updateRecentFiles();
}

void MainWindow::restoreLevel(Variant variant)
{
    QSettings settings;
    QString key = QString("level_") + to_string_id(variant);
    m_level = settings.value(key, 1).toInt();
    if (m_level < 1)
        m_level = 1;
    if (m_level > m_maxLevel)
        m_level = m_maxLevel;
    m_actionGroupLevel->actions().at(m_level - 1)->setChecked(true);
}

void MainWindow::rotateAnticlockwise()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = m_bd.get_transforms().get_rotated_anticlockwise(transform);
    transform = m_bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    updateFlipActions();
}

void MainWindow::rotateClockwise()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = m_bd.get_transforms().get_rotated_clockwise(transform);
    transform = m_bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    updateFlipActions();
}

void MainWindow::save()
{
    if (m_file.isEmpty())
        saveAs();
    else if (saveFile(m_file))
    {
        m_game.clear_modified();
        updateWindow(false);
    }
}

bool MainWindow::saveFile(const QString& file)
{
    if (! writeGame(file.toLocal8Bit().constData()))
    {
        showError(tr("The file could not be saved."),
                  /*: Error message if file cannot be saved. %1 is
                    replaced by the file name, %2 by the error message
                    of the operating system. */
                  tr("%1: %2").arg(file,
                                   QString::fromLocal8Bit(strerror(errno))));
        return false;
    }
    return true;
}

void MainWindow::saveAs()
{
    QString file = m_file;
    if (file.isEmpty())
    {
        file = getLastDir();
        file.append(QDir::separator());
        file.append(tr("Untitled Game.blksgf"));
        if (QFileInfo::exists(file))
            for (unsigned i = 1; ; ++i)
            {
                file = getLastDir();
                file.append(QDir::separator());
                //: The argument is a number, which will be increased if a
                //: file with the same name already exists
                file.append(tr("Untitled Game %1.blksgf").arg(i));
                if (! QFileInfo::exists(file))
                    break;
            }
    }
    file = QFileDialog::getSaveFileName(this, tr("Save"), file, getFilter());
    if (! file.isEmpty())
    {
        rememberDir(file);
        if (saveFile(file))
        {
            m_game.clear_modified();
            updateWindow(false);
        }
        setFile(file);
        rememberFile(file);
    }
}

void MainWindow::searchCallback(double elapsedSeconds, double remainingSeconds)
{
    // If the search is longer than 10 sec, we show the (maximum) remaining
    // time (only during a move generation, ignore search callbacks during
    // game analysis)
    if (! m_isGenMoveRunning || elapsedSeconds < 10)
        return;
    QString text;
    auto seconds = static_cast<int>(ceil(remainingSeconds));
    if (seconds < 90)
    {
        if (seconds == m_lastRemainingSeconds)
            return;
        m_lastRemainingSeconds = seconds;
        text =
            tr("Computer is thinking... (up to %1 seconds remaining)")
            .arg(seconds);
    }
    else
    {
        auto minutes = static_cast<int>(ceil(remainingSeconds / 60));
        if (minutes == m_lastRemainingMinutes)
            return;
        m_lastRemainingMinutes = minutes;
        text =
            tr("Computer is thinking... (up to %1 minutes remaining)")
            .arg(minutes);
    }
    QMetaObject::invokeMethod(statusBar(), "showMessage", Q_ARG(QString, text),
                              Q_ARG(int, 0));
}

void MainWindow::selectNamedPiece()
{
    string name(qobject_cast<QAction*>(sender())->data().toString()
                .toLocal8Bit().constData());
    auto c = m_bd.get_to_play();
    Board::PiecesLeftList pieces;
    for (Piece::IntType i = 0; i < m_bd.get_nu_uniq_pieces(); ++i)
    {
        Piece piece(i);
        if (m_bd.is_piece_left(c, piece)
                && m_bd.get_piece_info(piece).get_name().find(name) == 0)
            pieces.push_back(piece);
    }
    if (pieces.empty())
        return;
    auto piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        piece = pieces[0];
    else
    {
        auto pos = std::find(pieces.begin(), pieces.end(), piece);
        if (pos == pieces.end())
            piece = pieces[0];
        else
        {
            ++pos;
            if (pos == pieces.end())
                piece = pieces[0];
            else
                piece = *pos;
        }
    }
    selectPiece(c, piece);
}

void MainWindow::selectPiece(Color c, Piece piece)
{
    selectTransformedPiece(c, piece, m_bd.get_transforms().get_default());
}

void MainWindow::selectTransformedPiece(Color c, Piece piece,
                                        const Transform* transform)
{
    if (m_isGenMoveRunning
            || (m_bd.is_game_over() && ! m_actionSetupMode->isChecked()))
        return;
    m_game.set_to_play(c);
    m_guiBoard->selectPiece(c, piece);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->selectColor(c);
    m_orientationDisplay->setSelectedPiece(piece);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    bool can_rotate = m_bd.get_piece_info(piece).can_rotate();
    m_actionRotateClockwise->setEnabled(can_rotate);
    m_actionRotateAnticlockwise->setEnabled(can_rotate);
    updateFlipActions();
    m_actionClearPiece->setEnabled(true);
}

void MainWindow::setNoDelay()
{
    m_noDelay = true;
}

void MainWindow::setVariant(Variant variant)
{
    if (m_bd.get_variant() == variant)
        return;
    if (! checkSave())
    {
        initVariantActions();
        return;
    }
    cancelThread();
    QSettings settings;
    settings.setValue("variant", to_string_id(variant));
    clearPiece();
    m_game.init(variant);
    initPieceSelectors();
    m_orientationDisplay->update();
    newGame();
    loadHistory();
    restoreLevel(variant);
}

void MainWindow::setFile(const QString& file)
{
    m_file = file;
    // Don't use setWindowFilePath() because of QTBUG-16507
    if (m_file.isEmpty())
        //: Main window title if no file is loaded.
        setWindowTitle(tr("Pentobi"));
    else
        setWindowTitle(QString("[*]%1").arg(QFileInfo(m_file).fileName()));
}

void MainWindow::setLevel(unsigned level)
{
    if (level < 1 || level > m_maxLevel)
        return;
    m_level = level;
    m_actionGroupLevel->actions().at(level - 1)->setChecked(true);
    QSettings settings;
    settings.setValue(QString("level_") + to_string_id(m_bd.get_variant()),
                      m_level);
}

void MainWindow::setMoveMarkingAllNumber(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("move_marking", "all_number");
    updateWindow(false);
}

void MainWindow::setMoveMarkingLastDot(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("move_marking", "last_dot");
    updateWindow(false);
}

void MainWindow::setMoveMarkingLastNumber(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("move_marking", "last_number");
    updateWindow(false);
}

void MainWindow::setMoveMarkingNone(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("move_marking", "none");
    updateWindow(false);
}

void MainWindow::setPlayToolTip()
{
    m_actionPlay->setToolTip(
                m_computerColors[m_bd.get_to_play()] ?
                    tr("Make the computer continue to play the current color") :
                    tr("Make the computer play the current color"));
}

void MainWindow::setRated(bool isRated)
{
    m_isRated = isRated;
    if (isRated)
    {
        statusBar()->addWidget(m_ratedGameLabelText);
        m_ratedGameLabelText->show();
    }
    else if (m_ratedGameLabelText->isVisible())
        statusBar()->removeWidget(m_ratedGameLabelText);
}

void MainWindow::setSetupPlayer()
{
    if (! m_game.has_setup())
        m_game.remove_player();
    else
        m_game.set_player(m_bd.get_to_play());
}

void MainWindow::setTitleMenuLevel()
{
    QString title;
    switch (m_game.get_variant())
    {
    case Variant::classic:
        title = tr("&Level (Classic, 4 Players)");
        break;
    case Variant::classic_2:
        title = tr("&Level (Classic, 2 Players)");
        break;
    case Variant::classic_3:
        title = tr("&Level (Classic, 3 Players)");
        break;
    case Variant::duo:
        title = tr("&Level (Duo)");
        break;
    case Variant::trigon:
        title = tr("&Level (Trigon, 4 Players)");
        break;
    case Variant::trigon_2:
        title = tr("&Level (Trigon, 2 Players)");
        break;
    case Variant::trigon_3:
        title = tr("&Level (Trigon, 3 Players)");
        break;
    case Variant::junior:
        title = tr("&Level (Junior)");
        break;
    case Variant::nexos:
        title = tr("&Level (Nexos, 4 Players)");
        break;
    case Variant::nexos_2:
        title = tr("&Level (Nexos, 2 Players)");
        break;
    case Variant::callisto:
        title = tr("&Level (Callisto, 4 Players)");
        break;
    case Variant::callisto_2:
        title = tr("&Level (Callisto, 2 Players, 2 Colors)");
        break;
    case Variant::callisto_2_4:
        title = tr("&Level (Callisto, 2 Players, 4 Colors)");
        break;
    case Variant::callisto_3:
        title = tr("&Level (Callisto, 3 Players)");
        break;
    case Variant::gembloq:
        title = tr("&Level (GembloQ, 4 Players)");
        break;
    case Variant::gembloq_2:
        title = tr("&Level (GembloQ, 2 Players, 2 Colors)");
        break;
    case Variant::gembloq_2_4:
        title = tr("&Level (GembloQ, 2 Players, 4 Colors)");
        break;
    case Variant::gembloq_3:
        title = tr("&Level (GembloQ, 3 Players)");
        break;
    }
    m_menuLevel->setTitle(title);
}

void MainWindow::setupMode(bool enable)
{
    // Currently, we allow setup mode only if no moves have been played. It
    // should also work in inner nodes but this might be confusing for users
    // and violate some assumptions in the user interface (e.g. node depth is
    // equal to move number). Therefore, m_actionSetupMode is disabled if the
    // root node has children, but we still need to check for it here because
    // due to bugs in the Unitiy interface in Ubuntu 11.10, menu items are
    // not always disabled if the corresponding action is disabled.
    if (enable && m_game.get_root().has_children())
    {
        showInfo(tr("Setup mode cannot be used if moves have been played."));
        enable = false;
    }
    m_actionSetupMode->setChecked(enable);
    m_guiBoard->setFreePlacement(enable);
    if (enable)
    {
        m_setupModeLabel->show();
        for (Color c : m_bd.get_colors())
            m_pieceSelector[c]->setEnabled(true);
        m_computerColors.fill(false);
    }
    else
    {
        setSetupPlayer();
        m_setupModeLabel->hide();
        enablePieceSelector(m_bd.get_to_play());
        updateWindow(false);
    }
}

void MainWindow::setUtf8()
{
    m_game.set_charset("UTF-8");
    m_textCodec = QTextCodec::codecForName("UTF-8");
}

void MainWindow::showComment(bool checked)
{
    QSettings settings;
    bool wasVisible = m_comment->isVisible();
    if (wasVisible && ! checked)
        settings.setValue("splitter_state", m_splitter->saveState());
    settings.setValue("show_comment", checked);
    m_comment->setVisible(checked);
    if (! wasVisible && checked)
        m_splitter->restoreState(
                               settings.value("splitter_state").toByteArray());

}

void MainWindow::showError(const QString& text, const QString& infoText,
                           const QString& detailText)
{
    ::showError(this, text, infoText, detailText);
}

void MainWindow::showInfo(const QString& text, const QString& infoText,
                          const QString& detailText, bool withIcon)
{
    ::showInfo(this, text, infoText, detailText, withIcon);
}

void MainWindow::showInvalidFile(const QString& file, const exception& e)
{
    showError(tr("Error in file '%1'").arg(QFileInfo(file).fileName()),
              tr("The file is not a valid Blokus SGF file."), e.what());
}

void MainWindow::showInvalidSgf(const exception& e)
{
    showError(tr("Blokus SGF file format error"),
              tr("The file is not a valid Blokus SGF file."), e.what());
}

void MainWindow::showRating()
{
    if (m_ratingDialog == nullptr)
    {
        m_ratingDialog = new RatingDialog(this, *m_history);
        connect(m_ratingDialog, &RatingDialog::openRecentFile, [this](const QString& file) {
             if (checkSave())
                 openFile(file);
        });
    }
    loadHistory();
    m_ratingDialog->show();
}

void MainWindow::showStatus(const QString& text, bool temporary)
{
    int timeout = (temporary ? 4000 : 0);
    statusBar()->showMessage(text, timeout);
}

void MainWindow::showToolbar(bool checked)
{
    QSettings settings;
    settings.setValue("toolbar", checked);
    findChild<QToolBar*>()->setVisible(checked);
    m_menuToolBarText->setEnabled(checked);
}

QSize MainWindow::sizeHint() const
{
    auto geo = QApplication::desktop()->availableGeometry(this);
    return QSize(geo.width() * 2 / 3, min(geo.width() * 4 / 10, geo.height()));
}

void MainWindow::toolBarText(const QString& key, Qt::ToolButtonStyle style)
{
    QSettings settings;
    settings.setValue("toolbar_text", key);
    findChild<QToolBar*>()->setToolButtonStyle(style);
    if (m_helpWindow != nullptr)
        m_helpWindow->findChild<QToolBar*>()->setToolButtonStyle(style);
}

void MainWindow::truncate()
{
    auto& current = m_game.get_current();
    if (! current.has_parent())
        return;
    cancelThread();
    if (current.has_children())
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("Truncate this subtree?"),
                     tr("This position and all following moves and"
                        " variations will be removed from the game tree."));
        auto truncateButton =
            msgBox.addButton(tr("Truncate"),
                             QMessageBox::DestructiveRole);
        auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        if (msgBox.clickedButton() != truncateButton)
            return;
    }
    m_game.truncate();
    m_autoPlay = false;
    m_gameFinished = false;
    updateWindow(true);
}

void MainWindow::truncateChildren()
{
    if (! m_game.get_current().has_children())
        return;
    cancelThread();
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Truncate children?"),
                 tr("All following moves and variations will"
                    " be removed from the game tree."));
    auto truncateButton =
        msgBox.addButton(tr("Truncate Children"),
                         QMessageBox::DestructiveRole);
    auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();
    if (msgBox.clickedButton() != truncateButton)
        return;
    m_game.truncate_children();
    m_gameFinished = false;
    updateWindow(false);
}

void MainWindow::showVariations(bool checked)
{
    {
        QSettings settings;
        settings.setValue("show_variations", checked);
    }
    updateWindow(false);
}

void MainWindow::undo()
{
    auto& current = m_game.get_current();
    if (current.has_children() || ! m_game.get_tree().has_move(current)
            || ! current.has_parent())
        return;
    cancelThread();
    m_game.undo();
    m_autoPlay = false;
    m_gameFinished = false;
    updateWindow(true);
}

void MainWindow::updateComment()
{
    string comment = m_game.get_comment();
    QString text;
    if (! comment.empty())
        text = decode(comment);
    m_ignoreCommentTextChanged = true;
    m_comment->setPlainText(text);
    m_ignoreCommentTextChanged = false;
    if (! text.isEmpty())
        m_comment->ensureCursorVisible();
    m_comment->clearFocus();
}

void MainWindow::updateFlipActions()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    bool can_flip_horizontally =
        m_bd.get_piece_info(piece).can_flip_horizontally(transform);
    m_actionFlipHorizontally->setEnabled(can_flip_horizontally);
    bool can_flip_vertically =
        m_bd.get_piece_info(piece).can_flip_vertically(transform);
    m_actionFlipVertically->setEnabled(can_flip_vertically);
}

void MainWindow::updateMoveAnnotationActions()
{
    if (! m_game.get_tree().has_move(m_game.get_current()))
    {
        m_menuMoveAnnotation->setEnabled(false);
        return;
    }
    m_menuMoveAnnotation->setEnabled(true);
    double goodMove = m_game.get_good_move();
    if (goodMove > 1)
    {
        m_actionVeryGoodMove->setChecked(true);
        return;
    }
    if (goodMove > 0)
    {
        m_actionGoodMove->setChecked(true);
        return;
    }
    double badMove = m_game.get_bad_move();
    if (badMove > 1)
    {
        m_actionVeryBadMove->setChecked(true);
        return;
    }
    if (badMove > 0)
    {
        m_actionBadMove->setChecked(true);
        return;
    }
    if (m_game.is_interesting_move())
    {
        m_actionInterestingMove->setChecked(true);
        return;
    }
    if (m_game.is_doubtful_move())
    {
        m_actionDoubtfulMove->setChecked(true);
        return;
    }
    m_actionNoMoveAnnotation->setChecked(true);
}

void MainWindow::updateMoveNumber()
{
    auto& tree = m_game.get_tree();
    auto& current = m_game.get_current();
    unsigned move = get_move_number(tree, current);
    unsigned movesLeft = get_moves_left(tree, current);
    unsigned totalMoves = move + movesLeft;
    string variation = get_variation_string(current);
    QString text =
            QString::fromLocal8Bit(get_position_info(tree, current).c_str());
    QString toolTip;
    if (variation.empty())
    {
        if (movesLeft == 0)
        {
            if (move > 0)
                //: The argument is the current move number.
                toolTip = tr("Move %1").arg(move);
        }
        else
        {
            if (move == 0)
                //: The argument is the number of moves played in the main
                //: variation if the current position is the beginning (empty
                //: board or setup).
                toolTip = tr("%n move(s)", "", totalMoves);
            else
                //: The first argument is the current move number, the second
                //: is the number of moves in the main variation.
                toolTip = tr("Move %1 of %2").arg(QString::number(move),
                                                  QString::number(totalMoves));
        }
    }
    else
        //: The first argument is the current move number, the second is
        //: the number of moves in the current variation, the third is an
        //: identifier for the variation (e.g. 3b-4c).
        toolTip = tr("Move %1 of %2 in variation %3")
                  .arg(QString::number(move), QString::number(totalMoves),
                       variation.c_str());
    if (text.isEmpty())
    {
        if (m_moveNumber->isVisible())
            statusBar()->removeWidget(m_moveNumber);
    }
    else
    {
        m_moveNumber->setText(text);
        m_moveNumber->setToolTip(toolTip);
        if (! m_moveNumber->isVisible())
        {
            statusBar()->addPermanentWidget(m_moveNumber);
            m_moveNumber->show();
        }
    }
}

void MainWindow::updateRecentFiles()
{
    QSettings settings;
    auto files = settings.value("recent_files").toStringList();
    for (int i = 0; i < files.size(); ++i)
        if (! QFileInfo::exists(files[i]))
        {
            files.removeAt(i);
            --i;
        }
    int nuRecentFiles = files.size();
    if (nuRecentFiles > maxRecentFiles)
        nuRecentFiles = maxRecentFiles;
    m_menuOpenRecent->setEnabled(nuRecentFiles > 0);
    for (int i = 0; i < nuRecentFiles; ++i)
    {
        QFileInfo info = QFileInfo(files[i]);
        QString name = info.absoluteFilePath();
        // Don't prepend the filename by a number for a shortcut key
        // because the file name may contain underscores and Ubuntu Unity does
        // not handle this correctly (Unity bug #1390373)
        m_actionRecentFile[i]->setText(name);
        m_actionRecentFile[i]->setData(files[i]);
        m_actionRecentFile[i]->setVisible(true);
    }
    for (int j = nuRecentFiles; j < maxRecentFiles; ++j)
        m_actionRecentFile[j]->setVisible(false);
}

void MainWindow::updateWindow(bool currentNodeChanged)
{
    updateWindowModified();
    m_guiBoard->copyFromBoard(m_bd);
    QSettings settings;
    auto markVariations = settings.value("show_variations", true).toBool();
    unsigned nuMoves = m_bd.get_nu_moves();
    unsigned markMovesBegin, markMovesEnd;
    if (m_actionMoveMarkingAllNumber->isChecked())
    {
        markMovesBegin = 1;
        markMovesEnd = nuMoves;
    }
    else if (m_actionMoveMarkingLastNumber->isChecked()
             || m_actionMoveMarkingLastDot->isChecked())
    {
        markMovesBegin = nuMoves;
        markMovesEnd = nuMoves;
    }
    else
    {
        markMovesBegin = 0;
        markMovesEnd = 0;
    }
    gui_board_util::setMarkup(*m_guiBoard, m_game, markMovesBegin,
                              markMovesEnd, markVariations,
                              m_actionMoveMarkingLastDot->isChecked());
    m_scoreDisplay->updateScore(m_bd);
    if (m_legalMoves)
        m_legalMoves->clear();
    m_legalMoveIndex = 0;
    bool isGameOver = m_bd.is_game_over();
    auto to_play = m_bd.get_to_play();
    if (isGameOver && ! m_actionSetupMode->isChecked())
        m_orientationDisplay->clearSelectedColor();
    else
        m_orientationDisplay->selectColor(to_play);
    if (currentNodeChanged)
    {
        clearPiece();
        for (Color c : m_bd.get_colors())
            m_pieceSelector[c]->checkUpdate();
        if (! m_actionSetupMode->isChecked())
            enablePieceSelector(to_play);
        updateComment();
        updateMoveAnnotationActions();
    }
    updateMoveNumber();
    setPlayToolTip();
    auto& tree = m_game.get_tree();
    auto& current = m_game.get_current();
    bool isMain = is_main_variation(current);
    bool hasEarlierVariation = has_earlier_variation(current);
    bool hasParent = current.has_parent();
    bool hasChildren = current.has_children();
    bool hasMove = tree.has_move(current);
    bool hasMoves = m_bd.has_moves(to_play);
    bool isEmpty = libboardgame_sgf::is_empty(tree);
    bool hasNextVar = current.get_sibling() != nullptr;
    bool hasPrevVar = current.get_previous_sibling() != nullptr;
    m_actionAnalyzeGame->setEnabled(! m_isRated
                                    && tree.has_main_variation_moves());
    m_actionBackToMainVariation->setEnabled(! isMain);
    m_actionBeginning->setEnabled(! m_isRated && hasParent);
    m_actionBeginningOfBranch->setEnabled(hasEarlierVariation);
    m_actionBackward->setEnabled(! m_isRated && hasParent);
    m_actionComputerColors->setEnabled(! m_isRated);
    m_actionDeleteAllVariations->setEnabled(tree.has_variations());
    m_actionFindNextComment->setEnabled(! m_isRated);
    m_actionForward->setEnabled(hasChildren);
    m_actionEnd->setEnabled(hasChildren);
    m_actionFindMove->setEnabled(! isGameOver);
    m_actionGotoMove->setEnabled(! m_isRated &&
                                 hasCurrentVariationOtherMoves(tree, current));
    m_actionKeepOnlyPosition->setEnabled(! m_isRated
                                         && (hasParent || hasChildren)
                                         && m_bd.get_nu_onboard_pieces() > 0);
    m_actionKeepOnlySubtree->setEnabled(hasParent && hasChildren);
    m_actionGroupLevel->setEnabled(! m_isRated);
    m_actionMakeMainVariation->setEnabled(! isMain);
    m_actionMoveDownVariation->setEnabled(hasNextVar);
    m_actionMoveUpVariation->setEnabled(hasPrevVar);
    m_actionNew->setEnabled(! isEmpty);
    m_actionNextVariation->setEnabled(hasNextVar);
    if (! m_isGenMoveRunning)
    {
        m_actionNextPiece->setEnabled(! isGameOver);
        m_actionPreviousPiece->setEnabled(! isGameOver);
        m_actionPlay->setEnabled(! m_isRated && hasMoves);
        m_actionPlaySingleMove->setEnabled(! m_isRated && hasMoves);
    }
    m_actionPreviousVariation->setEnabled(hasPrevVar);
    m_actionRatedGame->setEnabled(! m_isRated);
    m_actionSave->setEnabled(! m_file.isEmpty() && m_game.is_modified());
    m_actionSaveAs->setEnabled(! isEmpty || m_game.is_modified());
    // See also comment in setupMode()
    m_actionSetupMode->setEnabled(! m_isRated && ! hasParent && ! hasChildren);
    m_actionNextColor->setEnabled(! m_isRated);
    m_actionTruncate->setEnabled(! m_isRated && hasParent);
    m_actionTruncateChildren->setEnabled(hasChildren);
    m_actionUndo->setEnabled(! m_isRated && hasParent && ! hasChildren
                             && hasMove);
    m_actionGroupVariant->setEnabled(! m_isRated);
    m_menuVariant->setEnabled(! m_isRated);
    setTitleMenuLevel();
}

void MainWindow::updateWindowModified()
{
    if (! m_file.isEmpty())
        setWindowModified(m_game.is_modified());
}

void MainWindow::variantTriggered(bool checked)
{
    if (checked)
        setVariant(Variant(qobject_cast<QAction*>(sender())->data().toInt()));
}

void MainWindow::veryBadMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_bad_move(2);
    updateWindow(false);
}

void MainWindow::veryGoodMove(bool checked)
{
    if (! checked)
        return;
    m_game.set_good_move(2);
    updateWindow(false);
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta() / 8 / 15;
    if (delta > 0)
    {
        if (! m_guiBoard->getSelectedPiece().is_null())
            for (int i = 0; i < delta; ++i)
                nextTransform();
    }
    else if (delta < 0)
    {
        if (! m_guiBoard->getSelectedPiece().is_null())
            for (int i = 0; i < -delta; ++i)
                previousTransform();
    }
    event->accept();
}

bool MainWindow::writeGame(const string& file)
{
    ofstream out(file);
    PentobiTreeWriter writer(out, m_game.get_tree());
    writer.set_indent(1);
    writer.write();
    return static_cast<bool>(out);
}

//-----------------------------------------------------------------------------
