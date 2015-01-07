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
#include <cstdlib>
#include <fstream>
#include <QAction>
#include <QApplication>
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
#include <QStatusBar>
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

using namespace std;
using Util::getPlayerString;
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::InvalidTree;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::back_to_main_variation;
using libboardgame_sgf::util::beginning_of_branch;
using libboardgame_sgf::util::find_next_comment;
using libboardgame_sgf::util::get_last_node;
using libboardgame_sgf::util::get_variation_string;
using libboardgame_sgf::util::has_comment;
using libboardgame_sgf::util::has_earlier_variation;
using libboardgame_sgf::util::is_main_variation;
using libboardgame_util::clear_abort;
using libboardgame_util::get_abort;
using libboardgame_util::log;
using libboardgame_util::set_abort;
using libboardgame_util::trim_right;
using libboardgame_util::ArrayList;
using libpentobi_base::ColorIterator;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::MoveList;
using libpentobi_base::PieceInfo;
using libpentobi_base::PentobiTree;
using libpentobi_base::PentobiTreeWriter;
using libpentobi_base::tree_util::get_move_number;
using libpentobi_base::tree_util::get_moves_left;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

namespace {

QToolButton* createOBoxToolButton(QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}

/** Return auto-save file name as a native path name. */
QString getAutoSaveFile()
{
    return Util::getDataDir() + QDir::separator() + "autosave.blksgf";
}

/** Determine the current color at the current node in the game.
    - If the color was explicitely set with a setup property, it will be used.
    - Otherwise, if the current node has children all with moves of the same
      color, this color will be used. This is because some trees like search
      dumps contain pass moves, so using the effective color to play is not
      appropriate there.
    - Otherwise, the effective color to play will be used. (see
      Game::get_effective_to_play())  */
Color getCurrentColor(const Game& game)
{
    auto& tree = game.get_tree();
    auto node = &game.get_current();
    Color c;
    while (node && ! tree.has_move(*node))
    {
        if (libpentobi_base::node_util::get_player(*node, c))
            return c;
        node = node->get_parent_or_null();
    }
    bool all_same_color = true;
    bool is_first = true;
    for (ChildIterator i(game.get_current()); i; ++i)
    {
        if (! tree.has_move(*i))
            continue;
        if (is_first)
        {
            c = tree.get_move(*i).color;
            is_first = false;
            continue;
        }
        if (tree.get_move(*i).color != c)
        {
            all_same_color = false;
            break;
        }
    }
    if (! is_first && all_same_color)
        return c;
    return game.get_effective_to_play();
}

bool hasCurrentVariationOtherMoves(const PentobiTree& tree,
                                   const SgfNode& current)
{
    auto node = current.get_parent_or_null();
    while (node)
    {
        if (! tree.get_move(*node).is_null())
            return true;
        node = node->get_parent_or_null();
    }
    node = current.get_first_child_or_null();
    while (node)
    {
        if (! tree.get_move(*node).is_null())
            return true;
        node = node->get_first_child_or_null();
    }
    return false;
}

void setIcon(QAction* action, const QString& name)
{
    QIcon icon(QString(":/pentobi/icons/%1.png").arg(name));
    QString file16 = QString(":/pentobi/icons/%1-16.png").arg(name);
    if (QFile::exists(file16))
        icon.addFile(file16, QSize(16, 16));
    action->setIcon(icon);
}

/** Simple heuristic that prefers moves with more piece points, more attach
    points and less adjacent points.
    Used for sorting the list used in Find Move. */
float getHeuristic(const Board& bd, Move mv)
{
    auto& info = bd.get_move_info(mv);
    auto& info_ext = bd.get_move_info_ext(mv);
    return static_cast<float>((1000 * info.size()
                               + 10 * info_ext.size_attach_points
                               - info_ext.size_adj_points));
}

} // namespace

//-----------------------------------------------------------------------------

MainWindow::MainWindow(const QString& initialFile, const QString& manualDir,
                       const QString& booksDir, bool noBook,
                       unsigned nu_threads, size_t memory)
    : m_noDelay(false),
      m_isGenMoveRunning(false),
      m_isAnalyzeRunning(false),
      m_autoPlay(true),
      m_isRated(false),
      m_ignoreCommentTextChanged(false),
      m_genMoveId(0),
      m_lastComputerMovesBegin(0),
      m_lastComputerMovesEnd(0),
      m_manualDir(manualDir),
      m_helpWindow(nullptr),
      m_ratingDialog(nullptr),
      m_analyzeGameWindow(nullptr),
      m_legalMoves(new MoveList()),
      m_leaveFullscreenButton(nullptr)
{
    Util::initDataDir();
    QSettings settings;
    auto variantString = settings.value("variant", "").toString();
    Variant variant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(), variant))
        variant = Variant::classic;
    m_game.reset(new Game(variant));
    m_history.reset(new RatingHistory(variant));
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
    m_player.reset(new Player(variant, booksDir.toLocal8Bit().constData(),
                              nu_threads, memory));
    m_player->get_search().set_callback(bind(&MainWindow::searchCallback,
                                             this, placeholders::_1,
                                             placeholders::_2));
    m_player->set_use_book(! noBook);
    createToolBar();
    connect(&m_genMoveWatcher, SIGNAL(finished()),
            SLOT(genMoveFinished()));
    connect(m_guiBoard, SIGNAL(play(Color, Move)),
            SLOT(placePiece(Color, Move)));
    connect(m_guiBoard, SIGNAL(pointClicked(Point)),
            SLOT(pointClicked(Point)));
    connect(m_actionMoveSelectedPieceLeft, SIGNAL(triggered()),
            m_guiBoard, SLOT(moveSelectedPieceLeft()));
    connect(m_actionMoveSelectedPieceRight, SIGNAL(triggered()),
            m_guiBoard, SLOT(moveSelectedPieceRight()));
    connect(m_actionMoveSelectedPieceUp, SIGNAL(triggered()),
            m_guiBoard, SLOT(moveSelectedPieceUp()));
    connect(m_actionMoveSelectedPieceDown, SIGNAL(triggered()),
            m_guiBoard, SLOT(moveSelectedPieceDown()));
    connect(m_actionPlaceSelectedPiece, SIGNAL(triggered()),
            m_guiBoard, SLOT(placeSelectedPiece()));
    createMenu();
    qApp->installEventFilter(this);
    updateRecentFiles();
    auto moveNumbers = settings.value("move_numbers", "last").toString();
    if (moveNumbers == "last")
        m_actionMoveNumbersLast->setChecked(true);
    else if (moveNumbers == "all")
        m_actionMoveNumbersAll->setChecked(true);
    else
        m_actionMoveNumbersNone->setChecked(true);
    auto coordinates = settings.value("coordinates", false).toBool();
    m_guiBoard->setCoordinates(coordinates);
    m_actionCoordinates->setChecked(coordinates);
    auto showToolbar = settings.value("toolbar", true).toBool();
    m_toolBar->setVisible(showToolbar);
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

    bool centerOnScreen = false;
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    if (restoreGeometry(settings.value("geometry").toByteArray()))
    {
        if (! screenGeometry.contains(geometry()))
        {
            if (width() > screenGeometry.width()
                    || height() > screenGeometry.height())
                adjustSize();
            centerOnScreen = true;
        }
    }
    else
    {
        adjustSize();
        centerOnScreen = true;
    }
    if (centerOnScreen)
    {
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

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
        if (open(initialFile))
            rememberFile(initialFile);
    }
    else
    {
        QString autoSaveFile = getAutoSaveFile();
        if (QFile(autoSaveFile).exists())
        {
            open(autoSaveFile, true);
            m_isAutoSaveLoaded = true;
            deleteAutoSaveFile();
            m_gameFinished = getBoard().is_game_over();
            updateWindow(true);
            if (settings.value("autosave_rated", false).toBool())
                QMetaObject::invokeMethod(this, "continueRatedGame",
                                          Qt::QueuedConnection);
        }
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Pentobi"),
                       "<style type=\"text/css\">"
                       ":link { text-decoration: none; }"
                       "</style>"
                       "<h2>" + tr("Pentobi") + "</h2>"
                       "<p>" + tr("Version %1").arg(getVersion()) + "</p>"
                       "<p>" +
                       tr("Pentobi is a computer opponent for the board game Blokus.")
                       + "<br>" +
                       tr("Copyright &copy; 2011&ndash;%1 Markus Enzenberger").arg(2015) +
                       + "<br>" +
                       "<a href=\"http://pentobi.sourceforge.net\">http://pentobi.sourceforge.net</a>"
                       "</p>");
}

void MainWindow::analyzeGame()
{
    if (! is_main_variation(m_game->get_current()))
    {
        showInfo(tr("Game analysis is only possible in the main variation."));
        return;
    }
    AnalyzeSpeedDialog dialog(this, tr("Analyze Game"));
    if (! dialog.exec())
        return;
    int speed = dialog.getSpeedValue();
    cancelThread();
    if (m_analyzeGameWindow)
        delete m_analyzeGameWindow;
    m_analyzeGameWindow = new AnalyzeGameWindow(this);
    // Make sure all action shortcuts work when the analyze dialog has the
    // focus apart from m_actionLeaveFullscreen because the Esc key is used
    // to close the dialog
    m_analyzeGameWindow->addActions(actions());
    m_analyzeGameWindow->removeAction(m_actionLeaveFullscreen);
    m_analyzeGameWindow->show();
    m_isAnalyzeRunning = true;
    connect(m_analyzeGameWindow->analyzeGameWidget, SIGNAL(finished()),
            SLOT(analyzeGameFinished()));
    connect(m_analyzeGameWindow->analyzeGameWidget,
            SIGNAL(gotoPosition(Variant,const vector<ColorMove>&)),
            SLOT(gotoPosition(Variant,const vector<ColorMove>&)));
    size_t nuSimulations;
    switch (speed)
    {
    case 0:
        nuSimulations = 5000;
        break;
    case 1:
        nuSimulations = 20000;
        break;
    default:
        nuSimulations = 80000;
    }
    m_analyzeGameWindow->analyzeGameWidget->start(*m_game,
                                                  m_player->get_search(),
                                                  nuSimulations);
}

void MainWindow::analyzeGameFinished()
{
    m_analyzeGameWindow->analyzeGameWidget
        ->setCurrentPosition(*m_game, m_game->get_current());
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
    result.move = m_player->genmove(getBoard(), c);
    auto elapsed = timer.elapsed();
    // Enforce minimum thinking time of 0.8 sec
    if (elapsed < 800 && ! m_noDelay)
    {
        // Use std::thread because QThread::sleep() is protected in Qt4
        chrono::milliseconds duration(800 - elapsed);
        this_thread::sleep_for(duration);
    }
    return result;
}

void MainWindow::badMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_bad_move();
    updateWindow(false);
}

void MainWindow::backward()
{
    auto& node = m_game->get_current();
    if (! node.has_parent())
        return;
    gotoNode(node.get_parent());
}

void MainWindow::backward10()
{
    auto& tree = m_game->get_tree();
    auto node = &m_game->get_current();
    unsigned n = 0;
    while (n < 10)
    {
        if (tree.has_move(*node))
            ++n;
        auto parent = node->get_parent_or_null();
        if (! parent)
            break;
        node = parent;
    }
    gotoNode(*node);
}

void MainWindow::backToMainVariation()
{
    gotoNode(back_to_main_variation(m_game->get_current()));
}

void MainWindow::beginning()
{
    gotoNode(m_game->get_root());
}

void MainWindow::beginningOfBranch()
{
    gotoNode(beginning_of_branch(m_game->get_current()));
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
    if (! m_autoPlay || ! isComputerToPlay() || getBoard().is_game_over())
        m_lastComputerMovesBegin = 0;
    else if (! m_isGenMoveRunning)
        genMove();
}

bool MainWindow::checkSave()
{
    if (! m_file.isEmpty())
    {
        if (! m_game->is_modified())
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
        return (result == discardButton);
    }
    // Don't ask if game should be saved if it was finished because the user
    // might only want to play and never save games.
    if (m_game->get_tree().get_root().has_children() && ! m_gameFinished)
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The current game is not finished."),
                     tr("Do you want to abort the game?"));
        auto abortGameButton =
            msgBox.addButton(tr("&Abort Game"), QMessageBox::DestructiveRole);
        auto cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        if (msgBox.clickedButton() != abortGameButton)
            return false;
        return true;
    }
    return true;
}

bool MainWindow::checkQuit()
{
    if (! m_file.isEmpty() && m_game->is_modified())
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
        return (result == discardButton);
    }
    cancelThread();
    QSettings settings;
    if (m_file.isEmpty() && ! m_gameFinished
        && (m_game->is_modified() || m_isAutoSaveLoaded))
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

void MainWindow::clearSelectedPiece()
{
    m_actionRotatePieceClockwise->setEnabled(false);
    m_actionRotatePieceAnticlockwise->setEnabled(false);
    m_actionFlipPieceHorizontally->setEnabled(false);
    m_actionFlipPieceVertically->setEnabled(false);
    m_actionClearSelectedPiece->setEnabled(false);
    m_guiBoard->clearSelectedPiece();
    m_orientationDisplay->clearSelectedPiece();
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
        m_game->set_comment("");
    else
    {
        string charset = m_game->get_root().get_property("CA", "");
        string value = Util::convertSgfValueFromQString(comment, charset);
        trim_right(value);
        m_game->set_comment(value);
    }
    updateWindowModified();
}

void MainWindow::computerColors()
{
    ColorMap<bool> oldComputerColors = m_computerColors;
    auto variant = getVariant();
    ComputerColorDialog dialog(this, variant, m_computerColors);
    dialog.exec();
    auto nu_colors = getBoard().get_nu_nonalt_colors();

    bool computerNone = true;
    for (ColorIterator i(nu_colors); i; ++i)
        if (m_computerColors[*i])
        {
            computerNone = false;
            break;
        }
    QSettings settings;
    settings.setValue("computer_color_none", computerNone);

    // Enable auto play only if any color has changed because that means that
    // the user probably wants to continue playing, otherwise the user could
    // have only opened the dialog to check the current settings
    for (ColorIterator i(nu_colors); i; ++i)
        if (m_computerColors[*i] != oldComputerColors[*i])
        {
            m_autoPlay = true;
            break;
        }

    checkComputerMove();
}

bool MainWindow::computerPlaysAll() const
{
    for (ColorIterator i(getBoard().get_nu_nonalt_colors()); i; ++i)
        if (! m_computerColors[*i])
            return false;
    return true;
}

void MainWindow::continueRatedGame()
{
    auto& bd = getBoard();
    auto nuColors = bd.get_nu_colors();
    QSettings settings;
    auto color =
            static_cast<Color::IntType>(
                settings.value("autosave_rated_color", 0).toUInt());
    if (color >= nuColors)
        return;
    m_ratedGameColor = Color(color);
    m_computerColors.fill(true);
    for (ColorIterator i(nuColors); i; ++i)
        if (bd.is_same_player(*i, m_ratedGameColor))
            m_computerColors[*i] = false;
    setRated(true);
    updateWindow(false);
    showInfo(tr("Continuing unfinished rated game."),
             tr("You play %1 in this game.")
             .arg(getPlayerString(bd.get_variant(), m_ratedGameColor)));
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
    // the menu bar, shortcuts won,t work in fullscreen mode because the menu
    // is not visible in fullscreen mode
    addAction(action);
    return action;
}

void MainWindow::createActions()
{
    auto groupVariant = new QActionGroup(this);
    auto groupLevel = new QActionGroup(this);
    auto groupMoveNumbers = new QActionGroup(this);
    auto groupMoveAnnotation = new QActionGroup(this);
    auto groupToolBarText = new QActionGroup(this);

    m_actionAbout = createAction(tr("&About"));
    m_actionAbout->setIcon(QIcon::fromTheme("help-about"));
    connect(m_actionAbout, SIGNAL(triggered()), SLOT(about()));

    m_actionAnalyzeGame = createAction(tr("&Analyze Game..."));
    connect(m_actionAnalyzeGame, SIGNAL(triggered()), SLOT(analyzeGame()));

    m_actionBackward = createAction(tr("B&ackward"));
    m_actionBackward->setToolTip(tr("Go one move backward"));
    m_actionBackward->setPriority(QAction::LowPriority);
    setIcon(m_actionBackward, "pentobi-backward");
    m_actionBackward->setShortcut(QString("Ctrl+Left"));
    connect(m_actionBackward, SIGNAL(triggered()), SLOT(backward()));

    m_actionBackward10 = createAction(tr("10 Back&ward"));
    m_actionBackward10->setToolTip(tr("Go ten moves backward"));
    m_actionBackward10->setPriority(QAction::LowPriority);
    setIcon(m_actionBackward10, "pentobi-backward10");
    m_actionBackward10->setShortcut(QString("Ctrl+Shift+Left"));
    connect(m_actionBackward10, SIGNAL(triggered()), SLOT(backward10()));

    m_actionBackToMainVariation = createAction(tr("Back to &Main Variation"));
    m_actionBackToMainVariation->setShortcut(QString("Ctrl+M"));
    connect(m_actionBackToMainVariation, SIGNAL(triggered()),
            SLOT(backToMainVariation()));

    m_actionBadMove = createAction(tr("&Bad"));
    m_actionBadMove->setActionGroup(groupMoveAnnotation);
    m_actionBadMove->setCheckable(true);
    connect(m_actionBadMove, SIGNAL(triggered(bool)), SLOT(badMove(bool)));

    m_actionBeginning = createAction(tr("&Beginning"));
    m_actionBeginning->setToolTip(tr("Go to beginning of game"));
    m_actionBeginning->setPriority(QAction::LowPriority);
    setIcon(m_actionBeginning, "pentobi-beginning");
    m_actionBeginning->setShortcut(QString("Ctrl+Home"));
    connect(m_actionBeginning, SIGNAL(triggered()), SLOT(beginning()));

    m_actionBeginningOfBranch = createAction(tr("Beginning of Bran&ch"));
    m_actionBeginningOfBranch->setShortcut(QString("Ctrl+B"));
    connect(m_actionBeginningOfBranch, SIGNAL(triggered()),
            SLOT(beginningOfBranch()));

    m_actionClearSelectedPiece = createAction(tr("Clear Piece"));
    setIcon(m_actionClearSelectedPiece, "pentobi-piece-clear");
    m_actionClearSelectedPiece->setShortcut(QString("0"));
    connect(m_actionClearSelectedPiece, SIGNAL(triggered()),
            SLOT(clearSelectedPiece()));

    m_actionComputerColors = createAction(tr("&Computer Colors"));
    m_actionComputerColors->setShortcut(QString("Ctrl+U"));
    m_actionComputerColors->setToolTip(
                                  tr("Set the colors played by the computer"));
    setIcon(m_actionComputerColors, "pentobi-computer-colors");
    connect(m_actionComputerColors, SIGNAL(triggered()),
            SLOT(computerColors()));

    m_actionCoordinates = createAction(tr("C&oordinates"));
    m_actionCoordinates->setCheckable(true);
    connect(m_actionCoordinates, SIGNAL(triggered(bool)),
            SLOT(coordinates(bool)));

    m_actionDeleteAllVariations = createAction(tr("&Delete All Variations"));
    connect(m_actionDeleteAllVariations, SIGNAL(triggered()),
            SLOT(deleteAllVariations()));

    m_actionDoubtfulMove = createAction(tr("&Doubtful"));
    m_actionDoubtfulMove->setActionGroup(groupMoveAnnotation);
    m_actionDoubtfulMove->setCheckable(true);
    connect(m_actionDoubtfulMove, SIGNAL(triggered(bool)),
            SLOT(doubtfulMove(bool)));

    m_actionEnd = createAction(tr("&End"));
    m_actionEnd->setToolTip(tr("Go to end of moves"));
    m_actionEnd->setPriority(QAction::LowPriority);
    m_actionEnd->setShortcut(QString("Ctrl+End"));
    setIcon(m_actionEnd, "pentobi-end");
    connect(m_actionEnd, SIGNAL(triggered()), SLOT(end()));

    m_actionExportAsciiArt = createAction(tr("&ASCII Art"));
    connect(m_actionExportAsciiArt, SIGNAL(triggered()),
            SLOT(exportAsciiArt()));

    m_actionExportImage = createAction(tr("I&mage"));
    connect(m_actionExportImage, SIGNAL(triggered()), SLOT(exportImage()));

    m_actionFindMove = createAction(tr("&Find Move"));
    m_actionFindMove->setShortcut(QString("F6"));
    connect(m_actionFindMove, SIGNAL(triggered()), SLOT(findMove()));

    m_actionFindNextComment = createAction(tr("Find Next &Comment"));
    m_actionFindNextComment->setShortcut(QString("F3"));
    connect(m_actionFindNextComment, SIGNAL(triggered()),
            SLOT(findNextComment()));

    m_actionFlipPieceHorizontally = createAction(tr("Flip Horizontally"));
    setIcon(m_actionFlipPieceHorizontally, "pentobi-flip-horizontal");
    connect(m_actionFlipPieceHorizontally, SIGNAL(triggered()),
            SLOT(flipPieceHorizontally()));

    m_actionFlipPieceVertically = createAction(tr("Flip Vertically"));
    setIcon(m_actionFlipPieceVertically, "pentobi-flip-vertical");

    m_actionForward = createAction(tr("&Forward"));
    m_actionForward->setToolTip(tr("Go one move forward"));
    m_actionForward->setPriority(QAction::LowPriority);
    m_actionForward->setShortcut(QString("Ctrl+Right"));
    setIcon(m_actionForward, "pentobi-forward");
    connect(m_actionForward, SIGNAL(triggered()), SLOT(forward()));

    m_actionForward10 = createAction(tr("10 F&orward"));
    m_actionForward10->setToolTip(tr("Go ten moves forward"));
    m_actionForward10->setPriority(QAction::LowPriority);
    m_actionForward10->setShortcut(QString("Ctrl+Shift+Right"));
    setIcon(m_actionForward10, "pentobi-forward10");
    connect(m_actionForward10, SIGNAL(triggered()), SLOT(forward10()));

    m_actionFullscreen = createAction(tr("&Fullscreen"));
    m_actionFullscreen->setIcon(QIcon::fromTheme("view-fullscreen"));
    m_actionFullscreen->setShortcut(QString("F11"));
    connect(m_actionFullscreen, SIGNAL(triggered()), SLOT(fullscreen()));

    m_actionGameInfo = createAction(tr("Ga&me Info"));
    m_actionGameInfo->setShortcut(QString("Ctrl+I"));
    connect(m_actionGameInfo, SIGNAL(triggered()), SLOT(gameInfo()));

    m_actionGoodMove = createAction(tr("&Good"));
    m_actionGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionGoodMove->setCheckable(true);
    connect(m_actionGoodMove, SIGNAL(triggered(bool)), SLOT(goodMove(bool)));

    m_actionGotoMove = createAction(tr("&Go to Move..."));
    m_actionGotoMove->setShortcut(QString("Ctrl+G"));
    connect(m_actionGotoMove, SIGNAL(triggered()), SLOT(gotoMove()));

    m_actionHelp = createAction(tr("&Contents"));
    m_actionHelp->setIcon(QIcon::fromTheme("help-contents"));
    m_actionHelp->setShortcut(QKeySequence::HelpContents);
    connect(m_actionHelp, SIGNAL(triggered()), SLOT(help()));

    m_actionInterestingMove = createAction(tr("I&nteresting"));
    m_actionInterestingMove->setActionGroup(groupMoveAnnotation);
    m_actionInterestingMove->setCheckable(true);
    connect(m_actionInterestingMove, SIGNAL(triggered(bool)),
            SLOT(interestingMove(bool)));

    m_actionInterrupt = createAction(tr("St&op"));
    m_actionInterrupt->setIcon(QIcon::fromTheme("process-stop"));
    m_actionInterrupt->setEnabled(false);
    connect(m_actionInterrupt, SIGNAL(triggered()), SLOT(interrupt()));

    m_actionInterruptPlay = createAction();
    m_actionInterruptPlay->setShortcut(QString("Shift+Esc"));
    connect(m_actionInterruptPlay, SIGNAL(triggered()), SLOT(interruptPlay()));

    m_actionKeepOnlyPosition = createAction(tr("&Keep Only Position"));
    connect(m_actionKeepOnlyPosition, SIGNAL(triggered()),
            SLOT(keepOnlyPosition()));

    m_actionKeepOnlySubtree = createAction(tr("Keep Only &Subtree"));
    connect(m_actionKeepOnlySubtree, SIGNAL(triggered()),
            SLOT(keepOnlySubtree()));

    m_actionLeaveFullscreen = createAction(tr("Leave Fullscreen"));
    m_actionLeaveFullscreen->setShortcut(QString("Esc"));
    connect(m_actionLeaveFullscreen, SIGNAL(triggered()),
            SLOT(leaveFullscreen()));

    m_actionMakeMainVariation = createAction(tr("M&ake Main Variation"));
    connect(m_actionMakeMainVariation, SIGNAL(triggered()),
            SLOT(makeMainVariation()));

    m_actionMoveDownVariation = createAction(tr("Move Variation D&own"));
    connect(m_actionMoveDownVariation, SIGNAL(triggered()),
            SLOT(moveDownVariation()));

    m_actionMoveUpVariation = createAction(tr("Move Variation &Up"));
    connect(m_actionMoveUpVariation, SIGNAL(triggered()),
            SLOT(moveUpVariation()));

    static_assert(maxLevel == 9, "");
    QString levelText[maxLevel] =
        { tr("&1"), tr("&2"), tr("&3"), tr("&4"), tr("&5"), tr("&6"),
          tr("&7"), tr("&8"), tr("&9") };
    for (int i = 0; i < maxLevel; ++i)
        m_actionLevel[i] = createLevelAction(groupLevel, i + 1, levelText[i]);
    connect(m_actionFlipPieceVertically, SIGNAL(triggered()),
            SLOT(flipPieceVertically()));

    m_actionMoveNumbersAll = createAction(tr("&All"));
    m_actionMoveNumbersAll->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersAll->setCheckable(true);
    connect(m_actionMoveNumbersAll, SIGNAL(triggered(bool)),
            SLOT(setMoveNumbersAll(bool)));

    m_actionMoveNumbersLast = createAction(tr("&Last"));
    m_actionMoveNumbersLast->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersLast->setCheckable(true);
    m_actionMoveNumbersLast->setChecked(true);
    connect(m_actionMoveNumbersLast, SIGNAL(triggered(bool)),
            SLOT(setMoveNumbersLast(bool)));

    m_actionMoveNumbersNone = createAction(tr("&None", "move numbers"));
    m_actionMoveNumbersNone->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersNone->setCheckable(true);
    connect(m_actionMoveNumbersNone, SIGNAL(triggered(bool)),
            SLOT(setMoveNumbersNone(bool)));

    m_actionMoveSelectedPieceLeft = createAction();
    m_actionMoveSelectedPieceLeft->setShortcut(QString("Left"));

    m_actionMoveSelectedPieceRight = createAction();
    m_actionMoveSelectedPieceRight->setShortcut(QString("Right"));

    m_actionMoveSelectedPieceUp = createAction();
    m_actionMoveSelectedPieceUp->setShortcut(QString("Up"));

    m_actionMoveSelectedPieceDown = createAction();
    m_actionMoveSelectedPieceDown->setShortcut(QString("Down"));

    m_actionNextPiece = createAction(tr("Next Piece"));
    setIcon(m_actionNextPiece, "pentobi-next-piece");
    m_actionNextPiece->setShortcut(QString("+"));
    connect(m_actionNextPiece, SIGNAL(triggered()), SLOT(nextPiece()));

    m_actionNextTransform = createAction();
    m_actionNextTransform->setShortcut(QString("Space"));
    connect(m_actionNextTransform, SIGNAL(triggered()), SLOT(nextTransform()));

    m_actionNextVariation = createAction(tr("&Next Variation"));
    m_actionNextVariation->setToolTip(tr("Go to next variation"));
    m_actionNextVariation->setPriority(QAction::LowPriority);
    m_actionNextVariation->setShortcut(QString("Ctrl+Down"));
    setIcon(m_actionNextVariation, "pentobi-next-variation");
    connect(m_actionNextVariation, SIGNAL(triggered()), SLOT(nextVariation()));

    m_actionNextVariation10 = createAction();
    m_actionNextVariation10->setShortcut(QString("Ctrl+Shift+Down"));
    connect(m_actionNextVariation10, SIGNAL(triggered()),
            SLOT(nextVariation10()));

    m_actionNew = createAction(tr("&New"));
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setToolTip(tr("Start a new game"));
    setIcon(m_actionNew, "pentobi-newgame");
    connect(m_actionNew, SIGNAL(triggered()), SLOT(newGame()));

    m_actionNoMoveAnnotation = createAction(tr("N&one", "move annotation"));
    m_actionNoMoveAnnotation->setActionGroup(groupMoveAnnotation);
    m_actionNoMoveAnnotation->setCheckable(true);
    connect(m_actionNoMoveAnnotation, SIGNAL(triggered(bool)),
            SLOT(noMoveAnnotation(bool)));

    m_actionOpen = createAction(tr("&Open..."));
    m_actionOpen->setIcon(QIcon::fromTheme("document-open"));
    m_actionOpen->setShortcut(QKeySequence::Open);
    connect(m_actionOpen, SIGNAL(triggered()), SLOT(open()));
    m_actionPlaceSelectedPiece = createAction();
    m_actionPlaceSelectedPiece->setShortcut(QString("Return"));

    m_actionPlay = createAction(tr("&Play"));
    m_actionPlay->setShortcut(QString("Ctrl+L"));
    setIcon(m_actionPlay, "pentobi-play");
    connect(m_actionPlay, SIGNAL(triggered()), SLOT(play()));

    m_actionPlaySingleMove = createAction(tr("Play &Single Move"));
    m_actionPlaySingleMove->setShortcut(QString("Ctrl+Shift+L"));
    connect(m_actionPlaySingleMove, SIGNAL(triggered()),
            SLOT(playSingleMove()));

    m_actionPreviousPiece = createAction(tr("Previous Piece"));
    setIcon(m_actionPreviousPiece, "pentobi-previous-piece");
    m_actionPreviousPiece->setShortcut(QString("-"));
    connect(m_actionPreviousPiece, SIGNAL(triggered()),
            SLOT(previousPiece()));

    m_actionPreviousTransform = createAction();
    m_actionPreviousTransform->setShortcut(QString("Shift+Space"));
    connect(m_actionPreviousTransform, SIGNAL(triggered()),
            SLOT(previousTransform()));

    m_actionPreviousVariation = createAction(tr("&Previous Variation"));
    m_actionPreviousVariation->setToolTip(tr("Go to previous variation"));
    m_actionPreviousVariation->setPriority(QAction::LowPriority);
    m_actionPreviousVariation->setShortcut(QString("Ctrl+Up"));
    setIcon(m_actionPreviousVariation, "pentobi-previous-variation");
    connect(m_actionPreviousVariation, SIGNAL(triggered()),
            SLOT(previousVariation()));

    m_actionPreviousVariation10 = createAction();
    m_actionPreviousVariation10->setShortcut(QString("Ctrl+Shift+Up"));
    connect(m_actionPreviousVariation10, SIGNAL(triggered()),
            SLOT(previousVariation10()));

    m_actionRatedGame = createAction(tr("&Rated Game"));
    m_actionRatedGame->setToolTip(tr("Start a new rated game"));
    m_actionRatedGame->setShortcut(QString("Ctrl+Shift+N"));
    setIcon(m_actionRatedGame, "pentobi-rated-game");
    connect(m_actionRatedGame, SIGNAL(triggered()), SLOT(ratedGame()));

    for (int i = 0; i < maxRecentFiles; ++i)
    {
         m_actionRecentFile[i] = createAction();
         m_actionRecentFile[i]->setVisible(false);
         connect(m_actionRecentFile[i], SIGNAL(triggered()),
                 SLOT(openRecentFile()));
     }

    m_actionRotatePieceAnticlockwise = createAction(tr("Rotate Anticlockwise"));
    setIcon(m_actionRotatePieceAnticlockwise, "pentobi-rotate-left");
    connect(m_actionRotatePieceAnticlockwise, SIGNAL(triggered()),
            SLOT(rotatePieceAnticlockwise()));

    m_actionRotatePieceClockwise = createAction(tr("Rotate Clockwise"));
    setIcon(m_actionRotatePieceClockwise, "pentobi-rotate-right");
    connect(m_actionRotatePieceClockwise, SIGNAL(triggered()),
            SLOT(rotatePieceClockwise()));

    m_actionQuit = createAction(tr("&Quit"));
    m_actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    m_actionQuit->setShortcut(QKeySequence::Quit);
    connect(m_actionQuit, SIGNAL(triggered()), SLOT(close()));

    m_actionSave = createAction(tr("&Save"));
    m_actionSave->setIcon(QIcon::fromTheme("document-save"));
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, SIGNAL(triggered()), SLOT(save()));

    m_actionSaveAs = createAction(tr("Save &As..."));
    m_actionSaveAs->setIcon(QIcon::fromTheme("document-save-as"));
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));

    m_actionSelectNextColor = createAction(tr("Select Next &Color"));
    m_actionSelectNextColor->setShortcut(QString("Ctrl+C"));
    connect(m_actionSelectNextColor, SIGNAL(triggered()),
            SLOT(selectNextColor()));

    m_actionSelectPiece1 = createAction();
    m_actionSelectPiece1->setShortcut(QString("1"));
    connect(m_actionSelectPiece1, SIGNAL(triggered()), SLOT(selectPiece1()));

    m_actionSelectPiece2 = createAction();
    m_actionSelectPiece2->setShortcut(QString("2"));
    connect(m_actionSelectPiece2, SIGNAL(triggered()), SLOT(selectPiece2()));

    m_actionSelectPieceA = createAction();
    m_actionSelectPieceA->setShortcut(QString("A"));
    connect(m_actionSelectPieceA, SIGNAL(triggered()), SLOT(selectPieceA()));

    m_actionSelectPieceC = createAction();
    m_actionSelectPieceC->setShortcut(QString("C"));
    connect(m_actionSelectPieceC, SIGNAL(triggered()), SLOT(selectPieceC()));

    m_actionSelectPieceF = createAction();
    m_actionSelectPieceF->setShortcut(QString("F"));
    connect(m_actionSelectPieceF, SIGNAL(triggered()), SLOT(selectPieceF()));

    m_actionSelectPieceG = createAction();
    m_actionSelectPieceG->setShortcut(QString("G"));
    connect(m_actionSelectPieceG, SIGNAL(triggered()), SLOT(selectPieceG()));

    m_actionSelectPieceI = createAction();
    m_actionSelectPieceI->setShortcut(QString("I"));
    connect(m_actionSelectPieceI, SIGNAL(triggered()), SLOT(selectPieceI()));

    m_actionSelectPieceL = createAction();
    m_actionSelectPieceL->setShortcut(QString("L"));
    connect(m_actionSelectPieceL, SIGNAL(triggered()), SLOT(selectPieceL()));

    m_actionSelectPieceN = createAction();
    m_actionSelectPieceN->setShortcut(QString("N"));
    connect(m_actionSelectPieceN, SIGNAL(triggered()), SLOT(selectPieceN()));

    m_actionSelectPieceO = createAction();
    m_actionSelectPieceO->setShortcut(QString("O"));
    connect(m_actionSelectPieceO, SIGNAL(triggered()), SLOT(selectPieceO()));

    m_actionSelectPieceP = createAction();
    m_actionSelectPieceP->setShortcut(QString("P"));
    connect(m_actionSelectPieceP, SIGNAL(triggered()), SLOT(selectPieceP()));

    m_actionSelectPieceS = createAction();
    m_actionSelectPieceS->setShortcut(QString("S"));
    connect(m_actionSelectPieceS, SIGNAL(triggered()), SLOT(selectPieceS()));

    m_actionSelectPieceT = createAction();
    m_actionSelectPieceT->setShortcut(QString("T"));
    connect(m_actionSelectPieceT, SIGNAL(triggered()), SLOT(selectPieceT()));

    m_actionSelectPieceU = createAction();
    m_actionSelectPieceU->setShortcut(QString("U"));
    connect(m_actionSelectPieceU, SIGNAL(triggered()), SLOT(selectPieceU()));

    m_actionSelectPieceV = createAction();
    m_actionSelectPieceV->setShortcut(QString("V"));
    connect(m_actionSelectPieceV, SIGNAL(triggered()), SLOT(selectPieceV()));

    m_actionSelectPieceW = createAction();
    m_actionSelectPieceW->setShortcut(QString("W"));
    connect(m_actionSelectPieceW, SIGNAL(triggered()), SLOT(selectPieceW()));

    m_actionSelectPieceX = createAction();
    m_actionSelectPieceX->setShortcut(QString("X"));
    connect(m_actionSelectPieceX, SIGNAL(triggered()), SLOT(selectPieceX()));

    m_actionSelectPieceY = createAction();
    m_actionSelectPieceY->setShortcut(QString("Y"));
    connect(m_actionSelectPieceY, SIGNAL(triggered()), SLOT(selectPieceY()));

    m_actionSelectPieceZ = createAction();
    m_actionSelectPieceZ->setShortcut(QString("Z"));
    connect(m_actionSelectPieceZ, SIGNAL(triggered()), SLOT(selectPieceZ()));

    m_actionSetupMode = createAction(tr("S&etup Mode"));
    m_actionSetupMode->setCheckable(true);
    connect(m_actionSetupMode, SIGNAL(triggered(bool)), SLOT(setupMode(bool)));

    m_actionShowComment = createAction(tr("&Comment"));
    m_actionShowComment->setCheckable(true);
    m_actionShowComment->setShortcut(QString("Ctrl+T"));
    connect(m_actionShowComment, SIGNAL(triggered(bool)),
            SLOT(showComment(bool)));

    m_actionRating = createAction(tr("&Rating"));
    m_actionRating->setShortcut(QString("F7"));
    connect(m_actionRating, SIGNAL(triggered()), SLOT(showRating()));

    m_actionToolBarNoText = createAction(tr("&No Text"));
    m_actionToolBarNoText->setActionGroup(groupToolBarText);
    m_actionToolBarNoText->setCheckable(true);
    connect(m_actionToolBarNoText, SIGNAL(triggered(bool)),
            SLOT(toolBarNoText(bool)));

    m_actionToolBarTextBesideIcons = createAction(tr("Text &Beside Icons"));
    m_actionToolBarTextBesideIcons->setActionGroup(groupToolBarText);
    m_actionToolBarTextBesideIcons->setCheckable(true);
    connect(m_actionToolBarTextBesideIcons, SIGNAL(triggered(bool)),
            SLOT(toolBarTextBesideIcons(bool)));

    m_actionToolBarTextBelowIcons = createAction(tr("Text Bel&ow Icons"));
    m_actionToolBarTextBelowIcons->setActionGroup(groupToolBarText);
    m_actionToolBarTextBelowIcons->setCheckable(true);
    connect(m_actionToolBarTextBelowIcons, SIGNAL(triggered(bool)),
            SLOT(toolBarTextBelowIcons(bool)));

    m_actionToolBarTextOnly = createAction(tr("&Text Only"));
    m_actionToolBarTextOnly->setActionGroup(groupToolBarText);
    m_actionToolBarTextOnly->setCheckable(true);
    connect(m_actionToolBarTextOnly, SIGNAL(triggered(bool)),
            SLOT(toolBarTextOnly(bool)));

    m_actionToolBarTextSystem = createAction(tr("&System Default"));
    m_actionToolBarTextSystem->setActionGroup(groupToolBarText);
    m_actionToolBarTextSystem->setCheckable(true);
    connect(m_actionToolBarTextSystem, SIGNAL(triggered(bool)),
            SLOT(toolBarTextSystem(bool)));

    m_actionTruncate = createAction(tr("&Truncate"));
    connect(m_actionTruncate, SIGNAL(triggered()), SLOT(truncate()));

    m_actionTruncateChildren = createAction(tr("Truncate C&hildren"));
    connect(m_actionTruncateChildren, SIGNAL(triggered()),
             SLOT(truncateChildren()));

    m_actionShowToolbar = createAction(tr("&Toolbar"));
    m_actionShowToolbar->setCheckable(true);
    connect(m_actionShowToolbar, SIGNAL(triggered(bool)),
            SLOT(showToolbar(bool)));

    m_actionShowVariations = createAction(tr("Show &Variations"));
    m_actionShowVariations->setCheckable(true);
    connect(m_actionShowVariations, SIGNAL(triggered(bool)),
            SLOT(showVariations(bool)));

    m_actionUndo = createAction(tr("&Undo Move"));
    connect(m_actionUndo, SIGNAL(triggered()), SLOT(undo()));

    m_actionVariantClassic = createAction(tr("&Classic (4 Players)"));
    m_actionVariantClassic->setActionGroup(groupVariant);
    m_actionVariantClassic->setCheckable(true);
    connect(m_actionVariantClassic, SIGNAL(triggered(bool)),
            SLOT(variantClassic(bool)));

    m_actionVariantClassic3 = createAction(tr("Classic (&3 Players)"));
    m_actionVariantClassic3->setActionGroup(groupVariant);
    m_actionVariantClassic3->setCheckable(true);
    connect(m_actionVariantClassic3, SIGNAL(triggered(bool)),
            SLOT(variantClassic3(bool)));

    m_actionVariantClassic2 = createAction(tr("Classic (&2 Players)"));
    m_actionVariantClassic2->setActionGroup(groupVariant);
    m_actionVariantClassic2->setCheckable(true);
    connect(m_actionVariantClassic2, SIGNAL(triggered(bool)),
            SLOT(variantClassic2(bool)));

    m_actionVariantDuo = createAction(tr("&Duo"));
    m_actionVariantDuo->setActionGroup(groupVariant);
    m_actionVariantDuo->setCheckable(true);
    connect(m_actionVariantDuo, SIGNAL(triggered(bool)),
            SLOT(variantDuo(bool)));

    m_actionVariantJunior = createAction(tr("J&unior"));
    m_actionVariantJunior->setActionGroup(groupVariant);
    m_actionVariantJunior->setCheckable(true);
    connect(m_actionVariantJunior, SIGNAL(triggered(bool)),
            SLOT(variantJunior(bool)));

    m_actionVariantTrigon = createAction(tr("&Trigon (4 Players)"));
    m_actionVariantTrigon->setActionGroup(groupVariant);
    m_actionVariantTrigon->setCheckable(true);
    connect(m_actionVariantTrigon, SIGNAL(triggered(bool)),
            SLOT(variantTrigon(bool)));

    m_actionVariantTrigon2 = createAction(tr("Tri&gon (2 Players)"));
    m_actionVariantTrigon2->setActionGroup(groupVariant);
    m_actionVariantTrigon2->setCheckable(true);
    connect(m_actionVariantTrigon2, SIGNAL(triggered(bool)),
            SLOT(variantTrigon2(bool)));

    m_actionVariantTrigon3 = createAction(tr("Trigo&n (3 Players)"));
    m_actionVariantTrigon3->setActionGroup(groupVariant);
    m_actionVariantTrigon3->setCheckable(true);
    connect(m_actionVariantTrigon3, SIGNAL(triggered(bool)),
            SLOT(variantTrigon3(bool)));

    m_actionVeryBadMove = createAction(tr("V&ery Bad"));
    m_actionVeryBadMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryBadMove->setCheckable(true);
    connect(m_actionVeryBadMove, SIGNAL(triggered(bool)),
            SLOT(veryBadMove(bool)));

    m_actionVeryGoodMove = createAction(tr("&Very Good"));
    m_actionVeryGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryGoodMove->setCheckable(true);
    connect(m_actionVeryGoodMove, SIGNAL(triggered(bool)),
            SLOT(veryGoodMove(bool)));
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
    m_guiBoard = new GuiBoard(nullptr, getBoard());
    m_splitter->addWidget(m_guiBoard);
    m_comment = new QPlainTextEdit;
    m_comment->setTabChangesFocus(true);
    connect(m_comment, SIGNAL(textChanged()), SLOT(commentChanged()));
    m_splitter->addWidget(m_comment);
    m_splitter->setStretchFactor(0, 85);
    m_splitter->setStretchFactor(1, 15);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    return m_splitter;
}

QAction* MainWindow::createLevelAction(QActionGroup* group, int level,
                                       const QString& text)
{
    LIBBOARDGAME_ASSERT(level >= 1 && level <= maxLevel);
    auto action = createAction(text);
    action->setCheckable(true);
    if (level == m_level)
        action->setChecked(true);
    action->setActionGroup(group);
    action->setData(level);
    connect(action, SIGNAL(triggered(bool)), SLOT(setLevel(bool)));
    return action;
}

void MainWindow::createMenu()
{
    auto menuGame = menuBar()->addMenu(tr("&Game"));
    menuGame->addAction(m_actionNew);
    menuGame->addAction(m_actionRatedGame);
    menuGame->addSeparator();
    m_menuVariant = menuGame->addMenu(tr("Game &Variant"));
    m_menuVariant->addAction(m_actionVariantClassic);
    m_menuVariant->addAction(m_actionVariantClassic3);
    m_menuVariant->addAction(m_actionVariantClassic2);
    m_menuVariant->addAction(m_actionVariantDuo);
    m_menuVariant->addAction(m_actionVariantTrigon);
    m_menuVariant->addAction(m_actionVariantTrigon3);
    m_menuVariant->addAction(m_actionVariantTrigon2);
    m_menuVariant->addAction(m_actionVariantJunior);
    menuGame->addAction(m_actionGameInfo);
    menuGame->addSeparator();
    menuGame->addAction(m_actionUndo);
    menuGame->addAction(m_actionFindMove);
    menuGame->addSeparator();
    menuGame->addAction(m_actionOpen);
    m_menuOpenRecent = menuGame->addMenu(tr("Open R&ecent"));
    m_menuOpenRecent->setIcon(QIcon::fromTheme("document-open-recent"));
    for (int i = 0; i < maxRecentFiles; ++i)
        m_menuOpenRecent->addAction(m_actionRecentFile[i]);
    menuGame->addSeparator();
    menuGame->addAction(m_actionSave);
    menuGame->addAction(m_actionSaveAs);
    auto menuExport = menuGame->addMenu(tr("E&xport"));
    menuExport->addAction(m_actionExportImage);
    menuExport->addAction(m_actionExportAsciiArt);
    menuGame->addSeparator();
    menuGame->addAction(m_actionQuit);

    auto menuGo = menuBar()->addMenu(tr("G&o"));
    menuGo->addAction(m_actionBeginning);
    menuGo->addAction(m_actionBackward10);
    menuGo->addAction(m_actionBackward);
    menuGo->addAction(m_actionForward);
    menuGo->addAction(m_actionForward10);
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
    m_menuMoveAnnotation->addAction(m_actionGoodMove);
    m_menuMoveAnnotation->addAction(m_actionVeryGoodMove);
    m_menuMoveAnnotation->addAction(m_actionBadMove);
    m_menuMoveAnnotation->addAction(m_actionVeryBadMove);
    m_menuMoveAnnotation->addAction(m_actionInterestingMove);
    m_menuMoveAnnotation->addAction(m_actionDoubtfulMove);
    m_menuMoveAnnotation->addAction(m_actionNoMoveAnnotation);
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
    menuEdit->addAction(m_actionSelectNextColor);

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
    auto menuMoveNumbers = menuView->addMenu(tr("&Move Numbers"));
    menuMoveNumbers->addAction(m_actionMoveNumbersLast);
    menuMoveNumbers->addAction(m_actionMoveNumbersAll);
    menuMoveNumbers->addAction(m_actionMoveNumbersNone);
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
    m_menuLevel = menuComputer->addMenu(tr("&Level"));
    for (int i = 0; i < maxLevel; ++i)
        m_menuLevel->addAction(m_actionLevel[i]);

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
    layout->addWidget(createOBoxToolButton(m_actionRotatePieceAnticlockwise),
                      0, 0);
    layout->addWidget(createOBoxToolButton(m_actionRotatePieceClockwise),
                      0, 1);
    layout->addWidget(createOBoxToolButton(m_actionFlipPieceHorizontally),
                      1, 0);
    layout->addWidget(createOBoxToolButton(m_actionFlipPieceVertically),
                      1, 1);
    outerLayout->addStretch();
    outerLayout->addLayout(layout);
    outerLayout->addStretch();
    return outerLayout;
}

QLayout* MainWindow::createOrientationButtonBoxRight()
{
    auto outerLayout = new QVBoxLayout;
    auto layout = new QGridLayout;
    layout->addWidget(createOBoxToolButton(m_actionPreviousPiece),
                      0, 0);
    layout->addWidget(createOBoxToolButton(m_actionNextPiece),
                      0, 1);
    layout->addWidget(createOBoxToolButton(m_actionClearSelectedPiece),
                      1, 0, 1, 2, Qt::AlignHCenter);
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
    m_orientationDisplay = new OrientationDisplay(nullptr, getBoard());
    connect(m_orientationDisplay, SIGNAL(colorClicked(Color)),
            SLOT(orientationDisplayColorClicked(Color)));
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
    auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addLayout(createOrientationSelector(), 20);
    m_scoreDisplay = new ScoreDisplay;
    layout->addWidget(m_scoreDisplay, 6);
    auto pieceSelectorLayout = new SameHeightLayout;
    layout->addLayout(pieceSelectorLayout, 80);
    for (ColorIterator i(Color::range); i; ++i)
    {
        m_pieceSelector[*i] = new PieceSelector(nullptr, getBoard(), *i);
        connect(m_pieceSelector[*i],
                SIGNAL(pieceSelected(Color,Piece,const Transform*)),
                SLOT(selectPiece(Color,Piece,const Transform*)));
        pieceSelectorLayout->addWidget(m_pieceSelector[*i]);
    }
    return layout;
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
    bool currentNodeChanges = ! is_main_variation(m_game->get_current());
    if (currentNodeChanges)
        cancelThread();
    m_game->delete_all_variations();
    updateWindow(currentNodeChanges);
}

void MainWindow::doubtfulMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_doubtful_move();
    updateWindow(false);
}

void MainWindow::createToolBar()
{
    m_toolBar = new QToolBar;
    m_toolBar->setMovable(false);
    m_toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    m_toolBar->addAction(m_actionNew);
    m_toolBar->addAction(m_actionRatedGame);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionComputerColors);
    m_toolBar->addAction(m_actionPlay);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionBeginning);
    m_toolBar->addAction(m_actionBackward10);
    m_toolBar->addAction(m_actionBackward);
    m_toolBar->addAction(m_actionForward);
    m_toolBar->addAction(m_actionForward10);
    m_toolBar->addAction(m_actionEnd);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionNextVariation);
    m_toolBar->addAction(m_actionPreviousVariation);
    addToolBar(m_toolBar);
    QSettings settings;
    auto toolBarText = settings.value("toolbar_text", "system").toString();
    if (toolBarText == "no_text")
    {
        m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_actionToolBarNoText->setChecked(true);
    }
    else if (toolBarText == "beside_icons")
    {
        m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        m_actionToolBarTextBesideIcons->setChecked(true);
    }
    else if (toolBarText == "below_icons")
    {
        m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_actionToolBarTextBelowIcons->setChecked(true);
    }
    else if (toolBarText == "text_only")
    {
        m_toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
        m_actionToolBarTextOnly->setChecked(true);
    }
    else
    {
        m_toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        m_actionToolBarTextSystem->setChecked(true);
    }
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
    for (ColorIterator i(getBoard().get_nu_colors()); i; ++i)
    {
        m_pieceSelector[*i]->checkUpdate();
        m_pieceSelector[*i]->setEnabled(*i == c);
    }
}

void MainWindow::end()
{
    gotoNode(get_last_node(m_game->get_current()));
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    // By default, Qt 4.7 shows status tips in the status bar if the mouse
    // goes over a menu. This is undesirable because it deletes the current
    // text in the status line (e.g. the "The computer is thinking..." status)
    if (event->type() == QEvent::StatusTip)
        return true;
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::exportAsciiArt()
{
    QString file = QFileDialog::getSaveFileName(this, "", getLastDir(),
                                                tr("Text files (*.txt)"));
    if (file.isEmpty())
        return;
    rememberDir(file);
    ofstream out(file.toLocal8Bit().constData());
    auto& bd = getBoard();
    bd.write(out, false);
    if (! out)
        showError(strerror(errno));
}

void MainWindow::exportImage()
{
    QSettings settings;
    auto size = settings.value("export_image_size", 420).toInt();
    QInputDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags()
                          & ~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle(qApp->translate("ExportImage", "Export Image"));
    dialog.setLabelText(qApp->translate("ExportImage", "Image size:"));
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(0, 2147483647);
    dialog.setIntStep(40);
    dialog.setIntValue(size);
    if (! dialog.exec())
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
    auto& bd = getBoard();
    boardPainter.paintEmptyBoard(painter, size, size, bd.get_variant(),
                                 bd.get_geometry());
    boardPainter.paintPieces(painter, bd.get_grid(), &m_guiBoard->getLabels());
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
        else
            showError(writer.errorString());
    }
}

void MainWindow::findMove()
{
    auto& bd = getBoard();
    if (bd.is_game_over())
        return;
    if (m_legalMoves->empty())
    {
        bd.gen_moves(m_currentColor, *m_legalMoves);
        sort(m_legalMoves->begin(), m_legalMoves->end(),
             [&](Move mv1, Move mv2) {
                 return getHeuristic(bd, mv1) > getHeuristic(bd, mv2);
             });
    }
    if (m_legalMoves->empty())
    {
        // m_currentColor must have moves if game is not over
        LIBBOARDGAME_ASSERT(false);
        return;
    }
    if (m_legalMoveIndex >= m_legalMoves->size())
        m_legalMoveIndex = 0;
    auto mv = (*m_legalMoves)[m_legalMoveIndex];
    selectPiece(m_currentColor, bd.get_move_info(mv).get_piece());
    m_guiBoard->showMove(m_currentColor, mv);
    ++m_legalMoveIndex;
}

void MainWindow::findNextComment()
{
    auto& root = m_game->get_root();
    auto& current = m_game->get_current();
    auto node = find_next_comment(current);
    if (! node && &current != &root)
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
    if (! node)
    {
        showInfo(tr("No comment found"));
        return;
    }
    showComment(true);
    gotoNode(*node);
}

void MainWindow::flipPieceHorizontally()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto& bd = getBoard();
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = bd.get_transforms().get_mirrored_horizontally(transform);
    transform = bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::flipPieceVertically()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    auto& bd = getBoard();
    transform = bd.get_transforms().get_mirrored_vertically(transform);
    transform = bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::forward()
{
    auto node = m_game->get_current().get_first_child_or_null();
    if (! node)
        return;
    gotoNode(*node);
}

void MainWindow::forward10()
{
    auto& tree = m_game->get_tree();
    auto node = &m_game->get_current();
    unsigned n = 0;
    while (n < 10)
    {
        if (tree.has_move(*node))
            ++n;
        auto child = node->get_first_child_or_null();
        if (! child)
            break;
        node = child;
    }
    gotoNode(*node);
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
    m_toolBar->hide();
    settings.setValue("geometry", saveGeometry());
    showFullScreen();
    if (! m_leaveFullscreenButton)
        m_leaveFullscreenButton =
            new LeaveFullscreenButton(this, m_actionLeaveFullscreen);
    m_leaveFullscreenButton->showButton();
}

void MainWindow::gameInfo()
{
    GameInfoDialog dialog(this, *m_game);
    dialog.exec();
    updateWindow(false);
}

void MainWindow::gameOver()
{
    auto variant = getVariant();
    auto& bd = getBoard();
    QString info;
    if (variant == Variant::duo || variant == Variant::junior)
    {
        int score = bd.get_score(Color(0));
        if (score > 0)
            info = tr("Blue wins with %n point(s).", "", score);
        else if (score < 0)
            info = tr("Green wins with %n point(s).", "", -score);
        else
            info = tr("The game ends in a tie.");
    }
    else if (variant == Variant::classic_2 || variant == Variant::trigon_2)
    {
        int score = bd.get_score(Color(0));
        if (score > 0)
            info = tr("Blue/Red wins with %n point(s).", "", score);
        else if (score < 0)
            info = tr("Yellow/Green wins with %n point(s).", "", -score);
        else
            info = tr("The game ends in a tie.");
    }
    else if (variant == Variant::classic_3 || variant == Variant::trigon_3)
    {
        auto blue = bd.get_points(Color(0));
        auto yellow = bd.get_points(Color(1));
        auto red = bd.get_points(Color(2));
        auto maxPoints = max(blue, max(yellow, red));
        if (blue == yellow && yellow == red)
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
        LIBBOARDGAME_ASSERT(variant == Variant::classic
                            || variant == Variant::trigon);
        auto blue = bd.get_points(Color(0));
        auto yellow = bd.get_points(Color(1));
        auto red = bd.get_points(Color(2));
        auto green = bd.get_points(Color(3));
        auto maxPoints = max(blue, max(yellow, max(red, green)));
        if (blue == yellow && yellow == red && red == green)
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
        bd.get_place(m_ratedGameColor, place, isPlaceShared);
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
            gameResult, date, m_level, m_game->get_tree());
        if (m_ratingDialog)
            m_ratingDialog->updateContent();
        int newRating = m_history->getRating().to_int();
        if (newRating > oldRating)
            detailText =
            tr("Your rating has increased from %1 to %2.")
            .arg(oldRating).arg(newRating);
        else if (newRating == oldRating)
            detailText = tr("Your rating stays at %1.").arg(oldRating);
        else
            detailText =
            tr("Your rating has decreased from %1 to %2.")
            .arg(oldRating).arg(newRating);
        setRated(false);
        QSettings settings;
        auto key = QString("next_rated_random_") + to_string_id(getVariant());
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
    showStatus(tr("The computer is thinking..."));
    clearSelectedPiece();
    clear_abort();
    m_lastRemainingSeconds = 0;
    m_lastRemainingMinutes = 0;
    m_player->set_level(m_level);
    QFuture<GenMoveResult> future =
        QtConcurrent::run(this, &MainWindow::asyncGenMove, m_currentColor,
                          m_genMoveId, playSingleMove);
    m_genMoveWatcher.setFuture(future);
    m_isGenMoveRunning = true;
    auto& bd = getBoard();
    unsigned nuMoves = bd.get_nu_moves();
    if (m_lastComputerMovesBegin == 0 && ! computerPlaysAll())
    {
        m_lastComputerMovesBegin = nuMoves + 1;
        m_lastComputerMovesEnd = m_lastComputerMovesBegin;
    }
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
    auto& bd = getBoard();
    Color c = result.color;
    auto mv = result.move;
    if (mv.is_null() || ! bd.is_legal(c, mv))
    {
        // No need to translate this message, it should never occur if the
        // program is correct
        showError("Computer failed to generate a move");
        return;
    }
    if (mv.is_pass())
        return;
    m_lastComputerMovesEnd = bd.get_nu_moves() + 1;
    play(c, mv);
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
    if (dir.isEmpty() || ! QFileInfo(dir).exists())
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        dir = QStandardPaths::writableLocation(
                                          QStandardPaths::DocumentsLocation);
#else
        dir = QDesktopServices::storageLocation(
                                          QDesktopServices::DocumentsLocation);
#endif
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
    m_game->set_good_move();
    updateWindow(false);
}

void MainWindow::gotoMove()
{
    QSettings settings;
    vector<const SgfNode*> nodes;
    auto& tree = m_game->get_tree();
    auto node = &m_game->get_current();
    do
    {
        if (! tree.get_move(*node).is_null())
            nodes.insert(nodes.begin(), node);
        node = node->get_parent_or_null();
    }
    while (node);
    node = m_game->get_current().get_first_child_or_null();
    while (node)
    {
        if (! tree.get_move(*node).is_null())
            nodes.push_back(node);
        node = node->get_first_child_or_null();
    }
    int maxMoves = int(nodes.size());
    if (maxMoves == 0)
        return;
    int defaultValue = getBoard().get_nu_moves();
    if (defaultValue == 0)
        defaultValue = maxMoves;
    QInputDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags()
                          & ~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle(tr("Go to Move"));
    dialog.setLabelText(tr("Move number:"));
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(1, static_cast<int>(nodes.size()));
    dialog.setIntStep(1);
    dialog.setIntValue(defaultValue);
    if (! dialog.exec())
        return;
    gotoNode(*nodes[dialog.intValue() - 1]);
}

void MainWindow::gotoNode(const SgfNode& node)
{
    cancelThread();
    leaveSetupMode();
    try
    {
        m_game->goto_node(node);
    }
    catch (const InvalidTree& e)
    {
        showInvalidFile(m_file, e);
        return;
    }
    m_currentColor = getCurrentColor(*m_game);
    m_lastComputerMovesBegin = 0;
    if (m_analyzeGameWindow && m_analyzeGameWindow->isVisible())
        m_analyzeGameWindow->analyzeGameWidget
            ->setCurrentPosition(*m_game, node);
    m_autoPlay = false;
    updateWindow(true);
}

void MainWindow::gotoPosition(Variant variant,
                              const vector<ColorMove>& moves)
{
    if (getVariant() != variant)
        return;
    auto& tree = m_game->get_tree();
    auto node = &tree.get_root();
    if (tree.has_move_ignore_invalid(*node))
        // Move in root node not supported.
        return;
    for (ColorMove mv : moves)
    {
        bool found = false;
        for (ChildIterator i(*node); i; ++i)
            if (tree.get_move(*i) == mv)
            {
                found = true;
                node = &(*i);
                break;
            }
        if (! found)
            return;
    }
    gotoNode(*node);
}

void MainWindow::help()
{
    if (m_helpWindow)
    {
        m_helpWindow->show();
        m_helpWindow->raise();
        return;
    }
    QString path = HelpWindow::findMainPage(m_manualDir, "index.html",
                                            QLocale::system().name());
    m_helpWindow = new HelpWindow(nullptr, path);
    m_helpWindow->show();
}

void MainWindow::initGame()
{
    setRated(false);
    if (m_analyzeGameWindow)
    {
        delete m_analyzeGameWindow;
        m_analyzeGameWindow = nullptr;
    }
    m_game->init();
    m_game->set_charset("UTF-8");
#ifdef VERSION
    m_game->set_application("Pentobi", VERSION);
#else
    m_game->set_application("Pentobi");
#endif
    m_game->set_date_today();
    m_game->clear_modified();
    QSettings settings;
    if (! settings.value("computer_color_none").toBool())
    {
        auto& bd = getBoard();
        for (ColorIterator i(bd.get_nu_nonalt_colors()); i; ++i)
            m_computerColors[*i] = ! bd.is_same_player(*i, Color(0));
        m_autoPlay = true;
    }
    else
    {
        m_computerColors.fill(false);
        m_autoPlay = false;
    }
    m_currentColor = Color(0);
    leaveSetupMode();
    m_lastComputerMovesBegin = 0;
    m_gameFinished = false;
    m_isAutoSaveLoaded = false;
    setFile("");
}

void MainWindow::initVariantActions()
{
    switch (getVariant())
    {
    case Variant::classic:
        m_actionVariantClassic->setChecked(true);
        break;
    case Variant::classic_2:
        m_actionVariantClassic2->setChecked(true);
        break;
    case Variant::classic_3:
        m_actionVariantClassic3->setChecked(true);
        break;
    case Variant::duo:
        m_actionVariantDuo->setChecked(true);
        break;
    case Variant::junior:
        m_actionVariantJunior->setChecked(true);
        break;
    case Variant::trigon:
        m_actionVariantTrigon->setChecked(true);
        break;
    case Variant::trigon_2:
        m_actionVariantTrigon2->setChecked(true);
        break;
    case Variant::trigon_3:
        m_actionVariantTrigon3->setChecked(true);
        break;
    }
}

void MainWindow::initPieceSelectors()
{
    auto& bd = getBoard();
    for (Color::IntType i = 0; i < Color::range; ++i)
    {
        bool isVisible = (i < bd.get_nu_colors());
        m_pieceSelector[Color(i)]->setVisible(isVisible);
        if (isVisible)
            m_pieceSelector[Color(i)]->init();
    }
}

void MainWindow::interestingMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_interesting_move();
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
    if (m_game->get_variant() != Variant::classic_3
            || m_currentColor != Color(3))
        return m_computerColors[m_currentColor];
    auto& bd = m_game->get_board();
    return m_computerColors[Color(bd.get_alt_player())];
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
    m_game->keep_only_position();
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
    m_game->keep_only_subtree();
    updateWindow(true);
}

void MainWindow::leaveFullscreen()
{
    if (! isFullScreen())
        return;
    QSettings settings;
    auto showToolbar = settings.value("toolbar", true).toBool();
    menuBar()->show();
    m_toolBar->setVisible(showToolbar);
    // m_leaveFullscreenButton can be null if the window was put in fullscreen
    // mode by a "generic" method by the window manager (e.g. the title bar
    // menu on KDE) and not by MainWindow::fullscreen()
    if (m_leaveFullscreenButton)
        m_leaveFullscreenButton->hideButton();
    showNormal();
}

void MainWindow::leaveSetupMode()
{
    if (! m_actionSetupMode->isChecked())
        return;
    setupMode(false);
}

void MainWindow::loadHistory()
{
    auto variant = m_game->get_variant();
    if (m_history->getVariant() == variant)
        return;
    m_history->load(variant);
    if (m_ratingDialog)
        m_ratingDialog->updateContent();
}

void MainWindow::makeMainVariation()
{
    m_game->make_main_variation();
    updateWindow(false);
}

void MainWindow::moveDownVariation()
{
    m_game->move_down_variation();
    updateWindow(false);
}

void MainWindow::moveUpVariation()
{
    m_game->move_up_variation();
    updateWindow(false);
}

void MainWindow::nextPiece()
{
    auto& bd = getBoard();
    if ( bd.get_pieces_left(m_currentColor).empty())
        return;
    auto nuUniqPieces = bd.get_nu_uniq_pieces();
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
        if (bd.is_piece_left(m_currentColor, Piece(i)))
            break;
        ++i;
    }
    selectPiece(m_currentColor, Piece(i));
}

void MainWindow::nextTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = getBoard().get_piece_info(piece).get_next_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::nextVariation()
{
    auto node = m_game->get_current().get_sibling();
    if (! node)
        return;
    gotoNode(*node);
}

void MainWindow::nextVariation10()
{
    auto node = &m_game->get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        if (! node->get_sibling())
            break;
        node = node->get_sibling();
    }
    gotoNode(*node);
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
    m_game->remove_move_annotation();
    updateWindow(false);
}

void MainWindow::open()
{
    if (! checkSave())
        return;
    QSettings settings;
    QString file = QFileDialog::getOpenFileName(this, tr("Open"), getLastDir(),
                                                getFilter());
    if (file.isEmpty())
        return;
    rememberDir(file);
    if (open(file))
        rememberFile(file);
}

bool MainWindow::open(const QString& file, bool isTemporary)
{
    if (file.isEmpty())
        return false;
    cancelThread();
    TreeReader reader;
    ifstream in(file.toLocal8Bit().constData());
    try
    {
        reader.read(in);
    }
    catch (const TreeReader::ReadError& e)
    {
        if (! in)
        {
            QString text =
                tr("Could not read file '%1'").arg(QFileInfo(file).fileName());
            showError(text, strerror(errno));
        }
        else
        {
            showInvalidFile(file, e);
        }
        return false;
    }
    m_isAutoSaveLoaded = false;
    if (! isTemporary)
    {
        setFile(file);
        deleteAutoSaveFile();
    }
    if (m_analyzeGameWindow)
    {
        delete m_analyzeGameWindow;
        m_analyzeGameWindow = nullptr;
    }
    setRated(false);
    try
    {
        auto tree = reader.get_tree_transfer_ownership();
        m_game->init(tree);
        if (! libpentobi_base::node_util::has_setup(m_game->get_root()))
            m_game->goto_node(get_last_node(m_game->get_root()));
        m_currentColor = getCurrentColor(*m_game);
        initPieceSelectors();
    }
    catch (const InvalidTree& e)
    {
        showInvalidFile(file, e);
    }
    m_computerColors.fill(false);
    m_autoPlay = false;
    leaveSetupMode();
    m_lastComputerMovesBegin = 0;
    initVariantActions();
    restoreLevel(getVariant());
    updateWindow(true);
    loadHistory();
    return true;
}

void MainWindow::openRecentFile()
{
     auto action = qobject_cast<QAction*>(sender());
     if (! action)
         return;
     if (! checkSave())
         return;
     open(action->data().toString());
}

void MainWindow::orientationDisplayColorClicked(Color)
{
    if (m_actionSetupMode->isChecked())
        selectNextColor();
}

void MainWindow::placePiece(Color c, Move mv)
{
    cancelThread();
    auto& bd = m_game->get_board();
    bool isSetupMode = m_actionSetupMode->isChecked();
    bool isAltColor =
            (bd.get_variant() == Variant::classic_3 && c.to_int() == 3);
    if ((! isAltColor && m_computerColors[c])
            || (isAltColor && m_computerColors[Color(bd.get_alt_player())])
            || isSetupMode)
        // If the user enters a move previously played by the computer (e.g.
        // after undoing moves) then it is unlikely that the user wants to keep
        // the computer color settings.
        m_computerColors.fill(false);
    if (isSetupMode)
    {
        m_game->add_setup(c, mv);
        setSetupPlayer();
        updateWindow(true);
    }
    else
    {
        play(c, mv);
        updateWindow(true);
        checkComputerMove();
    }
}

void MainWindow::play()
{
    cancelThread();
    leaveSetupMode();
    auto variant = getVariant();
    if (variant != Variant::classic && variant != Variant::trigon
            && variant != Variant::trigon_3)
    {
        QSettings settings;
        settings.setValue("computer_color_none", false);
    }
    if (! isComputerToPlay())
    {
        m_computerColors.fill(false);
        if (variant == Variant::classic_2 || variant == Variant::trigon_2)
        {
            if (m_currentColor == Color(0) || m_currentColor == Color(2))
                m_computerColors[Color(0)] = m_computerColors[Color(2)] = true;
            else
                m_computerColors[Color(1)] = m_computerColors[Color(3)] = true;
        }
        else if (variant == Variant::classic_3 && m_currentColor.to_int() == 3)
            m_computerColors[Color(getBoard().get_alt_player())] = true;
        else
            m_computerColors[m_currentColor] = true;
    }
    m_autoPlay = true;
    m_lastComputerMovesBegin = 0;
    genMove();
}

void MainWindow::play(Color c, Move mv)
{
    auto& bd = getBoard();
    m_game->play(c, mv, false);
    c = m_game->get_to_play();
    m_gameFinished = false;
    if (bd.is_game_over())
    {
        updateWindow(true);
        repaint();
        gameOver();
        m_gameFinished = true;
        deleteAutoSaveFile();
        return;
    }
    m_currentColor = m_game->get_effective_to_play();
}

void MainWindow::playSingleMove()
{
    cancelThread();
    leaveSetupMode();
    m_autoPlay = false;
    m_lastComputerMovesBegin = 0;
    genMove(true);
}

void MainWindow::pointClicked(Point p)
{
    if (! m_actionSetupMode->isChecked())
        return;
    auto& bd = getBoard();
    PointState s = bd.get_point_state(p);
    if (s.is_empty())
        return;
    m_game->remove_setup(s.to_color(), bd.get_move_at(p));
    setSetupPlayer();
    updateWindow(true);
}

void MainWindow::previousPiece()
{
    auto& bd = getBoard();
    if (bd.get_pieces_left(m_currentColor).empty())
        return;
    auto nuUniqPieces = bd.get_nu_uniq_pieces();
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
        if (bd.is_piece_left(m_currentColor, Piece(i)))
            break;
    }
    selectPiece(m_currentColor, Piece(i));
}

void MainWindow::previousTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform =
        getBoard().get_piece_info(piece).get_previous_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::previousVariation()
{
    auto node = m_game->get_current().get_previous_sibling();
    if (! node)
        return;
    gotoNode(*node);
}

void MainWindow::previousVariation10()
{
    auto node = &m_game->get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        if (! node->get_previous_sibling())
            break;
        node = node->get_previous_sibling();
    }
    gotoNode(*node);
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
    auto key = QString("next_rated_random_") + to_string_id(getVariant());
    if (settings.contains(key))
        random = settings.value(key).toUInt();
    else
    {
        // RandomGenerator::ResultType may be larger than unsigned
        random = static_cast<unsigned>(m_random.generate() % 1000);
        settings.setValue(key, random);
    }
    m_history->getNextRatedGameSettings(maxLevel, random,
                                        level, m_ratedGameColor);
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Start new rated game?"),
                 "<html>" +
                 tr("In the next game, you will play %1 against"
                    " Pentobi level&nbsp;%2.")
                 .arg(getPlayerString(getVariant(), m_ratedGameColor))
                 .arg(level));
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
    auto& bd = getBoard();
    m_computerColors.fill(true);
    for (ColorIterator i(bd.get_nu_nonalt_colors()); i; ++i)
        if (bd.is_same_player(*i, m_ratedGameColor))
            m_computerColors[*i] = false;
    m_autoPlay = true;
    QString computerPlayerName =
        //: The first argument is the version of Pentobi
        tr("Pentobi %1 (level %2)").arg(getVersion()).arg(level);
    string charset = m_game->get_root().get_property("CA", "");
    string computerPlayerNameStdStr =
        Util::convertSgfValueFromQString(computerPlayerName, charset);
    string humanPlayerNameStdStr =
        Util::convertSgfValueFromQString(tr("Human"), charset);
    for (ColorIterator i(bd.get_nu_nonalt_colors()); i; ++i)
        if (m_computerColors[*i])
            m_game->set_player_name(*i, computerPlayerNameStdStr);
        else
            m_game->set_player_name(*i, humanPlayerNameStdStr);
    // Setting the player names marks the game as modified but there is nothing
    // important that would need to be saved yet
    m_game->clear_modified();
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
    QFileInfo info(canonicalFile);
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
    if (m_level < 1 || m_level > maxLevel)
        m_level = 1;
    m_actionLevel[m_level - 1]->setChecked(true);
}

void MainWindow::rotatePieceAnticlockwise()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto& bd = getBoard();
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = bd.get_transforms().get_rotated_anticlockwise(transform);
    transform = bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    updateFlipActions();
}

void MainWindow::rotatePieceClockwise()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto& bd = getBoard();
    auto transform = m_guiBoard->getSelectedPieceTransform();
    transform = bd.get_transforms().get_rotated_clockwise(transform);
    transform = bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    updateFlipActions();
}

void MainWindow::save()
{
    if (m_file.isEmpty())
    {
        saveAs();
        return;
    }
    if (save(m_file))
    {
        m_game->clear_modified();
        updateWindow(false);
    }
}

bool MainWindow::save(const QString& file)
{
    if (! writeGame(file.toLocal8Bit().constData()))
    {
        showError(tr("The file could not be saved."),
                  /*: Error message if file cannot be saved. %1 is
                    replaced by the file name, %2 by the error message
                    of the operating system. */
                  tr("%1: %2").arg(file).arg(strerror(errno)));
        return false;
    }
    else
    {
        Util::removeThumbnail(file);
        showStatus(tr("Game saved: %1").arg(file), true);
        return true;
    }
}

void MainWindow::saveAs()
{
    QString file = m_file;
    if (file.isEmpty())
    {
        file = getLastDir();
        file.append(QDir::separator());
        file.append(tr("Untitled Game.blksgf"));
        if (QFileInfo(file).exists())
            for (unsigned i = 1; ; ++i)
            {
                file = getLastDir();
                file.append(QDir::separator());
                file.append(tr("Untitled Game %1.blksgf").arg(i));
                if (! QFileInfo(file).exists())
                    break;
            }
    }
    file = QFileDialog::getSaveFileName(this, tr("Save"), file, getFilter());
    if (! file.isEmpty())
    {
        if (save(file))
        {
            m_game->clear_modified();
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
    int seconds = static_cast<int>(ceil(remainingSeconds));
    if (seconds < 90)
    {
        if (seconds == m_lastRemainingSeconds)
            return;
        m_lastRemainingSeconds = seconds;
        text =
            tr("The computer is thinking... (max. %1 seconds remaining)")
            .arg(seconds);
    }
    else
    {
        int minutes = static_cast<int>(ceil(remainingSeconds / 60));
        if (minutes == m_lastRemainingMinutes)
            return;
        m_lastRemainingMinutes = minutes;
        text =
            tr("The computer is thinking... (max. %1 minutes remaining)")
            .arg(minutes);
    }
    QMetaObject::invokeMethod(statusBar(), "showMessage", Q_ARG(QString, text),
                              Q_ARG(int, 0));
}

void MainWindow::selectNamedPiece(initializer_list<const char*> names)
{
    auto& bd = getBoard();
    vector<Piece> pieces;
    Piece piece;
    for (auto name : names)
        if (bd.get_piece_by_name(name, piece)
            && bd.is_piece_left(m_currentColor, piece))
            pieces.push_back(piece);
    if (pieces.empty())
        return;
    piece = m_guiBoard->getSelectedPiece();
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
    selectPiece(m_currentColor, piece);
}

void MainWindow::selectNextColor()
{
    auto& bd = getBoard();
    m_currentColor = bd.get_next(m_currentColor);
    m_orientationDisplay->selectColor(m_currentColor);
    clearSelectedPiece();
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        m_pieceSelector[*i]->setEnabled(m_currentColor == *i);
    if (m_actionSetupMode->isChecked())
        setSetupPlayer();
    updateWindow(false);
}

void MainWindow::selectPiece(Color c, Piece piece)
{
    selectPiece(c, piece, getBoard().get_transforms().get_default());
}

void MainWindow::selectPiece(Color c, Piece piece, const Transform* transform)
{
    if (m_isGenMoveRunning || getBoard().is_game_over())
        return;
    m_currentColor = c;
    m_guiBoard->selectPiece(c, piece);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->selectColor(c);
    m_orientationDisplay->setSelectedPiece(piece);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    bool can_rotate = getBoard().get_piece_info(piece).can_rotate();
    m_actionRotatePieceClockwise->setEnabled(can_rotate);
    m_actionRotatePieceAnticlockwise->setEnabled(can_rotate);
    updateFlipActions();
    m_actionClearSelectedPiece->setEnabled(true);
}

void MainWindow::selectPiece1()
{
    selectNamedPiece({"1"});
}

void MainWindow::selectPiece2()
{
    selectNamedPiece({"2"});
}

void MainWindow::selectPieceA()
{
    selectNamedPiece({"A6", "A4"});
}

void MainWindow::selectPieceC()
{
    selectNamedPiece({"C5", "C4"});
}

void MainWindow::selectPieceF()
{
    selectNamedPiece({"F"});
}

void MainWindow::selectPieceG()
{
    selectNamedPiece({"G"});
}

void MainWindow::selectPieceI()
{
    selectNamedPiece({"I6", "I5", "I4", "I3"});
}

void MainWindow::selectPieceL()
{
    selectNamedPiece({"L6", "L5", "L4"});
}

void MainWindow::selectPieceN()
{
    selectNamedPiece({"N"});
}

void MainWindow::selectPieceO()
{
    selectNamedPiece({"O"});
}

void MainWindow::selectPieceP()
{
    selectNamedPiece({"P6", "P5", "P"});
}

void MainWindow::selectPieceS()
{
    selectNamedPiece({"S"});
}

void MainWindow::selectPieceT()
{
    selectNamedPiece({"T5", "T4"});
}

void MainWindow::selectPieceU()
{
    selectNamedPiece({"U"});
}

void MainWindow::selectPieceV()
{
    selectNamedPiece({"V", "V5", "V3"});
}

void MainWindow::selectPieceW()
{
    selectNamedPiece({"W"});
}

void MainWindow::selectPieceX()
{
    selectNamedPiece({"X"});
}

void MainWindow::selectPieceY()
{
    selectNamedPiece({"Y"});
}

void MainWindow::selectPieceZ()
{
    selectNamedPiece({"Z5", "Z4"});
}

void MainWindow::setCommentText(const QString& text)
{
    m_ignoreCommentTextChanged = true;
    m_comment->setPlainText(text);
    m_ignoreCommentTextChanged = false;
    if (! text.isEmpty())
        m_comment->ensureCursorVisible();
    m_comment->clearFocus();
}

void MainWindow::setDeterministic()
{
    m_player->get_search().set_deterministic();
}

void MainWindow::setNoDelay()
{
    m_noDelay = true;
}

void MainWindow::setVariant(Variant variant)
{
    if (getVariant() == variant)
        return;
    if (! checkSave())
    {
        initVariantActions();
        return;
    }
    cancelThread();
    QSettings settings;
    settings.setValue("variant", to_string_id(variant));
    clearSelectedPiece();
    m_game->init(variant);
    initPieceSelectors();
    newGame();
    loadHistory();
    restoreLevel(variant);
}

void MainWindow::setFile(const QString& file)
{
    m_file = file;
    // Don't use setWindowFilePath(), it is buggy in Qt 4.6.
    if (m_file.isEmpty())
        setWindowTitle(tr("Pentobi"));
    else
        setWindowTitle(tr("[*]%1").arg(QFileInfo(m_file).fileName()));
}

void MainWindow::setLevel(int level)
{
    if (level <= 0 || level > maxLevel)
        return;
    m_level = level;
    m_actionLevel[level - 1]->setChecked(true);
    QSettings settings;
    settings.setValue(QString("level_") + to_string_id(getVariant()), m_level);
}

void MainWindow::setLevel(bool checked)
{
    if (! checked)
        return;
    setLevel(qobject_cast<QAction*>(sender())->data().toInt());
}

void MainWindow::setMoveNumbersAll(bool checked)
{
    if (checked)
    {
        QSettings settings;
        settings.setValue("move_numbers", "all");
        updateWindow(false);
    }
}

void MainWindow::setMoveNumbersLast(bool checked)
{
    if (checked)
    {
        QSettings settings;
        settings.setValue("move_numbers", "last");
        updateWindow(false);
    }
}

void MainWindow::setMoveNumbersNone(bool checked)
{
    if (checked)
    {
        QSettings settings;
        settings.setValue("move_numbers", "none");
        updateWindow(false);
    }
}

void MainWindow::setPlayToolTip()
{
    QString s;
    auto variant = getVariant();
    Color c = m_currentColor;
    bool isComputerColor = m_computerColors[m_currentColor];
    if (variant == Variant::classic_2 || variant == Variant::trigon_2)
    {
        if (c == Color(0) || c == Color(2))
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Blue/Red");
            else
                s = tr("Make the computer play Blue/Red");
        }
        else
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Yellow/Green");
            else
                s = tr("Make the computer play Yellow/Green");
        }
    }
    else
    {
        bool isTwoColorVariant =
            (variant == Variant::duo || variant == Variant::junior);
        if (c == Color(0))
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Blue");
            else
                s = tr("Make the computer play Blue");
        }
        else if (c == Color(1) && ! isTwoColorVariant)
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Yellow");
            else
                s = tr("Make the computer play Yellow");
        }
        else if ((c == Color(1) && isTwoColorVariant)
                 || (c == Color(3) && ! isTwoColorVariant))
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Green");
            else
                s = tr("Make the computer play Green");
        }
        else
        {
            if (isComputerColor)
                s = tr("Make the computer continue to play Red");
            else
                s = tr("Make the computer play Red");
        }
    }
    m_actionPlay->setToolTip(s);
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
    if (! m_game->has_setup())
        m_game->remove_player();
    else
        m_game->set_player(m_currentColor);
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
    if (enable && m_game->get_root().has_children())
    {
        showInfo(tr("Setup mode cannot be used if moves have been played."));
        enable = false;
    }
    m_actionSetupMode->setChecked(enable);
    m_guiBoard->setFreePlacement(enable);
    if (enable)
    {
        m_setupModeLabel->show();
        for (ColorIterator i(getBoard().get_nu_colors()); i; ++i)
            m_pieceSelector[*i]->setEnabled(true);
        m_computerColors.fill(false);
    }
    else
    {
        setSetupPlayer();
        m_setupModeLabel->hide();
        m_currentColor = m_game->get_effective_to_play();
        enablePieceSelector(m_currentColor);
        updateWindow(false);
    }
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

void MainWindow::showInvalidFile(QString file, const Exception& e)
{
    showError(tr("Error in file '%1'").arg(QFileInfo(file).fileName()),
              tr("The file is not a valid Blokus SGF file."), e.what());
}

void MainWindow::showRating()
{
    if (! m_ratingDialog)
    {
        m_ratingDialog = new RatingDialog(this, *m_history);
        connect(m_ratingDialog, SIGNAL(open(const QString&)),
                SLOT(open(const QString&)));
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
    m_toolBar->setVisible(checked);
    m_menuToolBarText->setEnabled(checked);
}

QSize MainWindow::sizeHint() const
{
    return QSize(1020, 634);
}

void MainWindow::toolBarNoText(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("toolbar_text", "no_text");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void MainWindow::toolBarTextBesideIcons(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("toolbar_text", "beside_icons");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::toolBarTextBelowIcons(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("toolbar_text", "below_icons");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void MainWindow::toolBarTextOnly(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("toolbar_text", "text_only");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
}

void MainWindow::toolBarTextSystem(bool checked)
{
    if (! checked)
        return;
    QSettings settings;
    settings.setValue("toolbar_text", "system");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
}

void MainWindow::truncate()
{
    auto& current = m_game->get_current();
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
    m_game->truncate();
    m_currentColor = getCurrentColor(*m_game);
    updateWindow(true);
}

void MainWindow::truncateChildren()
{
    if (! m_game->get_current().has_children())
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
    m_game->truncate_children();
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
    auto& current = m_game->get_current();
    if (current.has_children()
        || ! m_game->get_tree().has_move_ignore_invalid(current))
        return;
    truncate();
}

void MainWindow::updateComment()
{
    string comment = m_game->get_comment();
    if (comment.empty())
    {
        setCommentText("");
        return;
    }
    string charset = m_game->get_root().get_property("CA", "");
    setCommentText(Util::convertSgfValueToQString(comment, charset));
}

void MainWindow::updateFlipActions()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    auto transform = m_guiBoard->getSelectedPieceTransform();
    bool can_flip_horizontally =
        getBoard().get_piece_info(piece).can_flip_horizontally(transform);
    m_actionFlipPieceHorizontally->setEnabled(can_flip_horizontally);
    bool can_flip_vertically =
        getBoard().get_piece_info(piece).can_flip_vertically(transform);
    m_actionFlipPieceVertically->setEnabled(can_flip_vertically);
}

void MainWindow::updateMoveAnnotationActions()
{
    if (m_game->get_move_ignore_invalid().is_null())
    {
        m_menuMoveAnnotation->setEnabled(false);
        return;
    }
    m_menuMoveAnnotation->setEnabled(true);
    double goodMove = m_game->get_good_move();
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
    double badMove = m_game->get_bad_move();
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
    if (m_game->is_interesting_move())
    {
        m_actionInterestingMove->setChecked(true);
        return;
    }
    if (m_game->is_doubtful_move())
    {
        m_actionDoubtfulMove->setChecked(true);
        return;
    }
    m_actionNoMoveAnnotation->setChecked(true);
}

void MainWindow::updateMoveNumber()
{
    auto& tree = m_game->get_tree();
    auto& current = m_game->get_current();
    unsigned move = get_move_number(tree, current);
    unsigned movesLeft = get_moves_left(tree, current);
    unsigned totalMoves = move + movesLeft;
    string variation = get_variation_string(current);
    QString text;
    QString toolTip;
    if (variation.empty())
    {
        if (movesLeft == 0)
        {
            // If last move in main variation, show the number only if it is
            // not already displayed on the board.
            if (! m_actionMoveNumbersLast->isChecked())
            {
                text = QString("%1").arg(move);
                toolTip = tr("Move %1").arg(move);
            }
        }
        else
        {
            text = QString("%1/%2").arg(move).arg(totalMoves);
            if (move == 0)
                toolTip = tr("%n move(s)", "", totalMoves);
            else
                toolTip = tr("Move %1 of %2").arg(move).arg(totalMoves);
        }
    }
    else
    {
        if (movesLeft == 0)
            text = QString("%1 (%2)").arg(move).arg(variation.c_str());
        else
            text =
                QString("%1/%2 (%3)")
                .arg(move).arg(totalMoves).arg(variation.c_str());
        toolTip =
            tr("Move %1 of %2 in variation %3")
            .arg(move).arg(totalMoves).arg(variation.c_str());
    }
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
        if (! QFileInfo(files[i]).exists())
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
    auto& bd = getBoard();
    updateWindowModified();
    m_guiBoard->copyFromBoard(bd);
    QSettings settings;
    auto markVariations = settings.value("show_variations", true).toBool();
    unsigned nuMoves = bd.get_nu_moves();
    unsigned markMovesBegin = 0;
    unsigned markMovesEnd = 0;
    if (m_actionMoveNumbersAll->isChecked())
    {
        markMovesBegin = 1;
        markMovesEnd = nuMoves;
    }
    else if (m_actionMoveNumbersLast->isChecked())
    {
        if (m_lastComputerMovesBegin != 0)
        {
            markMovesBegin = m_lastComputerMovesBegin;
            markMovesEnd = m_lastComputerMovesEnd;
        }
        else
        {
            markMovesBegin = nuMoves;
            markMovesEnd = nuMoves;
        }
    }
    gui_board_util::setMarkup(*m_guiBoard, *m_game, markMovesBegin,
                              markMovesEnd, markVariations);
    m_scoreDisplay->updateScore(bd);
    m_legalMoves->clear();
    m_legalMoveIndex = 0;
    bool isGameOver = bd.is_game_over();
    if (isGameOver && ! m_actionSetupMode->isChecked())
        m_orientationDisplay->clearSelectedColor();
    else
        m_orientationDisplay->selectColor(m_currentColor);
    if (currentNodeChanged)
    {
        clearSelectedPiece();
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
            m_pieceSelector[*i]->checkUpdate();
        if (! m_actionSetupMode->isChecked())
            enablePieceSelector(m_currentColor);
        updateComment();
        updateMoveAnnotationActions();
    }
    updateMoveNumber();
    setPlayToolTip();
    auto& tree = m_game->get_tree();
    auto& current = m_game->get_current();
    bool isMain = is_main_variation(current);
    bool hasEarlierVariation = has_earlier_variation(current);
    bool hasParent = current.has_parent();
    bool hasChildren = current.has_children();
    bool hasMove = tree.has_move_ignore_invalid(current);
    bool hasMoves = bd.has_moves(m_currentColor);
    m_actionAnalyzeGame->setEnabled(! m_isRated
                                    && tree.has_main_variation_moves());
    m_actionBackToMainVariation->setEnabled(! isMain);
    m_actionBeginning->setEnabled(! m_isRated && hasParent);
    m_actionBeginningOfBranch->setEnabled(hasEarlierVariation);
    m_actionBackward->setEnabled(! m_isRated && hasParent);
    m_actionBackward10->setEnabled(! m_isRated && hasParent);
    m_actionComputerColors->setEnabled(! m_isRated);
    m_actionDeleteAllVariations->setEnabled(tree.has_variations());
    m_actionFindNextComment->setEnabled(! m_isRated);
    m_actionForward->setEnabled(hasChildren);
    m_actionForward10->setEnabled(hasChildren);
    m_actionEnd->setEnabled(hasChildren);
    m_actionFindMove->setEnabled(! isGameOver);
    m_actionGotoMove->setEnabled(! m_isRated &&
                                 hasCurrentVariationOtherMoves(tree, current));
    m_actionKeepOnlyPosition->setEnabled(! m_isRated
                                         && (hasParent || hasChildren));
    m_actionKeepOnlySubtree->setEnabled(hasParent && hasChildren);
    for (int i = 0; i < maxLevel; ++i)
        m_actionLevel[i]->setEnabled(! m_isRated);
    m_actionMakeMainVariation->setEnabled(! isMain);
    m_actionMoveDownVariation->setEnabled(current.get_sibling());
    m_actionMoveUpVariation->setEnabled(hasParent
                       && &current.get_parent().get_first_child() != &current);
    m_actionNextVariation->setEnabled(current.get_sibling());
    if (! m_isGenMoveRunning)
    {
        m_actionNextPiece->setEnabled(! isGameOver);
        m_actionPreviousPiece->setEnabled(! isGameOver);
        m_actionPlay->setEnabled(! m_isRated && hasMoves);
        m_actionPlaySingleMove->setEnabled(! m_isRated && hasMoves);
    }
    m_actionPreviousVariation->setEnabled(current.get_previous_sibling());
    m_actionRatedGame->setEnabled(! m_isRated);
    // See also comment in setupMode()
    m_actionSetupMode->setEnabled(! m_isRated && ! hasParent && ! hasChildren);
    m_actionSelectNextColor->setEnabled(! m_isRated);
    m_actionTruncate->setEnabled(! m_isRated && hasParent);
    m_actionTruncateChildren->setEnabled(hasChildren);
    m_actionUndo->setEnabled(! m_isRated && hasParent && ! hasChildren
                             && hasMove);
    m_actionVariantClassic->setEnabled(! m_isRated);
    m_actionVariantClassic2->setEnabled(! m_isRated);
    m_actionVariantClassic3->setEnabled(! m_isRated);
    m_actionVariantDuo->setEnabled(! m_isRated);
    m_actionVariantJunior->setEnabled(! m_isRated);
    m_actionVariantTrigon->setEnabled(! m_isRated);
    m_actionVariantTrigon2->setEnabled(! m_isRated);
    m_actionVariantTrigon3->setEnabled(! m_isRated);
    // Don't disable m_menuLevel but all level items such that it is still
    // possible to see what the current level is even if it cannot be changed
    // in rated games.
    for (int i = 0; i < maxLevel; ++i)
        m_actionLevel[i]->setEnabled(! m_isRated);
    m_menuVariant->setEnabled(! m_isRated);
}

void MainWindow::updateWindowModified()
{
    if (! m_file.isEmpty())
        setWindowModified(m_game->is_modified());
}

void MainWindow::variantClassic(bool checked)
{
    if (checked)
        setVariant(Variant::classic);
}

void MainWindow::variantClassic2(bool checked)
{
    if (checked)
        setVariant(Variant::classic_2);
}

void MainWindow::variantClassic3(bool checked)
{
    if (checked)
        setVariant(Variant::classic_3);
}

void MainWindow::variantDuo(bool checked)
{
    if (checked)
        setVariant(Variant::duo);
}

void MainWindow::variantJunior(bool checked)
{
    if (checked)
        setVariant(Variant::junior);
}

void MainWindow::variantTrigon(bool checked)
{
    if (checked)
        setVariant(Variant::trigon);
}

void MainWindow::variantTrigon2(bool checked)
{
    if (checked)
        setVariant(Variant::trigon_2);
}

void MainWindow::variantTrigon3(bool checked)
{
    if (checked)
        setVariant(Variant::trigon_3);
}

void MainWindow::veryBadMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_bad_move(2);
    updateWindow(false);
}

void MainWindow::veryGoodMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_good_move(2);
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
    PentobiTreeWriter writer(out, m_game->get_tree());
    writer.set_indent(1);
    writer.write();
    return static_cast<bool>(out);
}

//-----------------------------------------------------------------------------
