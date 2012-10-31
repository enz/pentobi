//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MainWindow.h"

#include <algorithm>
#include <cstdlib>
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/fstream.hpp>
#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QtConcurrentRun>
#include "RatingHistory.h"
#include "Util.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Assert.h"
#include "libpentobi_base/TreeUtil.h"
#include "libpentobi_base/TreeWriter.h"
#include "libpentobi_gui/ComputerColorDialog.h"
#include "libpentobi_gui/GameInfoDialog.h"
#include "libpentobi_gui/GuiBoardUtil.h"
#include "libpentobi_gui/InitialRatingDialog.h"
#include "libpentobi_gui/SameHeightLayout.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using Util::getPlayerString;
using boost::format;
using boost::trim_right;
using boost::filesystem::path;
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
using libboardgame_util::ArrayList;
using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::MoveList;
using libpentobi_base::PieceInfo;
using libpentobi_base::Tree;
using libpentobi_base::TreeWriter;
using libpentobi_base::tree_util::get_move_number;
using libpentobi_base::tree_util::get_moves_left;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

namespace {

QToolButton* createOBoxToolButton(QAction* action)
{
    QToolButton* button = new QToolButton();
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
    const Tree& tree = game.get_tree();
    const Node* node = &game.get_current();
    Color c;
    while (node != 0 && ! tree.has_move(*node))
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

bool hasCurrentVariationOtherMoves(const Tree& tree, const Node& current)
{
    const Node* node = current.get_parent_or_null();
    while (node != 0)
    {
        if (! tree.get_move(*node).is_null())
            return true;
        node = node->get_parent_or_null();
    }
    node = current.get_first_child_or_null();
    while (node != 0)
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
float getMoveHeuristic(const Board& bd, Move mv)
{
    const MoveInfo& info = bd.get_move_info(mv);
    const MoveInfoExt& info_ext = bd.get_move_info_ext(mv);
    return static_cast<float>((1000 * info.points.size()
                               + 10 * info_ext.attach_points.size()
                               - info_ext.adj_points.size()));
}

/** Comparison for sorting move list in Find Move. */
bool isMoveBetter(const Board* bd, Move mv1, Move mv2)
{
    return getMoveHeuristic(*bd, mv1) > getMoveHeuristic(*bd, mv2);
}

bool isUbuntuUnity()
{
    QString session = QString::fromLocal8Bit(qgetenv("DESKTOP_SESSION"));
    return (session == "ubuntu");
}

} // namespace

//-----------------------------------------------------------------------------

MainWindow::MainWindow(const QString& initialFile, const QString& manualDir,
                       const QString& booksDir, bool noBook, size_t memory)
    : m_isGenMoveRunning(false),
      m_isAnalyzeRunning(false),
      m_ignoreCommentTextChanged(false),
      m_genMoveId(0),
      m_lastComputerMovesBegin(0),
      m_lastComputerMovesEnd(0),
      m_manualDir(manualDir),
      m_helpWindow(0),
      m_ratingDialog(0),
      m_analyzeGameWindow(0),
      m_legalMoves(new MoveList())
{
    QSettings settings;
    m_level = settings.value("level", 4).toInt();
    if (m_level < 1 || m_level > maxLevel)
        m_level = 4;
    QString variantString = settings.value("variant", "").toString();
    Variant variant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(), variant))
        variant = variant_classic;
    m_game.reset(new Game(variant));
    m_history.reset(new RatingHistory(variant));
    createActions();
    setCentralWidget(createCentralWidget());
    initPieceSelectors();
    m_moveNumber = new QLabel();
    statusBar()->addPermanentWidget(m_moveNumber);
    m_setupModeLabel = new QLabel(tr("Setup mode"));
    statusBar()->addWidget(m_setupModeLabel);
    m_setupModeLabel->hide();
    m_ratedGameLabel = new QLabel(tr("Rated game"));
    statusBar()->addWidget(m_ratedGameLabel);
    m_ratedGameLabel->hide();
    m_buttonFullscreen = new StatusBarButton(m_actionFullscreen);
    statusBar()->addPermanentWidget(m_buttonFullscreen);
    initGame();
    m_player.reset(new Player(variant, booksDir.toLocal8Bit().constData(),
                              memory));
    m_player->set_use_book(! noBook);
    createToolBar();
    connect(&m_genMoveWatcher, SIGNAL(finished()),
            this, SLOT(genMoveFinished()));
    connect(m_guiBoard, SIGNAL(play(Color, Move)),
            this, SLOT(placePiece(Color, Move)));
    connect(m_guiBoard, SIGNAL(pointClicked(Point)),
            this, SLOT(pointClicked(Point)));
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
    addAction(m_actionMoveSelectedPieceLeft);
    addAction(m_actionMoveSelectedPieceRight);
    addAction(m_actionMoveSelectedPieceUp);
    addAction(m_actionMoveSelectedPieceDown);
    addAction(m_actionNextPiece);
    addAction(m_actionNextTransform);
    addAction(m_actionNextVariation10);
    addAction(m_actionPreviousPiece);
    addAction(m_actionPreviousTransform);
    addAction(m_actionPreviousVariation10);
    addAction(m_actionPlaceSelectedPiece);
    addAction(m_actionSelectPiece1);
    addAction(m_actionSelectPiece2);
    addAction(m_actionSelectPieceA);
    addAction(m_actionSelectPieceC);
    addAction(m_actionSelectPieceF);
    addAction(m_actionSelectPieceG);
    addAction(m_actionSelectPieceI);
    addAction(m_actionSelectPieceL);
    addAction(m_actionSelectPieceN);
    addAction(m_actionSelectPieceO);
    addAction(m_actionSelectPieceP);
    addAction(m_actionSelectPieceS);
    addAction(m_actionSelectPieceT);
    addAction(m_actionSelectPieceU);
    addAction(m_actionSelectPieceV);
    addAction(m_actionSelectPieceW);
    addAction(m_actionSelectPieceX);
    addAction(m_actionSelectPieceY);
    addAction(m_actionSelectPieceZ);
    QString moveNumbers = settings.value("move_numbers", "last").toString();
    if (moveNumbers == "last")
        m_actionMoveNumbersLast->setChecked(true);
    else if (moveNumbers == "all")
        m_actionMoveNumbersAll->setChecked(true);
    else
        m_actionMoveNumbersNone->setChecked(true);
    bool coordinates = settings.value("coordinates", false).toBool();
    m_guiBoard->setCoordinates(coordinates);
    m_actionCoordinates->setChecked(coordinates);
    bool showToolbar = settings.value("toolbar", true).toBool();
    m_toolBar->setVisible(showToolbar);
    bool showVariations = settings.value("show_variations", true).toBool();
    m_actionShowVariations->setChecked(showVariations);
    initVariantActions();
    QIcon icon;
    icon.addFile(":/pentobi/icons/pentobi.png");
    icon.addFile(":/pentobi/icons/pentobi-16.png");
    icon.addFile(":/pentobi/icons/pentobi-32.png");
    setWindowIcon(icon);

    if (! restoreGeometry(settings.value("geometry").toByteArray()))
        adjustSize();
    if (isFullScreen())
        m_actionFullscreen->setChecked(true);
    if (! isUbuntuUnity() || ! isFullScreen())
        // See comment at m_buttonFullscreen
        statusBar()->removeWidget(m_buttonFullscreen);

    bool showComment = settings.value("show_comment", false).toBool();
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
            m_gameFinished = getBoard().is_game_over();
            updateWindow(true);
            deleteAutoSaveFile();
            if (settings.value("autosave_rated", false).toBool())
            {
                Variant variant = getVariant();
                unsigned ratedGameColor =
                    settings.value("autosave_rated_color", 0).toUInt();
                if (ratedGameColor < get_nu_colors(variant))
                {
                    m_ratedGameColor = Color(ratedGameColor);
                    m_computerColors.fill(true);
                    const Board& bd = getBoard();
                    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
                        if (bd.is_same_player(*i, m_ratedGameColor))
                            m_computerColors[*i] = false;
                    setRated(true);
                    show();
                    showInfo(tr("Continuing unfinished rated game."),
                             tr("You play %1 in this game.")
                             .arg(getPlayerString(variant, m_ratedGameColor)));
                    checkComputerMove();
                }
            }
        }
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Pentobi"),
                       "<style type=\"text/css\">"
                       ":link { text-decoration: none; }"
                       "</style>"
                       "<h3>" + tr("Pentobi") + "</h3>"
                       "<p>" + tr("Version %1").arg(getVersion()) + "</p>"
                       "<p>" +
                       tr("Computer program that plays the board game Blokus.")
                       + "<br>"
                       "<a href=\"http://pentobi.sourceforge.net\">http://pentobi.sourceforge.net</a>"
                       + "</p>"
                       "<p>" +
                       tr("Copyright &copy; 2011&ndash;2012 Markus Enzenberger") +
                       "</p>");
}

void MainWindow::analyzeGame()
{
    QStringList items;
    items << tr("Fast") << tr("Normal") << tr("Slow");
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Analyze Game"),
                                         tr("Analysis speed:"), items, 0,
                                         false, &ok);
    if (! ok || item.isEmpty())
        return;
    int speed = items.indexOf(item);
    cancelThread();
    if (m_analyzeGameWindow != 0)
        delete m_analyzeGameWindow;
    m_analyzeGameWindow = new AnalyzeGameWindow(this);
    m_analyzeGameWindow->show();
    m_isAnalyzeRunning = true;
    connect(m_analyzeGameWindow->analyzeGameWidget, SIGNAL(finished()),
            this, SLOT(analyzeGameFinished()));
    connect(m_analyzeGameWindow->analyzeGameWidget,
            SIGNAL(gotoPosition(Variant,const vector<ColorMove>&)),
            this, SLOT(gotoPosition(Variant,const vector<ColorMove>&)));
    size_t nuSimulations;
    switch (speed)
    {
    case 2:
        nuSimulations = 56000;
        break;
    case 1:
        nuSimulations = 14000;
        break;
    default:
        nuSimulations = 3500;
        break;
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
    GenMoveResult result;
    result.playSingleMove = playSingleMove;
    result.color = c;
    result.genMoveId = genMoveId;
    result.move = m_player->genmove(getBoard(), c);
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
    const Node& node = m_game->get_current();
    if (! node.has_parent())
        return;
    gotoNode(node.get_parent());
}

void MainWindow::backward10()
{
    const Tree& tree = m_game->get_tree();
    const Node* node = &m_game->get_current();
    unsigned n = 0;
    while (n < 10)
    {
        if (tree.has_move(*node))
            ++n;
        const Node* parent = node->get_parent_or_null();
        if (parent == 0)
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
    if (m_isGenMoveRunning)
    {
        // After waitForFinished() returns, we can be sure that the move
        // generation is no longer running, but we will still receive the
        // finished event. Increasing m_genMoveId will make genMoveFinished()
        // ignore the event.
        ++m_genMoveId;
        set_abort();
        m_genMoveWatcher.waitForFinished();
        m_isGenMoveRunning = false;
        clearStatus();
        setCursor(QCursor(Qt::ArrowCursor));
        m_actionInterrupt->setEnabled(false);
    }
}

void MainWindow::checkComputerMove()
{
    bool isGameOver = getBoard().is_game_over();
    if (! isGameOver && m_computerColors[m_currentColor])
        genMove();
    else
        m_lastComputerMovesBegin = 0;
}

bool MainWindow::checkSave()
{
    if (! m_file.isEmpty())
    {
        if (! m_game->get_modified())
            return true;
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The file has been modified."),
                     tr("Do you want to save your changes?"));
        // Don't use QMessageBox::Discard because on some platforms it uses the
        // text "Close without saving" which implies that the window would be
        // closed
        QPushButton* discardButton =
            msgBox.addButton(tr("&Don't Save"), QMessageBox::DestructiveRole);
        QPushButton* saveButton = msgBox.addButton(QMessageBox::Save);
        QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        QAbstractButton* result = msgBox.clickedButton();
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
        QPushButton* abortGameButton =
            msgBox.addButton(tr("&Abort Game"), QMessageBox::DestructiveRole);
        QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
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
    if (! m_file.isEmpty() && m_game->get_modified())
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The file has been modified."),
                     tr("Do you want to save your changes?"));
        QPushButton* discardButton = msgBox.addButton(QMessageBox::Discard);
        QPushButton* saveButton = msgBox.addButton(QMessageBox::Save);
        QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancelButton);
        msgBox.exec();
        QAbstractButton* result = msgBox.clickedButton();
        if (result == saveButton)
        {
            save();
            return true;
        }
        return (result == discardButton);
    }
    cancelThread();
    QSettings settings;
    if (m_file.isEmpty() && ! m_gameFinished && m_game->get_modified())
    {
        writeGame(getAutoSaveFile().toLocal8Bit().constData());
        settings.setValue("autosave_rated", m_isRated);
        if (m_isRated)
            settings.setValue("autosave_rated_color",
                              m_ratedGameColor.to_int());
    }
    settings.setValue("geometry", saveGeometry());
    if (m_comment->isVisible())
        settings.setValue("splitter_state", m_splitter->saveState());
    settings.setValue("toolbar", m_toolBar->isVisible());
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

void MainWindow::coordinates(bool checked)
{
    m_guiBoard->setCoordinates(checked);
    QSettings settings;
    settings.setValue("coordinates", checked);
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
    Color oldCurrentColor = m_currentColor;
    Variant variant = getVariant();
    ComputerColorDialog dialog(this, variant, m_computerColors);
    dialog.exec();
    if (variant != variant_classic && variant != variant_trigon
        && variant != variant_trigon_3)
    {
        bool computerNone = true;
        for (ColorIterator i(getBoard().get_nu_colors()); i; ++i)
            if (m_computerColors[*i])
            {
                computerNone = false;
                break;
            }
        if (computerNone)
        {
            QSettings settings;
            settings.setValue("computer_color_none", true);
        }
    }
    // Don't automatically start playing if color was already played by the
    // computer, this could be irritating if the computer currently does not
    // think (even if it is set to play the current color) and the user only
    // opens the dialog to check the computer color settings.
    bool isCurrentPlayedByComputer = m_computerColors[m_currentColor];
    bool wasCurrentPlayedByComputer = oldComputerColors[m_currentColor];
    // Current color could have changed if a move generation finished while
    // the dialog was running.
    bool hasCurrentColorChanged = (oldCurrentColor != m_currentColor);
    if (! m_isGenMoveRunning && isCurrentPlayedByComputer
        && (hasCurrentColorChanged || ! wasCurrentPlayedByComputer))
        checkComputerMove();
}

bool MainWindow::computerPlaysAll() const
{
    for (ColorIterator i(getBoard().get_nu_colors()); i; ++i)
        if (! m_computerColors[*i])
            return false;
    return true;
}

void MainWindow::createActions()
{
    QActionGroup* groupVariant = new QActionGroup(this);
    QActionGroup* groupLevel = new QActionGroup(this);
    QActionGroup* groupMoveNumbers = new QActionGroup(this);
    QActionGroup* groupMoveAnnotation = new QActionGroup(this);

    m_actionAbout = new QAction(tr("&About"), this);
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actionAnalyzeGame = new QAction(tr("&Analyze Game..."), this);
    connect(m_actionAnalyzeGame, SIGNAL(triggered()),
            this, SLOT(analyzeGame()));

    m_actionBackward = new QAction(tr("B&ackward"), this);
    m_actionBackward->setToolTip(tr("Go one move backward"));
    setIcon(m_actionBackward, "pentobi-backward");
    m_actionBackward->setPriority(QAction::LowPriority);
    m_actionBackward->setShortcut(QString("Ctrl+Left"));
    connect(m_actionBackward, SIGNAL(triggered()), this, SLOT(backward()));

    m_actionBackward10 = new QAction(tr("10 Moves Back&ward"), this);
    m_actionBackward10->setToolTip(tr("Go ten moves backward"));
    setIcon(m_actionBackward10, "pentobi-backward10");
    m_actionBackward10->setPriority(QAction::LowPriority);
    m_actionBackward10->setShortcut(QString("Ctrl+Shift+Left"));
    connect(m_actionBackward10, SIGNAL(triggered()), this, SLOT(backward10()));

    m_actionBackToMainVariation = new QAction(tr("Back to &Main Variation"),
                                              this);
    m_actionBackToMainVariation->setShortcut(QString("Ctrl+M"));
    connect(m_actionBackToMainVariation, SIGNAL(triggered()),
            this, SLOT(backToMainVariation()));

    m_actionBadMove = new QAction(tr("&Bad"), this);
    m_actionBadMove->setActionGroup(groupMoveAnnotation);
    m_actionBadMove->setCheckable(true);
    connect(m_actionBadMove, SIGNAL(triggered(bool)),
            this, SLOT(badMove(bool)));

    m_actionBeginning = new QAction(tr("&Beginning"), this);
    m_actionBeginning->setToolTip(tr("Go to beginning of game"));
    setIcon(m_actionBeginning, "pentobi-beginning");
    m_actionBeginning->setPriority(QAction::LowPriority);
    m_actionBeginning->setShortcut(QString("Ctrl+Home"));
    connect(m_actionBeginning, SIGNAL(triggered()), this, SLOT(beginning()));

    m_actionBeginningOfBranch = new QAction(tr("Beginning of Bran&ch"), this);
    m_actionBeginningOfBranch->setShortcut(QString("Ctrl+Shift+Home"));
    connect(m_actionBeginningOfBranch, SIGNAL(triggered()),
            this, SLOT(beginningOfBranch()));

    m_actionClearSelectedPiece = new QAction(tr("Clear Piece"), this);
    setIcon(m_actionClearSelectedPiece, "pentobi-piece-clear");
    m_actionClearSelectedPiece->setShortcut(QString("0"));
    connect(m_actionClearSelectedPiece, SIGNAL(triggered()),
            this, SLOT(clearSelectedPiece()));

    m_actionComputerColors = new QAction(tr("&Computer Colors"), this);
    m_actionComputerColors->setToolTip(
                                  tr("Set the colors played by the computer"));
    setIcon(m_actionComputerColors, "pentobi-computer-color");
    connect(m_actionComputerColors, SIGNAL(triggered()),
            this, SLOT(computerColors()));

    m_actionCoordinates = new QAction(tr("C&oordinates"), this);
    m_actionCoordinates->setCheckable(true);
    connect(m_actionCoordinates, SIGNAL(triggered(bool)),
            this, SLOT(coordinates(bool)));

    m_actionDeleteAllVariations =
        new QAction(tr("&Delete All Variations"), this);
    connect(m_actionDeleteAllVariations, SIGNAL(triggered()),
            this, SLOT(deleteAllVariations()));

    m_actionDoubtfulMove = new QAction(tr("&Doubtful"), this);
    m_actionDoubtfulMove->setActionGroup(groupMoveAnnotation);
    m_actionDoubtfulMove->setCheckable(true);
    connect(m_actionDoubtfulMove, SIGNAL(triggered(bool)),
            this, SLOT(doubtfulMove(bool)));

    m_actionEnd = new QAction(tr("&End"), this);
    m_actionEnd->setToolTip(tr("Go to end of moves"));
    m_actionEnd->setPriority(QAction::LowPriority);
    m_actionEnd->setShortcut(QString("Ctrl+End"));
    setIcon(m_actionEnd, "pentobi-end");
    connect(m_actionEnd, SIGNAL(triggered()), this, SLOT(end()));

    m_actionExportAsciiArt = new QAction(tr("&ASCII Art"), this);
    connect(m_actionExportAsciiArt, SIGNAL(triggered()),
            this, SLOT(exportAsciiArt()));

    m_actionExportImage = new QAction(tr("I&mage"), this);
    connect(m_actionExportImage, SIGNAL(triggered()),
            this, SLOT(exportImage()));

    m_actionFindMove = new QAction(tr("&Find Move"), this);
    m_actionFindMove->setShortcut(QString("F6"));
    connect(m_actionFindMove, SIGNAL(triggered()), this, SLOT(findMove()));

    m_actionFindNextComment = new QAction(tr("&Find Next Comment"), this);
    m_actionFindNextComment->setShortcut(QString("F3"));
    connect(m_actionFindNextComment, SIGNAL(triggered()),
            this, SLOT(findNextComment()));

    m_actionFlipPieceHorizontally = new QAction(tr("Flip Horizontally"), this);
    setIcon(m_actionFlipPieceHorizontally, "pentobi-flip-horizontal");
    connect(m_actionFlipPieceHorizontally, SIGNAL(triggered()),
            this, SLOT(flipPieceHorizontally()));

    m_actionFlipPieceVertically = new QAction(tr("Flip Vertically"), this);
    setIcon(m_actionFlipPieceVertically, "pentobi-flip-vertical");

    m_actionForward = new QAction(tr("&Forward"), this);
    m_actionForward->setToolTip(tr("Go one move forward"));
    m_actionForward->setPriority(QAction::LowPriority);
    m_actionForward->setShortcut(QString("Ctrl+Right"));
    setIcon(m_actionForward, "pentobi-forward");
    connect(m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));

    m_actionForward10 = new QAction(tr("10 Moves F&orward"), this);
    m_actionForward10->setToolTip(tr("Go ten moves forward"));
    m_actionForward10->setPriority(QAction::LowPriority);
    m_actionForward10->setShortcut(QString("Ctrl+Shift+Right"));
    setIcon(m_actionForward10, "pentobi-forward10");
    connect(m_actionForward10, SIGNAL(triggered()), this, SLOT(forward10()));

    m_actionFullscreen = new QAction(tr("&Fullscreen"), this);
    m_actionFullscreen->setShortcut(QString("F11"));
    setIcon(m_actionFullscreen, "pentobi-fullscreen");
    m_actionFullscreen->setCheckable(true);
    m_actionFullscreen->setIconVisibleInMenu(false);
    connect(m_actionFullscreen, SIGNAL(triggered(bool)),
            this, SLOT(fullscreen(bool)));

    m_actionGameInfo = new QAction(tr("G&ame Info"), this);
    m_actionGameInfo->setShortcut(QString("Ctrl+I"));
    connect(m_actionGameInfo, SIGNAL(triggered()), this, SLOT(gameInfo()));

    m_actionVariantClassic = new QAction(tr("&Classic (4 Players)"), this);
    m_actionVariantClassic->setActionGroup(groupVariant);
    m_actionVariantClassic->setCheckable(true);
    connect(m_actionVariantClassic, SIGNAL(triggered(bool)),
            this, SLOT(variantClassic(bool)));

    m_actionVariantClassic2 = new QAction(tr("Classic (&2 Players)"), this);
    m_actionVariantClassic2->setActionGroup(groupVariant);
    m_actionVariantClassic2->setCheckable(true);
    connect(m_actionVariantClassic2, SIGNAL(triggered(bool)),
            this, SLOT(variantClassic2(bool)));

    m_actionVariantDuo = new QAction(tr("&Duo"), this);
    m_actionVariantDuo->setActionGroup(groupVariant);
    m_actionVariantDuo->setCheckable(true);
    connect(m_actionVariantDuo, SIGNAL(triggered(bool)),
            this, SLOT(variantDuo(bool)));

    m_actionVariantJunior = new QAction(tr("J&unior"), this);
    m_actionVariantJunior->setActionGroup(groupVariant);
    m_actionVariantJunior->setCheckable(true);
    connect(m_actionVariantJunior, SIGNAL(triggered(bool)),
            this, SLOT(variantJunior(bool)));

    m_actionVariantTrigon = new QAction(tr("&Trigon (4 Players)"), this);
    m_actionVariantTrigon->setActionGroup(groupVariant);
    m_actionVariantTrigon->setCheckable(true);
    connect(m_actionVariantTrigon, SIGNAL(triggered(bool)),
            this, SLOT(variantTrigon(bool)));

    m_actionVariantTrigon2 = new QAction(tr("Tri&gon (2 Players)"), this);
    m_actionVariantTrigon2->setActionGroup(groupVariant);
    m_actionVariantTrigon2->setCheckable(true);
    connect(m_actionVariantTrigon2, SIGNAL(triggered(bool)),
            this, SLOT(variantTrigon2(bool)));

    m_actionVariantTrigon3 = new QAction(tr("Trigon (&3 Players)"), this);
    m_actionVariantTrigon3->setActionGroup(groupVariant);
    m_actionVariantTrigon3->setCheckable(true);
    connect(m_actionVariantTrigon3, SIGNAL(triggered(bool)),
            this, SLOT(variantTrigon3(bool)));

    m_actionGoodMove = new QAction(tr("&Good"), this);
    m_actionGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionGoodMove->setCheckable(true);
    connect(m_actionGoodMove, SIGNAL(triggered(bool)),
            this, SLOT(goodMove(bool)));

    m_actionGotoMove = new QAction(tr("&Go to Move..."), this);
    m_actionGotoMove->setShortcut(QString("Ctrl+G"));
    connect(m_actionGotoMove, SIGNAL(triggered()), this, SLOT(gotoMove()));

    m_actionHelp = new QAction(tr("&Contents"), this);
    m_actionHelp->setShortcut(QKeySequence::HelpContents);
    connect(m_actionHelp, SIGNAL(triggered()), this, SLOT(help()));

    m_actionInterestingMove = new QAction(tr("I&nteresting"), this);
    m_actionInterestingMove->setActionGroup(groupMoveAnnotation);
    m_actionInterestingMove->setCheckable(true);
    connect(m_actionInterestingMove, SIGNAL(triggered(bool)),
            this, SLOT(interestingMove(bool)));

    m_actionInterrupt = new QAction(tr("I&nterrupt"), this);
    m_actionInterrupt->setShortcut(QString("Escape"));
    m_actionInterrupt->setEnabled(false);
    connect(m_actionInterrupt, SIGNAL(triggered()), this, SLOT(interrupt()));

    m_actionKeepOnlyPosition = new QAction(tr("&Keep Only Position"), this);
    connect(m_actionKeepOnlyPosition, SIGNAL(triggered()),
            this, SLOT(keepOnlyPosition()));

    m_actionKeepOnlySubtree = new QAction(tr("Keep Only &Subtree"), this);
    connect(m_actionKeepOnlySubtree, SIGNAL(triggered()),
            this, SLOT(keepOnlySubtree()));

    m_actionMakeMainVariation = new QAction(tr("M&ake Main Variation"), this);
    connect(m_actionMakeMainVariation, SIGNAL(triggered()),
            this, SLOT(makeMainVariation()));

    m_actionMoveDownVariation = new QAction(tr("Move Variation D&own"), this);
    connect(m_actionMoveDownVariation, SIGNAL(triggered()),
            this, SLOT(moveDownVariation()));

    m_actionMoveUpVariation = new QAction(tr("Move Variation &Up"), this);
    connect(m_actionMoveUpVariation, SIGNAL(triggered()),
            this, SLOT(moveUpVariation()));

    QString levelText[maxLevel] =
        {
            tr("&1"),
            tr("&2"),
            tr("&3"),
            tr("&4"),
            tr("&5"),
            tr("&6"),
            tr("&7")
        };
    for (int i = 0; i < maxLevel; ++i)
        m_actionLevel[i] = createLevelAction(groupLevel, i + 1, levelText[i]);
    connect(m_actionFlipPieceVertically, SIGNAL(triggered()),
            this, SLOT(flipPieceVertically()));

    m_actionMoveNumbersAll = new QAction(tr("&All"), this);
    m_actionMoveNumbersAll->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersAll->setCheckable(true);
    connect(m_actionMoveNumbersAll, SIGNAL(triggered(bool)),
            this, SLOT(setMoveNumbersAll(bool)));

    m_actionMoveNumbersLast = new QAction(tr("&Last"), this);
    m_actionMoveNumbersLast->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersLast->setCheckable(true);
    m_actionMoveNumbersLast->setChecked(true);
    connect(m_actionMoveNumbersLast, SIGNAL(triggered(bool)),
            this, SLOT(setMoveNumbersLast(bool)));

    m_actionMoveNumbersNone = new QAction(tr("&None", "move numbers"), this);
    m_actionMoveNumbersNone->setActionGroup(groupMoveNumbers);
    m_actionMoveNumbersNone->setCheckable(true);
    connect(m_actionMoveNumbersNone, SIGNAL(triggered(bool)),
            this, SLOT(setMoveNumbersNone(bool)));

    m_actionMoveSelectedPieceLeft = new QAction("", this);
    m_actionMoveSelectedPieceLeft->setShortcut(QString("Left"));

    m_actionMoveSelectedPieceRight = new QAction("", this);
    m_actionMoveSelectedPieceRight->setShortcut(QString("Right"));

    m_actionMoveSelectedPieceUp = new QAction("", this);
    m_actionMoveSelectedPieceUp->setShortcut(QString("Up"));

    m_actionMoveSelectedPieceDown = new QAction("", this);
    m_actionMoveSelectedPieceDown->setShortcut(QString("Down"));

    m_actionNextPiece = new QAction(tr("Next Piece"), this);
    setIcon(m_actionNextPiece, "pentobi-next-piece");
    m_actionNextPiece->setShortcut(QString("+"));
    connect(m_actionNextPiece, SIGNAL(triggered()), this, SLOT(nextPiece()));

    m_actionNextTransform = new QAction("", this);
    m_actionNextTransform->setShortcut(QString("Space"));
    connect(m_actionNextTransform, SIGNAL(triggered()),
            this, SLOT(nextTransform()));

    m_actionNextVariation = new QAction(tr("&Next Variation"), this);
    m_actionNextVariation->setToolTip(tr("Go to next variation"));
    m_actionNextVariation->setPriority(QAction::LowPriority);
    m_actionNextVariation->setShortcut(QString("Ctrl+Down"));
    setIcon(m_actionNextVariation, "pentobi-next-variation");
    connect(m_actionNextVariation, SIGNAL(triggered()),
            this, SLOT(nextVariation()));

    m_actionNextVariation10 = new QAction("", this);
    m_actionNextVariation10->setShortcut(QString("Ctrl+Shift+Down"));
    connect(m_actionNextVariation10, SIGNAL(triggered()),
            this, SLOT(nextVariation10()));

    m_actionNewRatedGame = new QAction(tr("New &Rated Game"), this);
    m_actionNewRatedGame->setShortcut(QString("Ctrl+Shift+N"));
    connect(m_actionNewRatedGame, SIGNAL(triggered()),
            this, SLOT(newRatedGame()));

    m_actionNewGame = new QAction(tr("&New"), this);
    m_actionNewGame->setShortcut(QKeySequence::New);
    m_actionNewGame->setToolTip(tr("Start a new game"));
    setIcon(m_actionNewGame, "pentobi-newgame");
    connect(m_actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));

    m_actionNoMoveAnnotation =
        new QAction(tr("N&one", "move annotation"), this);
    m_actionNoMoveAnnotation->setActionGroup(groupMoveAnnotation);
    m_actionNoMoveAnnotation->setCheckable(true);
    connect(m_actionNoMoveAnnotation, SIGNAL(triggered(bool)),
            this, SLOT(noMoveAnnotation(bool)));

    m_actionOpen = new QAction(tr("&Open..."), this);
    m_actionOpen->setPriority(QAction::LowPriority);
    m_actionOpen->setShortcut(QKeySequence::Open);
    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    m_actionPlaceSelectedPiece = new QAction("", this);
    m_actionPlaceSelectedPiece->setShortcut(QString("Return"));

    m_actionPlay = new QAction(tr("&Play"), this);
    m_actionPlay->setShortcut(QString("Ctrl+L"));
    setIcon(m_actionPlay, "pentobi-play");
    connect(m_actionPlay, SIGNAL(triggered()), this, SLOT(play()));

    m_actionPlaySingleMove = new QAction(tr("Play &Single Move"), this);
    m_actionPlaySingleMove->setShortcut(QString("Ctrl+Shift+L"));
    connect(m_actionPlaySingleMove, SIGNAL(triggered()),
            this, SLOT(playSingleMove()));

    m_actionPreviousPiece = new QAction(tr("Previous Piece"), this);
    setIcon(m_actionPreviousPiece, "pentobi-previous-piece");
    m_actionPreviousPiece->setShortcut(QString("-"));
    connect(m_actionPreviousPiece, SIGNAL(triggered()),
            this, SLOT(previousPiece()));

    m_actionPreviousTransform = new QAction("", this);
    m_actionPreviousTransform->setShortcut(QString("Shift+Space"));
    connect(m_actionPreviousTransform, SIGNAL(triggered()),
            this, SLOT(previousTransform()));

    m_actionPreviousVariation = new QAction(tr("&Previous Variation"), this);
    m_actionPreviousVariation->setToolTip(tr("Go to previous variation"));
    m_actionPreviousVariation->setShortcut(QString("Ctrl+Up"));
    m_actionPreviousVariation->setPriority(QAction::LowPriority);
    setIcon(m_actionPreviousVariation, "pentobi-previous-variation");
    connect(m_actionPreviousVariation, SIGNAL(triggered()),
            this, SLOT(previousVariation()));

    m_actionPreviousVariation10 = new QAction("", this);
    m_actionPreviousVariation10->setShortcut(QString("Ctrl+Shift+Up"));
    connect(m_actionPreviousVariation10, SIGNAL(triggered()),
            this, SLOT(previousVariation10()));

    for (int i = 0; i < maxRecentFiles; ++i)
    {
         m_actionRecentFile[i] = new QAction(this);
         m_actionRecentFile[i]->setVisible(false);
         connect(m_actionRecentFile[i], SIGNAL(triggered()),
                 this, SLOT(openRecentFile()));
     }

    m_actionRotatePieceAnticlockwise = new QAction(tr("Rotate Anticlockwise"),
                                                   this);
    setIcon(m_actionRotatePieceAnticlockwise, "pentobi-rotate-left");
    connect(m_actionRotatePieceAnticlockwise, SIGNAL(triggered()),
            this, SLOT(rotatePieceAnticlockwise()));

    m_actionRotatePieceClockwise = new QAction(tr("Rotate Clockwise"), this);
    setIcon(m_actionRotatePieceClockwise, "pentobi-rotate-right");
    connect(m_actionRotatePieceClockwise, SIGNAL(triggered()),
            this, SLOT(rotatePieceClockwise()));

    m_actionQuit = new QAction(tr("&Quit"), this);
    m_actionQuit->setShortcut(QKeySequence::Quit);
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(quit()));

    m_actionSave = new QAction(tr("&Save"), this);
    m_actionSave->setPriority(QAction::LowPriority);
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(save()));

    m_actionSaveAs = new QAction(tr("Save &As..."), this);
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_actionSelectNextColor = new QAction(tr("Select Next &Color"), this);
    m_actionSelectNextColor->setShortcut(QString("Ctrl+C"));
    connect(m_actionSelectNextColor, SIGNAL(triggered()),
            this, SLOT(selectNextColor()));

    m_actionSelectPiece1 = new QAction(this);
    m_actionSelectPiece1->setShortcut(QString("1"));
    connect(m_actionSelectPiece1, SIGNAL(triggered()),
            this, SLOT(selectPiece1()));

    m_actionSelectPiece2 = new QAction(this);
    m_actionSelectPiece2->setShortcut(QString("2"));
    connect(m_actionSelectPiece2, SIGNAL(triggered()),
            this, SLOT(selectPiece2()));

    m_actionSelectPieceA = new QAction(this);
    m_actionSelectPieceA->setShortcut(QString("A"));
    connect(m_actionSelectPieceA, SIGNAL(triggered()),
            this, SLOT(selectPieceA()));

    m_actionSelectPieceC = new QAction(this);
    m_actionSelectPieceC->setShortcut(QString("C"));
    connect(m_actionSelectPieceC, SIGNAL(triggered()),
            this, SLOT(selectPieceC()));

    m_actionSelectPieceF = new QAction(this);
    m_actionSelectPieceF->setShortcut(QString("F"));
    connect(m_actionSelectPieceF, SIGNAL(triggered()),
            this, SLOT(selectPieceF()));

    m_actionSelectPieceG = new QAction(this);
    m_actionSelectPieceG->setShortcut(QString("G"));
    connect(m_actionSelectPieceG, SIGNAL(triggered()),
            this, SLOT(selectPieceG()));

    m_actionSelectPieceI = new QAction(this);
    m_actionSelectPieceI->setShortcut(QString("I"));
    connect(m_actionSelectPieceI, SIGNAL(triggered()),
            this, SLOT(selectPieceI()));

    m_actionSelectPieceL = new QAction(this);
    m_actionSelectPieceL->setShortcut(QString("L"));
    connect(m_actionSelectPieceL, SIGNAL(triggered()),
            this, SLOT(selectPieceL()));

    m_actionSelectPieceN = new QAction(this);
    m_actionSelectPieceN->setShortcut(QString("N"));
    connect(m_actionSelectPieceN, SIGNAL(triggered()),
            this, SLOT(selectPieceN()));

    m_actionSelectPieceO = new QAction(this);
    m_actionSelectPieceO->setShortcut(QString("O"));
    connect(m_actionSelectPieceO, SIGNAL(triggered()),
            this, SLOT(selectPieceO()));

    m_actionSelectPieceP = new QAction(this);
    m_actionSelectPieceP->setShortcut(QString("P"));
    connect(m_actionSelectPieceP, SIGNAL(triggered()),
            this, SLOT(selectPieceP()));

    m_actionSelectPieceS = new QAction(this);
    m_actionSelectPieceS->setShortcut(QString("S"));
    connect(m_actionSelectPieceS, SIGNAL(triggered()),
            this, SLOT(selectPieceS()));

    m_actionSelectPieceT = new QAction(this);
    m_actionSelectPieceT->setShortcut(QString("T"));
    connect(m_actionSelectPieceT, SIGNAL(triggered()),
            this, SLOT(selectPieceT()));

    m_actionSelectPieceU = new QAction(this);
    m_actionSelectPieceU->setShortcut(QString("U"));
    connect(m_actionSelectPieceU, SIGNAL(triggered()),
            this, SLOT(selectPieceU()));

    m_actionSelectPieceV = new QAction(this);
    m_actionSelectPieceV->setShortcut(QString("V"));
    connect(m_actionSelectPieceV, SIGNAL(triggered()),
            this, SLOT(selectPieceV()));

    m_actionSelectPieceW = new QAction(this);
    m_actionSelectPieceW->setShortcut(QString("W"));
    connect(m_actionSelectPieceW, SIGNAL(triggered()),
            this, SLOT(selectPieceW()));

    m_actionSelectPieceX = new QAction(this);
    m_actionSelectPieceX->setShortcut(QString("X"));
    connect(m_actionSelectPieceX, SIGNAL(triggered()),
            this, SLOT(selectPieceX()));

    m_actionSelectPieceY = new QAction(this);
    m_actionSelectPieceY->setShortcut(QString("Y"));
    connect(m_actionSelectPieceY, SIGNAL(triggered()),
            this, SLOT(selectPieceY()));

    m_actionSelectPieceZ = new QAction(this);
    m_actionSelectPieceZ->setShortcut(QString("Z"));
    connect(m_actionSelectPieceZ, SIGNAL(triggered()),
            this, SLOT(selectPieceZ()));

    m_actionSetupMode = new QAction(tr("S&etup Mode"), this);
    m_actionSetupMode->setCheckable(true);
    connect(m_actionSetupMode, SIGNAL(triggered(bool)),
            this, SLOT(setupMode(bool)));

    m_actionShowComment = new QAction(tr("&Comment"), this);
    m_actionShowComment->setCheckable(true);
    m_actionShowComment->setShortcut(QString("Ctrl+T"));
    connect(m_actionShowComment, SIGNAL(triggered(bool)),
            this, SLOT(showComment(bool)));

    m_actionShowRating = new QAction(tr("Your &Rating"), this);
    m_actionShowRating->setShortcut(QString("F7"));
    connect(m_actionShowRating, SIGNAL(triggered()), this, SLOT(showRating()));

    m_actionTruncate = new QAction(tr("&Truncate"), this);
    connect(m_actionTruncate, SIGNAL(triggered()), this, SLOT(truncate()));

    m_actionTruncateChildren = new QAction(tr("Truncate C&hildren"), this);
    connect(m_actionTruncateChildren, SIGNAL(triggered()),
            this, SLOT(truncateChildren()));

    m_actionShowVariations = new QAction(tr("Show &Variations"), this);
    m_actionShowVariations->setCheckable(true);
    connect(m_actionShowVariations, SIGNAL(triggered(bool)),
            this, SLOT(showVariations(bool)));

    m_actionUndo = new QAction(tr("&Undo Move"), this);
    connect(m_actionUndo, SIGNAL(triggered()), this, SLOT(undo()));

    m_actionVeryBadMove = new QAction(tr("V&ery Bad"), this);
    m_actionVeryBadMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryBadMove->setCheckable(true);
    connect(m_actionVeryBadMove, SIGNAL(triggered(bool)),
            this, SLOT(veryBadMove(bool)));

    m_actionVeryGoodMove = new QAction(tr("&Very Good"), this);
    m_actionVeryGoodMove->setActionGroup(groupMoveAnnotation);
    m_actionVeryGoodMove->setCheckable(true);
    connect(m_actionVeryGoodMove, SIGNAL(triggered(bool)),
            this, SLOT(veryGoodMove(bool)));
}

QWidget* MainWindow::createCentralWidget()
{
    QWidget* widget = new QWidget();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    widget->setLayout(layout);
    layout->addWidget(createLeftPanel(), 55);
    layout->addLayout(createRightPanel(), 45);
    // The central widget doesn't do anything with the focus right now, but we
    // allow it to receive the focus such that the user can switch away the
    // focus from the comment field and its blinking cursor.
    widget->setFocusPolicy(Qt::StrongFocus);
    return widget;
}

QWidget* MainWindow::createLeftPanel()
{
    m_splitter = new QSplitter(Qt::Vertical);
    m_guiBoard = new GuiBoard(0, getBoard());
    m_splitter->addWidget(m_guiBoard);
    m_comment = new QPlainTextEdit();
    m_comment->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_comment->setTabChangesFocus(true);
    connect(m_comment, SIGNAL(textChanged()), this, SLOT(commentChanged()));
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
    QAction* action = new QAction(text, this);
    action->setCheckable(true);
    if (level == m_level)
        action->setChecked(true);
    action->setActionGroup(group);
    action->setData(level);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setLevel(bool)));
    return action;
}

void MainWindow::createMenu()
{

    QMenu* menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(m_actionOpen);
    m_menuOpenRecent = menuFile->addMenu(tr("Open &Recent"));
    for (int i = 0; i < maxRecentFiles; ++i)
        m_menuOpenRecent->addAction(m_actionRecentFile[i]);
    menuFile->addSeparator();
    menuFile->addAction(m_actionSave);
    menuFile->addAction(m_actionSaveAs);
    QMenu* menuExport = menuFile->addMenu(tr("&Export"));
    menuExport->addAction(m_actionExportImage);
    menuExport->addAction(m_actionExportAsciiArt);
    menuFile->addSeparator();
    menuFile->addAction(m_actionQuit);

    QMenu* menuGame = menuBar()->addMenu(tr("G&ame"));
    menuGame->addAction(m_actionNewGame);
    menuGame->addAction(m_actionNewRatedGame);
    menuGame->addSeparator();
    QMenu* menuVariant = menuGame->addMenu(tr("&Game Variant"));
    menuVariant->addAction(m_actionVariantClassic);
    menuVariant->addAction(m_actionVariantClassic2);
    menuVariant->addAction(m_actionVariantDuo);
    menuVariant->addAction(m_actionVariantTrigon);
    menuVariant->addAction(m_actionVariantTrigon3);
    menuVariant->addAction(m_actionVariantTrigon2);
    menuVariant->addAction(m_actionVariantJunior);
    menuGame->addAction(m_actionComputerColors);
    menuGame->addAction(m_actionGameInfo);
    menuGame->addSeparator();
    menuGame->addAction(m_actionUndo);
    menuGame->addAction(m_actionFindMove);

    QMenu* menuGo = menuBar()->addMenu(tr("&Go"));
    menuGo->addAction(m_actionBeginning);
    menuGo->addAction(m_actionBackward10);
    menuGo->addAction(m_actionBackward);
    menuGo->addAction(m_actionForward);
    menuGo->addAction(m_actionForward10);
    menuGo->addAction(m_actionEnd);
    menuGo->addAction(m_actionNextVariation);
    menuGo->addAction(m_actionPreviousVariation);
    menuGo->addSeparator();
    menuGo->addAction(m_actionGotoMove);
    menuGo->addAction(m_actionBackToMainVariation);
    menuGo->addAction(m_actionBeginningOfBranch);

    QMenu* menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuEdit->addAction(m_actionFindNextComment);
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
    menuEdit->addAction(m_actionDeleteAllVariations);
    menuEdit->addAction(m_actionMoveUpVariation);
    menuEdit->addAction(m_actionMoveDownVariation);
    menuEdit->addAction(m_actionTruncate);
    menuEdit->addAction(m_actionTruncateChildren);
    menuEdit->addAction(m_actionKeepOnlyPosition);
    menuEdit->addAction(m_actionKeepOnlySubtree);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionSetupMode);
    menuEdit->addAction(m_actionSelectNextColor);

    QMenu* menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(m_actionShowToolbar);
    menuView->addAction(m_actionShowComment);
    menuView->addSeparator();
    QMenu* menuMoveNumbers = menuView->addMenu(tr("&Move Numbers"));
    menuMoveNumbers->addAction(m_actionMoveNumbersLast);
    menuMoveNumbers->addAction(m_actionMoveNumbersAll);
    menuMoveNumbers->addAction(m_actionMoveNumbersNone);
    menuView->addAction(m_actionCoordinates);
    menuView->addAction(m_actionShowVariations);
    menuView->addSeparator();
    menuView->addAction(m_actionFullscreen);

    QMenu* menuComputer = menuBar()->addMenu(tr("&Computer"));
    menuComputer->addAction(m_actionPlay);
    menuComputer->addAction(m_actionPlaySingleMove);
    menuComputer->addAction(m_actionInterrupt);
    menuComputer->addSeparator();
    QMenu* menuLevel = menuComputer->addMenu(tr("&Level"));
    for (int i = 0; i < maxLevel; ++i)
        menuLevel->addAction(m_actionLevel[i]);

    QMenu* menuTools = menuBar()->addMenu(tr("&Tools"));
    menuTools->addAction(m_actionShowRating);
    menuTools->addAction(m_actionAnalyzeGame);

    QMenu* menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(m_actionHelp);
    menuHelp->addAction(m_actionAbout);
}

QLayout* MainWindow::createOrientationButtonBoxLeft()
{
    QVBoxLayout* outerLayout = new QVBoxLayout();
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(QMargins());
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
    QVBoxLayout* outerLayout = new QVBoxLayout();
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(QMargins());
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
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(QMargins());
    layout->addStretch();
    layout->addLayout(createOrientationButtonBoxLeft());
    layout->addSpacing(8);
    m_orientationDisplay = new OrientationDisplay(0, getBoard());
    connect(m_orientationDisplay, SIGNAL(colorClicked(Color)),
            this, SLOT(orientationDisplayColorClicked(Color)));
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
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addLayout(createOrientationSelector(), 20);
    m_scoreDisplay = new ScoreDisplay();
    layout->addWidget(m_scoreDisplay, 3);
    SameHeightLayout* pieceSelectorLayout = new SameHeightLayout();
    layout->addLayout(pieceSelectorLayout, 80);
    for (ColorIterator i(Color::range); i; ++i)
    {
        m_pieceSelector[*i] = new PieceSelector(0, getBoard(), *i);
        connect(m_pieceSelector[*i],
                SIGNAL(pieceSelected(Color,Piece,const Transform*)),
                this, SLOT(selectPiece(Color,Piece,const Transform*)));
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
    QPushButton* deleteButton =
        msgBox.addButton(tr("Delete Variations"), QMessageBox::DestructiveRole);
    QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
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
    m_toolBar = new QToolBar();
    m_toolBar->setMovable(false);
    m_toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    m_toolBar->addAction(m_actionNewGame);
    m_toolBar->addAction(m_actionPlay);
    m_toolBar->addAction(m_actionComputerColors);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionBeginning);
    m_toolBar->addAction(m_actionBackward10);
    m_toolBar->addAction(m_actionBackward);
    m_toolBar->addAction(m_actionForward);
    m_toolBar->addAction(m_actionForward10);
    m_toolBar->addAction(m_actionEnd);
    m_toolBar->addAction(m_actionNextVariation);
    m_toolBar->addAction(m_actionPreviousVariation);
    addToolBar(m_toolBar);
    m_actionShowToolbar = m_toolBar->toggleViewAction();
    m_actionShowToolbar->setText(tr("&Toolbar"));
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
    QString file = QFileDialog::getSaveFileName(this, "", "",
                                                tr("Text files (*.txt)"));
    if (file.isEmpty())
        return;
    ofstream out(file.toLocal8Bit().constData());
    const Board& bd = getBoard();
    bd.write(out, false);
    if (! out)
        showError(strerror(errno));
}

void MainWindow::exportImage()
{
    QSettings settings;
    int size = settings.value("export_image_size", 420).toInt();
    bool ok;
    size = QInputDialog::getInt(this, tr("Export Image"), tr("Image size:"),
                                size, 0, 2147483647, 40, &ok);
    if (! ok)
        return;
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
    const Board& bd = getBoard();
    boardPainter.paintEmptyBoard(painter, size, size, bd.get_variant(),
                                 bd.get_geometry());
    boardPainter.paintPieces(painter, bd.get_grid(), &m_guiBoard->getLabels());
    painter.end();
    QString file;
    while (true)
    {
        file = QFileDialog::getSaveFileName(this, file);
        if (file.isEmpty())
            break;
        QImageWriter writer(file);
        if (writer.write(image))
            break;
        else
            showError(writer.errorString());
    }
}

void MainWindow::findMove()
{
    const Board& bd = getBoard();
    if (bd.is_game_over())
        return;
    if (m_legalMoves->empty())
    {
        bd.gen_moves(m_currentColor, *m_legalMoves);
        sort(m_legalMoves->begin(), m_legalMoves->end(),
             bind(&isMoveBetter, &bd, placeholders::_1, placeholders::_2));
    }
    if (m_legalMoves->empty())
    {
        // m_currentColor must have moves if game is not over
        LIBBOARDGAME_ASSERT(false);
        return;
    }
    if (m_legalMoveIndex >= m_legalMoves->size())
        m_legalMoveIndex = 0;
    Move mv = (*m_legalMoves)[m_legalMoveIndex];
    selectPiece(m_currentColor, bd.get_move_info(mv).piece);
    m_guiBoard->showMove(m_currentColor, mv);
    ++m_legalMoveIndex;
}

void MainWindow::findNextComment()
{
    const Node& root = m_game->get_root();
    const Node& current = m_game->get_current();
    const Node* node = find_next_comment(current);
    if (node == 0 && &current != &root)
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("The end of the tree was reached."),
                     tr("Continue the search from the start of the tree?"));
        QPushButton* continueButton =
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
    if (node == 0)
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
    const Board& bd = getBoard();
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
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
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
    const Board& bd = getBoard();
    transform = bd.get_transforms().get_mirrored_vertically(transform);
    transform = bd.get_piece_info(piece).get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::forward()
{
    const Node* node = m_game->get_current().get_first_child_or_null();
    if (node == 0)
        return;
    gotoNode(*node);
}

void MainWindow::forward10()
{
    const Tree& tree = m_game->get_tree();
    const Node* node = &m_game->get_current();
    unsigned n = 0;
    while (n < 10)
    {
        if (tree.has_move(*node))
            ++n;
        const Node* child = node->get_first_child_or_null();
        if (child == 0)
            break;
        node = child;
    }
    gotoNode(*node);
}

void MainWindow::fullscreen(bool checked)
{
    if (checked)
    {
        showFullScreen();
        if (isUbuntuUnity())
        {
            // See comment at m_buttonFullscreen
            statusBar()->addPermanentWidget(m_buttonFullscreen);
            m_buttonFullscreen->show();
        }
    }
    else
    {
        showNormal();
        statusBar()->removeWidget(m_buttonFullscreen);
    }
}

void MainWindow::gameInfo()
{
    GameInfoDialog dialog(this, *m_game);
    dialog.exec();
    updateWindow(false);
}

void MainWindow::variantClassic(bool checked)
{
    if (checked)
        setVariant(variant_classic);
}

void MainWindow::variantClassic2(bool checked)
{
    if (checked)
        setVariant(variant_classic_2);
}

void MainWindow::variantDuo(bool checked)
{
    if (checked)
        setVariant(variant_duo);
}

void MainWindow::variantJunior(bool checked)
{
    if (checked)
        setVariant(variant_junior);
}

void MainWindow::variantTrigon(bool checked)
{
    if (checked)
        setVariant(variant_trigon);
}

void MainWindow::variantTrigon2(bool checked)
{
    if (checked)
        setVariant(variant_trigon_2);
}

void MainWindow::variantTrigon3(bool checked)
{
    if (checked)
        setVariant(variant_trigon_3);
}

void MainWindow::genMove(bool playSingleMove)
{
    ++m_genMoveId;
    // Show "Computer is thinking" status only after a delay such that it is
    // not shown for very short thinking times
    QTimer::singleShot(800, this, SLOT(showThinking()));
    m_actionInterrupt->setEnabled(true);
    clearSelectedPiece();
    clear_abort();
    m_player->set_level(m_level);
    QFuture<GenMoveResult> future =
        QtConcurrent::run(this, &MainWindow::asyncGenMove, m_currentColor,
                          m_genMoveId, playSingleMove);
    m_genMoveWatcher.setFuture(future);
    m_isGenMoveRunning = true;
    const Board& bd = getBoard();
    unsigned nuMoves = bd.get_nu_moves();
    if (m_lastComputerMovesBegin == 0 && ! computerPlaysAll())
    {
        m_lastComputerMovesBegin = nuMoves + 1;
        m_lastComputerMovesEnd = m_lastComputerMovesBegin;
    }
}

void MainWindow::genMoveFinished()
{
    GenMoveResult result = m_genMoveWatcher.future().result();
    if (result.genMoveId != m_genMoveId)
        // Callback from a move generation canceled with cancelThread()
        return;
    LIBBOARDGAME_ASSERT(m_isGenMoveRunning);
    m_isGenMoveRunning = false;
    setCursor(QCursor(Qt::ArrowCursor));
    m_actionInterrupt->setEnabled(false);
    clearStatus();
    if (get_abort() && computerPlaysAll())
        m_computerColors.fill(false);
    Color c = result.color;
    Move mv = result.move;
    if (mv.is_null())
    {
        showError(tr("Computer failed to generate a move."));
        return;
    }
    const Board& bd = getBoard();
    if (! bd.is_legal(c, mv))
    {
        showError(tr("Computer generated illegal move: %1")
                  .arg(bd.to_string(mv).c_str()));
        return;
    }
    if (mv.is_pass())
        return;
    m_lastComputerMovesEnd = bd.get_nu_moves() + 1;
    bool checkComputerMove = ! result.playSingleMove;
    play(c, mv, checkComputerMove);
}

QString MainWindow::getFilter() const
{
    return tr("Blokus games (*.blksgf);;All files (*.*)");
}

QString MainWindow::getLastDir()
{
    QSettings settings;
    QString dir = settings.value("last_dir", "").toString();
    if (dir.isEmpty() || ! QFileInfo(dir).exists())
        dir = QDir::home().path();
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
    vector<const Node*> nodes;
    const Tree& tree = m_game->get_tree();
    const Node* node = &m_game->get_current();
    do
    {
        if (! tree.get_move(*node).is_null())
            nodes.insert(nodes.begin(), node);
        node = node->get_parent_or_null();
    }
    while (node != 0);
    node = m_game->get_current().get_first_child_or_null();
    while (node != 0)
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
    bool ok;
    int i = QInputDialog::getInt(this, tr("Go to Move"), tr("Move number:"),
                                 defaultValue, 1,
                                 static_cast<int>(nodes.size()), 1, &ok);
    if (! ok)
        return;
    gotoNode(*nodes[i - 1]);
}

void MainWindow::gotoNode(const Node& node)
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
    if (m_analyzeGameWindow != 0 && m_analyzeGameWindow->isVisible())
        m_analyzeGameWindow->analyzeGameWidget
            ->setCurrentPosition(*m_game, node);
    updateWindow(true);
}

void MainWindow::gotoPosition(Variant variant,
                              const vector<ColorMove>& moves)
{
    if (getVariant() != variant)
        return;
    const Tree& tree = m_game->get_tree();
    const Node* node = &tree.get_root();
    if (tree.has_move_ignore_invalid(*node))
        // Move in root node not supported.
        return;
    BOOST_FOREACH(ColorMove mv, moves)
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
    if (m_helpWindow != 0)
    {
        m_helpWindow->show();
        m_helpWindow->raise();
        return;
    }
    QString path = HelpWindow::findMainPage(m_manualDir, "index.html",
                                            QLocale::system().name());
    m_helpWindow = new HelpWindow(this, path);
    m_helpWindow->show();
}

void MainWindow::initGame()
{
    setRated(false);
    if (m_analyzeGameWindow != 0)
    {
        delete m_analyzeGameWindow;
        m_analyzeGameWindow = 0;
    }
    m_game->init();
    m_game->set_charset("UTF-8");
#ifdef VERSION
    m_game->set_application("Pentobi", VERSION);
#else
    m_game->set_application("Pentobi");
#endif
    m_game->set_date_today();
    m_game->set_modified(false);
    m_computerColors.fill(false);
    QSettings settings;
    if (! settings.value("computer_color_none").toBool())
    {
        Variant variant = getVariant();
        if (variant == variant_duo
            || variant == variant_junior)
            m_computerColors[Color(1)] = true;
        else if (variant == variant_classic_2
                 || variant == variant_trigon_2)
        {
            m_computerColors[Color(1)] = true;
            m_computerColors[Color(3)] = true;
        }
    }
    m_currentColor = Color(0);
    leaveSetupMode();
    m_lastComputerMovesBegin = 0;
    m_gameFinished = false;
    setFile("");
}

void MainWindow::initVariantActions()
{
    switch (getVariant())
    {
    case variant_classic:
        m_actionVariantClassic->setChecked(true);
        break;
    case variant_classic_2:
        m_actionVariantClassic2->setChecked(true);
        break;
    case variant_duo:
        m_actionVariantDuo->setChecked(true);
        break;
    case variant_junior:
        m_actionVariantJunior->setChecked(true);
        break;
    case variant_trigon:
        m_actionVariantTrigon->setChecked(true);
        break;
    case variant_trigon_2:
        m_actionVariantTrigon2->setChecked(true);
        break;
    case variant_trigon_3:
        m_actionVariantTrigon3->setChecked(true);
        break;
    }
}

void MainWindow::initPieceSelectors()
{
    const Board& bd = getBoard();
    for (unsigned i = 0; i < Color::range; ++i)
    {
        bool isVisible = (i < bd.get_nu_colors());
        m_pieceSelector[Color(i)]->setVisible(isVisible);
        if (isVisible)
            m_pieceSelector[Color(i)]->init();
    }
}

void MainWindow::initQuestion(QMessageBox& msgBox, const QString& text,
                              const QString& infoText)
{
    Util::setNoTitle(msgBox);
    // Workaround to avoid very small widths if the main text is short, which
    // causes ugly word wrapping with single-word lines in the informative text.
    // Why does QMessageBox::setMinimumWidth() not work (tested in Qt 4.7)?
    QString expandedText = text;
    QFontMetrics metrics(qApp->font("QLabel"));
    int minWidth = 30 * metrics.averageCharWidth();
    while (metrics.width(expandedText) < minWidth)
        expandedText.append(" ");
    msgBox.setText(expandedText);
    msgBox.setInformativeText(infoText);
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
    set_abort();
}

void MainWindow::keepOnlyPosition()
{
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Keep only position?"),
                 tr("All previous and following moves and variations will"
                    " be removed from the game tree."));
    QPushButton* keepOnlyPositionButton =
        msgBox.addButton(tr("Keep Only Position"),
                         QMessageBox::DestructiveRole);
    QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
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
    QPushButton* keepOnlySubtreeButton =
        msgBox.addButton(tr("Keep Only Subtree"),
                         QMessageBox::DestructiveRole);
    QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();
    if (msgBox.clickedButton() != keepOnlySubtreeButton)
        return;
    cancelThread();
    m_game->keep_only_subtree();
    updateWindow(true);
}

void MainWindow::leaveSetupMode()
{
    if (! m_actionSetupMode->isChecked())
        return;
    setupMode(false);
}

void MainWindow::loadHistory()
{
    Variant variant = m_game->get_variant();
    if (m_history->getVariant() == variant)
        return;
    m_history->load(variant);
    if (m_ratingDialog != 0)
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
    const Board& bd = getBoard();
    const Board::PiecesLeftList& piecesLeft =
        bd.get_pieces_left(m_currentColor);
    unsigned nuPiecesLeft = piecesLeft.size();
    if (nuPiecesLeft == 0)
        return;
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        piece = piecesLeft[0];
    else
    {
        for (unsigned i = 0; i < nuPiecesLeft; ++i)
            if (piecesLeft[i] == piece)
            {
                if (i + 1 >= nuPiecesLeft)
                    piece = piecesLeft[0];
                else
                    piece = piecesLeft[i + 1];
                break;
            }
    }
    selectPiece(m_currentColor, piece);
}

void MainWindow::nextTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
    transform = getBoard().get_piece_info(piece).get_next_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::nextVariation()
{
    const Node* node = m_game->get_current().get_sibling();
    if (node == 0)
        return;
    gotoNode(*node);
}

void MainWindow::nextVariation10()
{
    const Node* node = &m_game->get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        if (node->get_sibling() == 0)
            break;
        node = node->get_sibling();
    }
    gotoNode(*node);
}

void MainWindow::newRatedGame()
{
    if (! checkSave())
        return;
    cancelThread();
    if (m_history->getNuGames() == 0)
    {
        InitialRatingDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted)
            return;
        m_history->init(Rating(dialog.getRating()));
    }
    int level;
    m_history->getNextRatedGameSettings(maxLevel, level, m_ratedGameColor);
    QMessageBox msgBox(this);
    initQuestion(msgBox, tr("Start new rated game?"),
                 "<html>" +
                 tr("In the next game, you will play %1 against"
                    " Pentobi level&nbsp;%2.")
                 .arg(getPlayerString(getVariant(), m_ratedGameColor))
                 .arg(level));
    QPushButton* startGameButton =
        msgBox.addButton(tr("&Start Game"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(startGameButton);
    msgBox.exec();
    QAbstractButton* result = msgBox.clickedButton();
    if (result != startGameButton)
        return;
    setLevel(level);
    initGame();
    setFile("");
    setRated(true);
    m_computerColors.fill(true);
    const Board& bd = getBoard();
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        if (bd.is_same_player(*i, m_ratedGameColor))
            m_computerColors[*i] = false;
    QString computerPlayerName =
        //: The first argument is the version of Pentobi
        tr("Pentobi %1 (level %2)").arg(getVersion()).arg(level);
    string charset = m_game->get_root().get_property("CA", "");
    string computerPlayerNameStdStr =
        Util::convertSgfValueFromQString(computerPlayerName, charset);
    string humanPlayerNameStdStr =
        Util::convertSgfValueFromQString(tr("Human"), charset);
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        if (m_computerColors[*i])
            m_game->set_player_name(*i, computerPlayerNameStdStr);
        else
            m_game->set_player_name(*i, humanPlayerNameStdStr);
    deleteAutoSaveFile();
    updateWindow(true);
    checkComputerMove();
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
    if (! isTemporary)
    {
        setFile(file);
        deleteAutoSaveFile();
    }
    if (m_analyzeGameWindow != 0)
    {
        delete m_analyzeGameWindow;
        m_analyzeGameWindow = 0;
    }
    setRated(false);
    try
    {
        unique_ptr<Node> tree = reader.get_tree_transfer_ownership();
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
    if (isTemporary)
        // Set as modified to enable the Save action in updateWindowModified()
        m_game->set_modified(true);
    m_computerColors.fill(false);
    leaveSetupMode();
    m_lastComputerMovesBegin = 0;
    initVariantActions();
    updateWindow(true);
    loadHistory();
    return true;
}

void MainWindow::openRecentFile()
{
     QAction* action = qobject_cast<QAction*>(sender());
     if (action == 0)
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
    bool isSetupMode = m_actionSetupMode->isChecked();
    if (m_computerColors[c] || isSetupMode)
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
        play(c, mv, true);
}

void MainWindow::play()
{
    cancelThread();
    leaveSetupMode();
    Variant variant = getVariant();
    if (variant != variant_classic && variant != variant_trigon
         && variant != variant_trigon_3)
    {
        QSettings settings;
        settings.setValue("computer_color_none", false);
    }
    if (! m_computerColors[m_currentColor])
    {
        m_computerColors.fill(false);
        m_computerColors[m_currentColor] = true;
        if (variant == variant_classic_2
            || variant == variant_trigon_2)
        {
            if (m_currentColor == Color(0) || m_currentColor == Color(2))
                m_computerColors[Color(0)] = m_computerColors[Color(2)] = true;
            else
                m_computerColors[Color(1)] = m_computerColors[Color(3)] = true;
        }
        else
            m_computerColors[m_currentColor] = true;
    }
    genMove();
}

void MainWindow::play(Color c, Move mv, bool checkComputerMove)
{
    const Board& bd = getBoard();
    m_game->play(c, mv, false);
    c = m_game->get_to_play();
    m_gameFinished = false;
    if (bd.is_game_over())
    {
        updateWindow(true);
        repaint();
        if (m_isRated && ! m_isRatedGameFinished)
        {
            updateRating();
            m_isRatedGameFinished = true;
        }
        m_gameFinished = true;
        deleteAutoSaveFile();
        return;
    }
    updateWindow(true);
    repaint();
    m_currentColor = m_game->get_effective_to_play();
    updateWindow(true);
    if (checkComputerMove)
        MainWindow::checkComputerMove();
}

void MainWindow::playSingleMove()
{
    cancelThread();
    leaveSetupMode();
    genMove(true);
}

void MainWindow::pointClicked(Point p)
{
    if (! m_actionSetupMode->isChecked())
        return;
    const Board& bd = getBoard();
    PointState s = bd.get_point_state(p);
    if (s.is_empty())
        return;
    m_game->remove_setup(s.to_color(), bd.get_played_move(p));
    setSetupPlayer();
    updateWindow(true);
}

void MainWindow::previousPiece()
{
    const Board& bd = getBoard();
    const Board::PiecesLeftList& piecesLeft = bd.get_pieces_left(m_currentColor);
    unsigned nuPiecesLeft = piecesLeft.size();
    if (nuPiecesLeft == 0)
        return;
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        piece = piecesLeft[nuPiecesLeft - 1];
    else
    {
        for (unsigned i = 0; i < nuPiecesLeft; ++i)
            if (piecesLeft[i] == piece)
            {
                if (i == 0)
                    piece = piecesLeft[nuPiecesLeft - 1];
                else
                    piece = piecesLeft[i - 1];
                break;
            }
    }
    selectPiece(m_currentColor, piece);
}

void MainWindow::previousTransform()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
    transform =
        getBoard().get_piece_info(piece).get_previous_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::previousVariation()
{
    const Node* node = m_game->get_current().get_previous_sibling();
    if (node == 0)
        return;
    gotoNode(*node);
}

void MainWindow::previousVariation10()
{
    const Node* node = &m_game->get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        if (node->get_previous_sibling() == 0)
            break;
        node = node->get_previous_sibling();
    }
    gotoNode(*node);
}

void MainWindow::quit()
{
    if (! checkQuit())
        return;
    qApp->quit();
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
    QStringList files = settings.value("recent_files").toStringList();
    files.removeAll(canonicalFile);
    files.prepend(canonicalFile);
    while (files.size() > maxRecentFiles)
        files.removeLast();
    settings.setValue("recent_files", files);
    settings.sync(); // updateRecentFiles() needs the new settings
    updateRecentFiles();
}

void MainWindow::rotatePieceAnticlockwise()
{
    Piece piece = m_guiBoard->getSelectedPiece();
    if (piece.is_null())
        return;
    const Board& bd = getBoard();
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
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
    const Board& bd = getBoard();
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
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
        m_game->set_modified(false);
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
        showStatus(tr("File saved %1").arg(file), true);
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
            m_game->set_modified(false);
            updateWindow(false);
        }
        setFile(file);
    }
}

void MainWindow::selectNamedPiece(const char* name1, const char* name2,
                                  const char* name3, const char* name4)
{
    const Board& bd = getBoard();
    vector<Piece> pieces;
    Piece piece;
    if (bd.get_piece_by_name(name1, piece)
        && bd.is_piece_left(m_currentColor, piece))
        pieces.push_back(piece);
    if (name2 != 0 && bd.get_piece_by_name(name2, piece)
        && bd.is_piece_left(m_currentColor, piece))
        pieces.push_back(piece);
    if (name3 != 0 && bd.get_piece_by_name(name3, piece)
        && bd.is_piece_left(m_currentColor, piece))
        pieces.push_back(piece);
    if (name4 != 0 && bd.get_piece_by_name(name4, piece)
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
    const Board& bd = getBoard();
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
    selectNamedPiece("1");
}

void MainWindow::selectPiece2()
{
    selectNamedPiece("2");
}

void MainWindow::selectPieceA()
{
    selectNamedPiece("A6", "A4");
}

void MainWindow::selectPieceC()
{
    selectNamedPiece("C5", "C4");
}

void MainWindow::selectPieceF()
{
    selectNamedPiece("F");
}

void MainWindow::selectPieceG()
{
    selectNamedPiece("G");
}

void MainWindow::selectPieceI()
{
    selectNamedPiece("I6", "I5", "I4", "I3");
}

void MainWindow::selectPieceL()
{
    selectNamedPiece("L6", "L5", "L4");
}

void MainWindow::selectPieceN()
{
    selectNamedPiece("N");
}

void MainWindow::selectPieceO()
{
    selectNamedPiece("O");
}

void MainWindow::selectPieceP()
{
    selectNamedPiece("P6", "P5", "P");
}

void MainWindow::selectPieceS()
{
    selectNamedPiece("S");
}

void MainWindow::selectPieceT()
{
    selectNamedPiece("T5", "T4");
}

void MainWindow::selectPieceU()
{
    selectNamedPiece("U");
}

void MainWindow::selectPieceV()
{
    selectNamedPiece("V", "V5", "V3");
}

void MainWindow::selectPieceW()
{
    selectNamedPiece("W");
}

void MainWindow::selectPieceX()
{
    selectNamedPiece("X");
}

void MainWindow::selectPieceY()
{
    selectNamedPiece("Y");
}

void MainWindow::selectPieceZ()
{
    selectNamedPiece("Z5", "Z4");
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
}

void MainWindow::setFile(const QString& file)
{
    m_file = file;
    // Don't use setWindowFilePath(), it is buggy in Qt 4.6.
    if (m_file.isEmpty())
        setWindowTitle(tr("Pentobi"));
    else
    {
        QFileInfo info(m_file);
        setWindowTitle(tr("%1[*] - Pentobi").arg(info.fileName()));
    }
}

void MainWindow::setLevel(int level)
{
    if (level <= 0 || level > maxLevel)
        return;
    m_level = level;
    m_actionLevel[level - 1]->setChecked(true);
    QSettings settings;
    settings.setValue("level", m_level);
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
    Variant variant = getVariant();
    Color c = m_currentColor;
    bool isComputerColor = m_computerColors[m_currentColor];
    if (variant == variant_classic_2 || variant == variant_trigon_2)
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
            (variant == variant_duo || variant == variant_junior);
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
        statusBar()->addWidget(m_ratedGameLabel);
        m_ratedGameLabel->show();
        m_isRatedGameFinished = false;
    }
    else
        statusBar()->removeWidget(m_ratedGameLabel);
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
        enablePieceSelector(m_currentColor);
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
    showMessage(QMessageBox::Critical, text, infoText, detailText);
}

void MainWindow::showInfo(const QString& text, const QString& infoText,
                          const QString& detailText, bool withIcon)
{
    showMessage(withIcon ? QMessageBox::Information : QMessageBox::NoIcon,
                text, infoText, detailText);
}

void MainWindow::showInvalidFile(QString file, const Exception& e)
{
    showError(tr("Error in file '%1'").arg(QFileInfo(file).fileName()),
              tr("The file is not a valid Blokus SGF file."), e.what());
}

void MainWindow::showMessage(QMessageBox::Icon icon, const QString& text,
                             const QString& infoText, const QString& detailText)
{
    // Workaround to avoid very small widths if the main text is short, which
    // causes ugly word wrapping with single-word lines in the informative text.
    // Why does QMessageBox::setMinimumWidth() not work (tested in Qt 4.7)?
    QString expandedText = text;
    QFontMetrics metrics(qApp->font("QLabel"));
    int minWidth = 30 * metrics.averageCharWidth();
    while (metrics.width(expandedText) < minWidth)
        expandedText.append(" ");
    QMessageBox msgBox(this);
    Util::setNoTitle(msgBox);
    msgBox.setIcon(icon);
    msgBox.setText(expandedText);
    msgBox.setInformativeText(infoText);
    msgBox.setDetailedText(detailText);
    msgBox.exec();
}

void MainWindow::showRating()
{
    if (m_ratingDialog == 0)
    {
        m_ratingDialog = new RatingDialog(this, *m_history);
        connect(m_ratingDialog, SIGNAL(open(const QString&)),
                this, SLOT(open(const QString&)));
    }
    loadHistory();
    m_ratingDialog->show();
}

void MainWindow::showResult()
{
    Variant variant = getVariant();
    const Board& bd = getBoard();
    QString text;
    if (variant == variant_duo || variant == variant_junior)
    {
        int score = bd.get_score(Color(0));
        if (score > 0)
            text = tr("Blue wins with %n point(s).", "", score);
        else if (score < 0)
            text = tr("Green wins with %n point(s).", "", -score);
        else
            text = tr("The game ends in a tie.");
    }
    else if (variant == variant_classic_2 || variant == variant_trigon_2)
    {
        int score = bd.get_score(Color(0));
        if (score > 0)
            text = tr("Blue/Red wins with %n point(s).", "", score);
        else if (score < 0)
            text = tr("Yellow/Green wins with %n point(s).", "", -score);
        else
            text = tr("The game ends in a tie.");
    }
    else if (variant == variant_trigon_3)
    {
        unsigned blue = bd.get_points_with_bonus(Color(0));
        unsigned yellow = bd.get_points_with_bonus(Color(1));
        unsigned red = bd.get_points_with_bonus(Color(2));
        unsigned maxPoints = max(blue, max(yellow, red));
        if (blue == yellow && yellow == red)
            text = tr("The game ends in a tie between all colors.");
        else if (blue == maxPoints && blue == yellow)
            text = tr("The game ends in a tie between Blue and Yellow.");
        else if (blue == maxPoints && blue == red)
            text = tr("The game ends in a tie between Blue and Red.");
        else if (yellow == maxPoints && yellow == red)
            text = tr("The game ends in a tie between Yellow and Red.");
        else if (blue == maxPoints)
            text = tr("Blue wins.");
        else if (yellow == maxPoints)
            text = tr("Yellow wins.");
        else
            text = tr("Red wins.");
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == variant_classic
                            || variant == variant_trigon);
        unsigned blue = bd.get_points_with_bonus(Color(0));
        unsigned yellow = bd.get_points_with_bonus(Color(1));
        unsigned red = bd.get_points_with_bonus(Color(2));
        unsigned green = bd.get_points_with_bonus(Color(3));
        unsigned maxPoints = max(blue, max(yellow, max(red, green)));
        if (blue == yellow && yellow == red && red == green)
            text = tr("The game ends in a tie between all colors.");
        else if (blue == maxPoints && blue == yellow && yellow == red)
            text = tr("The game ends in a tie between Blue, Yellow and Red.");
        else if (blue == maxPoints && blue == yellow && yellow == green)
            text =
                tr("The game ends in a tie between Blue, Yellow and Green.");
        else if (blue == maxPoints && blue == red && red == green)
            text = tr("The game ends in a tie between Blue, Red and Green.");
        else if (yellow == maxPoints && yellow == red && red == green)
            text = tr("The game ends in a tie between Yellow, Red and Green.");
        else if (blue == maxPoints && blue == yellow)
            text = tr("The game ends in a tie between Blue and Yellow.");
        else if (blue == maxPoints && blue == red)
            text = tr("The game ends in a tie between Blue and Red.");
        else if (blue == maxPoints && blue == green)
            text = tr("The game ends in a tie between Blue and Green.");
        else if (yellow == maxPoints && yellow == red)
            text = tr("The game ends in a tie between Yellow and Red.");
        else if (yellow == maxPoints && yellow == green)
            text = tr("The game ends in a tie between Yellow and Green.");
        else if (red == maxPoints && red == green)
            text = tr("The game ends in a tie between Red and Green.");
        else if (blue == maxPoints)
            text = tr("Blue wins.");
        else if (yellow == maxPoints)
            text = tr("Yellow wins.");
        else if (red == maxPoints)
            text = tr("Red wins.");
        else
            text = tr("Green wins.");
    }
    showStatus(text);
}

void MainWindow::showStatus(const QString& text, bool temporary)
{
    int timeout = (temporary ? 4000 : 0);
    statusBar()->showMessage(text, timeout);
}

void MainWindow::showThinking()
{
    if (! m_isGenMoveRunning)
        return;
    setCursor(QCursor(Qt::BusyCursor));
    showStatus(tr("The computer is thinking..."));
}

QSize MainWindow::sizeHint() const
{
    return QSize(1020, 634);
}

void MainWindow::truncate()
{
    const Node& current = m_game->get_current();
    if (! current.has_parent())
        return;
    cancelThread();
    if (current.has_children())
    {
        QMessageBox msgBox(this);
        initQuestion(msgBox, tr("Truncate this subtree?"),
                     tr("This position and all following moves and"
                        " variations will be removed from the game tree."));
        QPushButton* truncateButton =
            msgBox.addButton(tr("Truncate"),
                             QMessageBox::DestructiveRole);
        QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
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
    QPushButton* truncateButton =
        msgBox.addButton(tr("Truncate Children"),
                         QMessageBox::DestructiveRole);
    QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
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
    const Node& current = m_game->get_current();
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
    const Transform* transform = m_guiBoard->getSelectedPieceTransform();
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
    const Tree& tree = m_game->get_tree();
    const Node& current = m_game->get_current();
    unsigned move = get_move_number(tree, current);
    unsigned movesLeft = get_moves_left(tree, current);
    unsigned totalMoves = move + movesLeft;
    string variation = get_variation_string(current);
    if (variation.empty())
    {
        if (movesLeft == 0)
        {
            // If last move in main variation, show the number only if it is
            // not already displayed on the board.
            if (! m_actionMoveNumbersLast->isChecked())
            {
                m_moveNumber->setText(QString("%1").arg(move));
                m_moveNumber->setToolTip(tr("Move number %1").arg(move));
            }
            else
            {
                m_moveNumber->setText("");
                m_moveNumber->setToolTip("");
            }
        }
        else
        {
            m_moveNumber->setText(QString("%1/%2").arg(move).arg(totalMoves));
            if (move == 0)
                m_moveNumber->setToolTip(tr("%n move(s)", "", totalMoves));
            else
                m_moveNumber->setToolTip(tr("Move number %1 of %2")
                                         .arg(move).arg(totalMoves));
        }
    }
    else
    {
        if (movesLeft == 0)
            m_moveNumber->setText(QString("%1 (%2)")
                                  .arg(move).arg(variation.c_str()));
        else
            m_moveNumber->setText(QString("%1/%2 (%3)")
                                  .arg(move).arg(totalMoves)
                                  .arg(variation.c_str()));
        m_moveNumber->setToolTip(tr("Move number %1 of %2 in variation %3")
                                 .arg(move).arg(totalMoves)
                                 .arg(variation.c_str()));
    }
}

void MainWindow::updateRating()
{
    Variant variant = getVariant();
    const Board& bd = getBoard();
    QString text;
    int oldRating = m_history->getRating().toInt();
    unsigned place;
    bool isPlaceShared;
    bd.get_place(m_ratedGameColor, place, isPlaceShared);
    float gameResult;
    if (place == 0 && ! isPlaceShared)
        gameResult = 1;
    else if (place == 0 && isPlaceShared)
        gameResult = 0.5;
    else
        gameResult = 0;
    unsigned nuOpp = get_nu_players(variant) - 1;
    Rating oppRating = m_player->get_rating(variant);
    m_history->addGame(gameResult, oppRating, nuOpp, m_ratedGameColor,
                       gameResult, Tree::get_date_today(), m_level,
                       m_game->get_tree());
    if (m_ratingDialog != 0)
        m_ratingDialog->updateContent();
    int newRating = m_history->getRating().toInt();
    if (newRating > oldRating)
        text =
            tr("Your rating has increased from %1 to %2.")
            .arg(oldRating).arg(newRating);
    else if (newRating == oldRating)
        text = tr("Your rating stays at %1.").arg(oldRating);
    else
        text =
            tr("Your rating has decreased from %1 to %2.")
            .arg(oldRating).arg(newRating);
    showInfo(text, "", "", true);
}

void MainWindow::updateRecentFiles()
{
    QSettings settings;
    QStringList files = settings.value("recent_files").toStringList();
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
        QString text;
#ifdef Q_WS_MAC
        const bool isMac = true;
#else
        const bool isMac = false;
#endif
        if (! isMac && i + 1 <= 9)
            text =
                /*: Label in Recent Files menu. The first 10 items
                  are numbered to provide a mnemonic. %1 is replaced
                  by the number, %2 by the file name. */
                tr("&%1: %2").arg(i + 1).arg(name);
        else
            text = QString("%1").arg(name);
        m_actionRecentFile[i]->setText(text);
        m_actionRecentFile[i]->setData(files[i]);
        m_actionRecentFile[i]->setVisible(true);
    }
    for (int j = nuRecentFiles; j < maxRecentFiles; ++j)
        m_actionRecentFile[j]->setVisible(false);
}

void MainWindow::updateWindow(bool currentNodeChanged)
{
    const Board& bd = getBoard();
    updateWindowModified();
    m_guiBoard->copyFromBoard(bd);
    QSettings settings;
    bool markVariations = settings.value("show_variations", true).toBool();
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
        if (isGameOver)
            showResult();
        else
            clearStatus();
    }
    updateMoveNumber();
    setPlayToolTip();
    const Tree& tree = m_game->get_tree();
    const Node& current = m_game->get_current();
    bool isMain = is_main_variation(current);
    bool hasEarlierVariation = has_earlier_variation(current);
    bool hasParent = current.has_parent();
    bool hasChildren = current.has_children();
    bool hasMove = tree.has_move_ignore_invalid(current);
    bool hasMoves = bd.has_moves(m_currentColor);
    m_actionAnalyzeGame->setEnabled(tree.has_main_variation_moves());
    m_actionBackToMainVariation->setEnabled(! isMain);
    m_actionBeginning->setEnabled(hasParent);
    m_actionBeginningOfBranch->setEnabled(hasEarlierVariation);
    m_actionBackward->setEnabled(hasParent);
    m_actionBackward10->setEnabled(hasParent);
    m_actionDeleteAllVariations->setEnabled(tree.has_variations());
    m_actionForward->setEnabled(hasChildren);
    m_actionForward10->setEnabled(hasChildren);
    m_actionEnd->setEnabled(hasChildren);
    m_actionFindMove->setEnabled(! isGameOver);
    m_actionGotoMove->setEnabled(hasCurrentVariationOtherMoves(tree, current));
    m_actionKeepOnlyPosition->setEnabled(hasParent || hasChildren);
    m_actionKeepOnlySubtree->setEnabled(hasParent && hasChildren);
    m_actionMakeMainVariation->setEnabled(! isMain);
    m_actionMoveDownVariation->setEnabled(current.get_sibling());
    m_actionMoveUpVariation->setEnabled(hasParent
                       && &current.get_parent().get_first_child() != &current);
    m_actionNextVariation->setEnabled(current.get_sibling() != 0);
    m_actionPlay->setEnabled(hasMoves);
    m_actionPreviousVariation->setEnabled(current.get_previous_sibling() != 0);
    // See also comment in setupMode()
    m_actionSetupMode->setEnabled(! hasParent && ! hasChildren);
    m_actionTruncate->setEnabled(hasParent);
    m_actionTruncateChildren->setEnabled(hasChildren);
    m_actionUndo->setEnabled(hasParent && ! hasChildren && hasMove);
}

void MainWindow::updateWindowModified()
{
    bool modified = m_game->get_modified();
    m_actionSave->setEnabled(modified);
    if (! m_file.isEmpty())
        setWindowModified(modified);
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
        if (m_guiBoard->getSelectedPiece().is_null())
            for (int i = 0; i < delta; ++i)
                backward();
        else
            for (int i = 0; i < delta; ++i)
                nextTransform();
    }
    else if (delta < 0)
    {
        if (m_guiBoard->getSelectedPiece().is_null())
            for (int i = 0; i < -delta; ++i)
                forward();
        else
            for (int i = 0; i < -delta; ++i)
                previousTransform();
    }
    event->accept();
}

bool MainWindow::writeGame(const path& file)
{
    boost::filesystem::ofstream out(file);
    TreeWriter writer(out, m_game->get_tree());
    writer.set_one_prop_per_line(true);
    writer.set_one_prop_value_per_line(true);
    writer.set_indent(2);
    writer.write();
    return static_cast<bool>(out);
}

//-----------------------------------------------------------------------------
