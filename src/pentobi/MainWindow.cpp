//-----------------------------------------------------------------------------
/** @file pentobi/MainWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MainWindow.h"

#include <algorithm>
#include <fstream>
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Assert.h"
#include "libpentobi_gui/ComputerColorDialog.h"
#include "libpentobi_gui/GuiBoardUtil.h"
#include "libpentobi_gui/SameHeightLayout.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using boost::filesystem::path;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::back_to_main_variation;
using libboardgame_sgf::util::get_last_node;
using libboardgame_sgf::util::is_main_variation;
using libboardgame_sgf::util::write_tree;
using libboardgame_util::clear_abort;
using libboardgame_util::get_abort;
using libboardgame_util::log;
using libboardgame_util::set_abort;
using libboardgame_util::ArrayList;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::Piece;
using libpentobi_base::Tree;
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

QString getAutoSaveFile()
{
    QString s = QDir::home().path();
    s.append(QDir::separator());
    s.append("autosave.blksgf");
    return s;
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
    node = current.get_first_child();
    while (node != 0)
    {
        if (! tree.get_move(*node).is_null())
            return true;
        node = node->get_first_child();
    }
    return false;
}

void setIcon(QAction* action, const QString& name)
{
    action->setIcon(QIcon(QString(":/pentobi/%1.png").arg(name)));
}

} // namespace

//-----------------------------------------------------------------------------

MainWindow::MainWindow(const QString& initialFile)
    : m_isGenMoveRunning(false),
      m_lastMoveByComputer(false),
      m_genMoveId(0),
      m_help_window(0)
{
    QSettings settings;
    m_useTimeLimit = settings.value("use_time_limit", false).toBool();
    m_useBook = settings.value("use_book", true).toBool();
    m_level = settings.value("level", 4).toInt();
    if (m_level < 1 || m_level > maxLevel)
        m_level = 4;
    m_timeLimit = settings.value("time_limit", 10).toInt();
    if (m_timeLimit < 1)
        m_timeLimit = 10;
    QString variantString = settings.value("game_variant", "").toString();
    GameVariant variant;
    if (variantString == "duo")
        variant = game_variant_duo;
    else if (variantString == "classic_2")
        variant = game_variant_classic_2;
    else
        variant = game_variant_classic;
    m_game.reset(new Game(variant));
    initGame();
    path appDirPath(QCoreApplication::applicationDirPath().toStdString());
    m_player.reset(new Player(m_game->get_board(), appDirPath));
    createActions();
    createToolBar();
    setCentralWidget(createCentralWidget());
    connect(&m_genMoveWatcher, SIGNAL(finished()),
            this, SLOT(genMoveFinished()));
    connect(m_guiBoard, SIGNAL(play(Color, Move)),
            this, SLOT(humanPlay(Color, Move)));
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
    updateRecentFiles();
    statusBar();
    addAction(m_actionMoveSelectedPieceLeft);
    addAction(m_actionMoveSelectedPieceRight);
    addAction(m_actionMoveSelectedPieceUp);
    addAction(m_actionMoveSelectedPieceDown);
    addAction(m_actionNextPiece);
    addAction(m_actionNextTransform);
    addAction(m_actionPreviousPiece);
    addAction(m_actionPreviousTransform);
    addAction(m_actionPlaceSelectedPiece);
    addAction(m_actionSelectPiece1);
    addAction(m_actionSelectPiece2);
    addAction(m_actionSelectPieceF);
    addAction(m_actionSelectPieceI);
    addAction(m_actionSelectPieceL);
    addAction(m_actionSelectPieceN);
    addAction(m_actionSelectPieceO);
    addAction(m_actionSelectPieceP);
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
    bool coordinateLabels =
        settings.value("coordinate_labels", false).toBool();
    m_guiBoard->setDrawCoordLabels(coordinateLabels);
    m_actionCoordinateLabels->setChecked(coordinateLabels);
    bool showToolbar = settings.value("toolbar", true).toBool();
    m_toolBar->setVisible(showToolbar);
    initGameVariantActions();
    setWindowIcon(QIcon(":/pentobi/pentobi.png"));
    if (! restoreGeometry(settings.value("geometry").toByteArray()))
    {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        if (screenGeometry.width() >= 1200 && screenGeometry.height() >= 800)
            resize(1105, 690);
        else if (screenGeometry.width() >= 1024
                 && screenGeometry.height() >= 600)
            resize(950, 530);
    }
    showComment(false);
    m_splitter->restoreState(settings.value("splitter_state").toByteArray());
    m_actionShowComment->setChecked(m_comment->height() > 0);
    boardChanged(true);
    clearFile();
    if (! initialFile.isEmpty())
        open(initialFile);
    else
    {
        QString autoSaveFile = getAutoSaveFile();
        if (QFile(autoSaveFile).exists())
        {
            open(autoSaveFile, true);
            m_gameFinished = m_game->get_board().is_game_over();
            boardChanged(true);
        }
    }
    QString version = getVersion();
    if (! version.contains("UNKNOWN"))
    {
        QString lastVersion =
            settings.value("last_version_used", "").toString();
        if (lastVersion != version)
        {
            settings.setValue("last_version_used", version);
            show();
            about();
        }
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Pentobi"),
          QString("<style type=\"text/css\">"
                  ":link { text-decoration: none; }"
                  "</style>") +
          QString(tr(
              "<h2>Pentobi</h2>"
              "<p>Version %1</p>"
              "<p>"
              "Computer program that plays the board game Blokus."
              "<br>"
              "&copy; 2011 Markus Enzenberger"
              "<br>"
              "<a href=\"http://pentobi.sf.net\">http://pentobi.sf.net</a>"
              "</p>"
              "<p>"
              "You can support the development of this program by making a "
              "donation:<br>"
              "<a href=\"http://sf.net/projects/pentobi/donate\">http://sf.net/projects/pentobi/donate</a>"
              "</p>"
                     ))
                       .arg(getVersion()));
}

/** Call to Player::genmove() that runs in a different thread. */
MainWindow::GenMoveResult MainWindow::asyncGenMove(Color c, int genMoveId)
{
    GenMoveResult result;
    result.color = c;
    result.genMoveId = genMoveId;
    result.move = m_player->genmove(c);
    return result;
}

void MainWindow::badMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_bad_move();
    boardChanged(false);
}

void MainWindow::backward()
{
    const Node& node = m_game->get_current();
    if (! node.has_parent())
        return;
    gotoNode(node.get_parent());
}

void MainWindow::backToMainVariation()
{
    gotoNode(back_to_main_variation(m_game->get_current()));
}

void MainWindow::beginning()
{
    gotoNode(m_game->get_root());
}

void MainWindow::boardChanged(bool currentNodeChanged)
{
    const Board& bd = m_game->get_board();
    if (m_file.isEmpty())
    {
        m_actionSave->setEnabled(bd.get_nu_moves() > 0);
        m_actionSave->setToolTip(QString());
    }
    else
    {
        bool is_modified = m_game->get_modified();
        setWindowModified(is_modified);
        m_actionSave->setEnabled(is_modified);
        m_actionSave->setToolTip(QString(tr("Save (%1)")).arg(m_file));
    }
    m_guiBoard->copyFromBoard(bd);
    // If the last move was played by the computer, show move numbers on all
    // last subsequent moves by the computer because the computer could have
    // played them quickly if the other colors cannot move anymore.
    bool markAllLastBySameColor = m_lastMoveByComputer;
    gui_board_util::setMarkup(*m_guiBoard, *m_game,
                              m_actionMoveNumbersLast->isChecked(),
                              m_actionMoveNumbersAll->isChecked(),
                              markAllLastBySameColor);
    m_scoreDisplay->updateScore(bd);
    m_toPlay = m_game->get_effective_to_play();
    m_legalMoves.clear();
    m_legalMoveIndex = 0;
    bool isGameOver = bd.is_game_over();
    unsigned int nuPiecesLeft = bd.get_pieces_left(m_toPlay).size();
    if (isGameOver)
        m_orientationDisplay->clearSelectedColor();
    else
        m_orientationDisplay->selectColor(m_toPlay);
    if (currentNodeChanged)
    {
        clearSelectedPiece();
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        {
            m_pieceSelector[*i]->update();
            m_pieceSelector[*i]->setEnabled(m_toPlay == *i);
        }
        updateComment();
        updateMoveAnnotationActions();
    }
    const Tree& tree = m_game->get_tree();
    const Node& current = m_game->get_current();
    bool isMain = is_main_variation(current);
    bool noPieceSelected = (m_guiBoard->getSelectedPiece() == 0);
    m_actionBeginning->setEnabled(current.has_parent());
    m_actionBackward->setEnabled(current.has_parent());
    m_actionForward->setEnabled(current.has_children());
    m_actionEnd->setEnabled(current.has_children());
    m_actionFindMove->setEnabled(! isGameOver);
    m_actionGotoMove->setEnabled(hasCurrentVariationOtherMoves(tree, current));
    m_actionNextVariation->setEnabled(current.get_sibling() != 0);
    m_actionPreviousVariation->setEnabled(current.get_previous_sibling() != 0);
    m_actionBackToMainVariation->setEnabled(! isMain);
    m_actionMakeMainVariation->setEnabled(! isMain);
    m_actionTruncate->setEnabled(current.has_parent());
    m_actionNextPiece->setEnabled(! isGameOver
                                  && (nuPiecesLeft > 1
                                      || (nuPiecesLeft == 1
                                          && noPieceSelected)));
    m_actionPreviousPiece->setEnabled(! isGameOver && nuPiecesLeft > 1);
}

void MainWindow::cancelGenMove()
{
    if (! m_isGenMoveRunning)
        return;
    // After waitForFinished() returns, we can be sure that the move generation
    // is no longer running, but we will still receive the finished event.
    // Increasing m_genMoveId will make genMoveFinished() ignore the event.
    ++m_genMoveId;
    set_abort();
    m_genMoveWatcher.waitForFinished();
    m_isGenMoveRunning = false;
    QApplication::restoreOverrideCursor();
    m_actionInterrupt->setEnabled(false);
}

void MainWindow::checkComputerMove()
{
    bool isGameOver = m_game->get_board().is_game_over();
    if (! isGameOver && m_computerColor[m_toPlay])
        genMove();
}

bool MainWindow::checkSave()
{
    if (! m_file.isEmpty())
    {
        if (! m_game->get_modified())
            return true;
        QMessageBox::StandardButton button =
            showQuestion(tr("The file has been modified. Save changes?"),
                         QMessageBox::Yes | QMessageBox::No
                         | QMessageBox::Cancel);
        if (button == QMessageBox::Cancel)
            return false;
        if (button == QMessageBox::Yes)
        {
            if (m_file.isEmpty())
                saveAs();
            else
                save();
        }
        return true;
    }
    const Board& bd = m_game->get_board();
    if (bd.get_nu_moves() > 0 && ! m_gameFinished)
    {
        QMessageBox::StandardButton button =
            showQuestion(tr("Abort current game?"),
                         QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::Yes || button == QMessageBox::Cancel)
            return true;
        return false;
    }
    return true;
}

void MainWindow::clearFile()
{
    setFile(QString());
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

    if (! m_file.isEmpty() && m_game->get_modified())
    {
        QMessageBox::StandardButton button =
            showQuestion(tr("The file has been modified. Save changes?"),
                         QMessageBox::Save | QMessageBox::Discard
                         | QMessageBox::Cancel);
        if (button == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        if (button == QMessageBox::Save)
        {
            if (m_file.isEmpty())
                saveAs();
            else
                save();
        }
        event->accept();
    }
    cancelGenMove();
    const Board& bd = m_game->get_board();
    if (m_file.isEmpty())
    {
        QString autoSaveFile = getAutoSaveFile();
        if (bd.get_nu_moves() > 0 && ! m_gameFinished)
        {
            ofstream out(autoSaveFile.toStdString().c_str());
            write_tree(out, m_game->get_root(), true, 2);
        }
        else
        {
            QFile file(autoSaveFile);
            if (file.exists() && ! file.remove())
                showError(QString(tr("Could not delete %1")).arg(autoSaveFile));
        }
    }
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter_state", m_splitter->saveState());
    settings.setValue("toolbar", m_toolBar->isVisible());
}

void MainWindow::coordinateLabels(bool checked)
{
    m_guiBoard->setDrawCoordLabels(checked);
    QSettings settings;
    settings.setValue("coordinate_labels", checked);
}

void MainWindow::commentChanged()
{
    QString comment = m_comment->toPlainText().trimmed();
    if (comment.isEmpty())
    {
        m_game->set_comment("");
        return;
    }
    // We only recognize UTF8 and ISO-8859-1 in the SGF charset property
    const Node& root = m_game->get_root();
    QString charset = QString(root.get_property("CA", "iso-8859-1").c_str());
    charset = charset.toLower();
    if (charset == "utf-8" || charset == "utf8")
        m_game->set_comment(comment.toUtf8().data());
    else if (charset == "iso-8859-1" || charset == "latin1")
        m_game->set_comment(comment.toLatin1().data());
    else
        m_game->set_comment(comment.toAscii().data());
}

void MainWindow::computerColor()
{
    GameVariant variant = m_game->get_game_variant();
    ComputerColorDialog dialog(this, variant, m_computerColor);
    dialog.exec();
    if (variant != game_variant_classic)
    {
        bool computerNone = true;
        for (ColorIterator i(m_game->get_board().get_nu_colors()); i; ++i)
            if (m_computerColor[*i])
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
    if (! m_isGenMoveRunning)
        checkComputerMove();
}

bool MainWindow::computerPlaysAll() const
{
    for (ColorIterator i(m_game->get_board().get_nu_colors()); i; ++i)
        if (! m_computerColor[*i])
            return false;
    return true;
}

void MainWindow::createActions()
{
    QActionGroup* groupGameVariant = new QActionGroup(this);
    QActionGroup* groupLevel = new QActionGroup(this);
    QActionGroup* groupMoveNumbers = new QActionGroup(this);
    QActionGroup* groupMoveAnnotation = new QActionGroup(this);

    m_actionAbout = new QAction(tr("&About"), this);
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actionBackward = new QAction(tr("&Backward"), this);
    setIcon(m_actionBackward, "go-previous");
    m_actionBackward->setShortcut(QString("Ctrl+Left"));
    connect(m_actionBackward, SIGNAL(triggered()), this, SLOT(backward()));

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

    m_actionBeginning = new QAction(tr("Be&ginning"), this);
    setIcon(m_actionBeginning, "go-first");
    m_actionBeginning->setShortcut(QString("Ctrl+Home"));
    connect(m_actionBeginning, SIGNAL(triggered()), this, SLOT(beginning()));

    m_actionClearSelectedPiece = new QAction(tr("Clear Piece"), this);
    setIcon(m_actionClearSelectedPiece, "piece-clear");
    m_actionClearSelectedPiece->setShortcut(QString("0"));
    connect(m_actionClearSelectedPiece, SIGNAL(triggered()),
            this, SLOT(clearSelectedPiece()));

    m_actionComputerColor = new QAction(tr("&Computer Color"), this);
    connect(m_actionComputerColor, SIGNAL(triggered()),
            this, SLOT(computerColor()));

    m_actionCoordinateLabels = new QAction(tr("C&oordinate Labels"), this);
    m_actionCoordinateLabels->setCheckable(true);
    connect(m_actionCoordinateLabels, SIGNAL(triggered(bool)),
            this, SLOT(coordinateLabels(bool)));

    m_actionDoubtfulMove = new QAction(tr("&Doubtful"), this);
    m_actionDoubtfulMove->setActionGroup(groupMoveAnnotation);
    m_actionDoubtfulMove->setCheckable(true);
    connect(m_actionDoubtfulMove, SIGNAL(triggered(bool)),
            this, SLOT(doubtfulMove(bool)));

    m_actionEnd = new QAction(tr("&End"), this);
    m_actionEnd->setShortcut(QString("Ctrl+End"));
    setIcon(m_actionEnd, "go-last");
    connect(m_actionEnd, SIGNAL(triggered()), this, SLOT(end()));

    m_actionExportAsciiArt = new QAction(tr("&ASCII Art"), this);
    connect(m_actionExportAsciiArt, SIGNAL(triggered()),
            this, SLOT(exportAsciiArt()));

    m_actionExportImage = new QAction(tr("&Image"), this);
    connect(m_actionExportImage, SIGNAL(triggered()),
            this, SLOT(exportImage()));

    m_actionFindMove = new QAction(tr("&Find Move"), this);
    m_actionFindMove->setShortcut(QString("F2"));
    connect(m_actionFindMove, SIGNAL(triggered()), this, SLOT(findMove()));

    m_actionFlipPieceHorizontally = new QAction(tr("Flip Horizontally"), this);
    setIcon(m_actionFlipPieceHorizontally, "piece-flip-horizontal");
    connect(m_actionFlipPieceHorizontally, SIGNAL(triggered()),
            this, SLOT(flipPieceHorizontally()));

    m_actionFlipPieceVertically = new QAction(tr("Flip Vertically"), this);
    setIcon(m_actionFlipPieceVertically, "piece-flip-vertical");

    m_actionForward = new QAction(tr("&Forward"), this);
    m_actionForward->setShortcut(QString("Ctrl+Right"));
    setIcon(m_actionForward, "go-next");
    connect(m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));

    m_actionFullscreen = new QAction(tr("&Fullscreen"), this);
    m_actionFullscreen->setShortcut(QString("F11"));
    m_actionFullscreen->setCheckable(true);
    connect(m_actionFullscreen, SIGNAL(triggered(bool)),
            this, SLOT(fullscreen(bool)));

    m_actionGameVariantClassic = new QAction(tr("&Classic"), this);
    m_actionGameVariantClassic->setActionGroup(groupGameVariant);
    m_actionGameVariantClassic->setCheckable(true);
    connect(m_actionGameVariantClassic, SIGNAL(triggered(bool)),
            this, SLOT(gameVariantClassic(bool)));

    m_actionGameVariantClassic2 = new QAction(tr("Classic &Two-Player"), this);
    m_actionGameVariantClassic2->setActionGroup(groupGameVariant);
    m_actionGameVariantClassic2->setCheckable(true);
    connect(m_actionGameVariantClassic2, SIGNAL(triggered(bool)),
            this, SLOT(gameVariantClassic2(bool)));

    m_actionGameVariantDuo = new QAction(tr("&Duo"), this);
    m_actionGameVariantDuo->setActionGroup(groupGameVariant);
    m_actionGameVariantDuo->setCheckable(true);
    connect(m_actionGameVariantDuo, SIGNAL(triggered(bool)),
            this, SLOT(gameVariantDuo(bool)));

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

    m_actionInterestingMove = new QAction(tr("&Interesting"), this);
    m_actionInterestingMove->setActionGroup(groupMoveAnnotation);
    m_actionInterestingMove->setCheckable(true);
    connect(m_actionInterestingMove, SIGNAL(triggered(bool)),
            this, SLOT(interestingMove(bool)));

    m_actionInterrupt = new QAction(tr("&Interrupt"), this);
    m_actionInterrupt->setShortcut(QString("Escape"));
    m_actionInterrupt->setEnabled(false);
    connect(m_actionInterrupt, SIGNAL(triggered()), this, SLOT(interrupt()));

    m_actionMakeMainVariation = new QAction(tr("&Make Main Variation"), this);
    connect(m_actionMakeMainVariation, SIGNAL(triggered()),
            this, SLOT(makeMainVariation()));

    QString levelText[maxLevel] =
        {
            tr("&1 (fast)"),
            tr("&2"),
            tr("&3"),
            tr("&4"),
            tr("&5"),
            tr("&6 (slow)")
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
    setIcon(m_actionNextPiece, "next-piece");
    m_actionNextPiece->setShortcut(QString("+"));
    connect(m_actionNextPiece, SIGNAL(triggered()), this, SLOT(nextPiece()));

    m_actionNextTransform = new QAction("", this);
    m_actionNextTransform->setShortcut(QString("Space"));
    connect(m_actionNextTransform, SIGNAL(triggered()),
            this, SLOT(nextTransform()));

    m_actionNextVariation = new QAction(tr("&Next Variation"), this);
    m_actionNextVariation->setShortcut(QString("Ctrl+Down"));
    setIcon(m_actionNextVariation, "go-down");
    connect(m_actionNextVariation, SIGNAL(triggered()),
            this, SLOT(nextVariation()));

    m_actionNewGame = new QAction(tr("&New Game"), this);
    m_actionNewGame->setShortcut(QKeySequence::New);
    setIcon(m_actionNewGame, "newgame");
    connect(m_actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));

    m_actionNoMoveAnnotation =
        new QAction(tr("&None", "move annotation"), this);
    m_actionNoMoveAnnotation->setActionGroup(groupMoveAnnotation);
    m_actionNoMoveAnnotation->setCheckable(true);
    connect(m_actionNoMoveAnnotation, SIGNAL(triggered(bool)),
            this, SLOT(noMoveAnnotation(bool)));

    m_actionOpen = new QAction(tr("&Open..."), this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    setIcon(m_actionOpen, "document-open");
    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    m_actionPlaceSelectedPiece = new QAction("", this);
    m_actionPlaceSelectedPiece->setShortcut(QString("Return"));

    m_actionPlay = new QAction(tr("&Play"), this);
    m_actionPlay->setShortcut(QString("Ctrl+L"));
    setIcon(m_actionPlay, "play");
    connect(m_actionPlay, SIGNAL(triggered()), this, SLOT(play()));

    m_actionPreviousPiece = new QAction(tr("Previous Piece"), this);
    setIcon(m_actionPreviousPiece, "previous-piece");
    m_actionPreviousPiece->setShortcut(QString("-"));
    connect(m_actionPreviousPiece, SIGNAL(triggered()),
            this, SLOT(previousPiece()));

    m_actionPreviousTransform = new QAction("", this);
    m_actionPreviousTransform->setShortcut(QString("Shift+Space"));
    connect(m_actionPreviousTransform, SIGNAL(triggered()),
            this, SLOT(previousTransform()));

    m_actionPreviousVariation = new QAction(tr("&Previous Variation"), this);
    m_actionPreviousVariation->setShortcut(QString("Ctrl+Up"));
    setIcon(m_actionPreviousVariation, "go-up");
    connect(m_actionPreviousVariation, SIGNAL(triggered()),
            this, SLOT(previousVariation()));

    for (int i = 0; i < maxRecentFiles; ++i)
    {
         m_actionRecentFile[i] = new QAction(this);
         m_actionRecentFile[i]->setVisible(false);
         connect(m_actionRecentFile[i], SIGNAL(triggered()),
                 this, SLOT(openRecentFile()));
     }

    m_actionRotatePieceAnticlockwise = new QAction(tr("Rotate Anticlockwise"),
                                                   this);
    setIcon(m_actionRotatePieceAnticlockwise, "piece-rotate-left");
    connect(m_actionRotatePieceAnticlockwise, SIGNAL(triggered()),
            this, SLOT(rotatePieceAnticlockwise()));

    m_actionRotatePieceClockwise = new QAction(tr("Rotate Clockwise"), this);
    setIcon(m_actionRotatePieceClockwise, "piece-rotate-right");
    connect(m_actionRotatePieceClockwise, SIGNAL(triggered()),
            this, SLOT(rotatePieceClockwise()));

    m_actionQuit = new QAction(tr("&Quit"), this);
    m_actionQuit->setShortcut(QKeySequence::Quit);
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(quit()));

    m_actionSave = new QAction(tr("&Save"), this);
    m_actionSave->setShortcut(QKeySequence::Save);
    setIcon(m_actionSave, "document-save");
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(save()));

    m_actionSaveAs = new QAction(tr("Save &As..."), this);
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_actionSelectPiece1 = new QAction(this);
    m_actionSelectPiece1->setShortcut(QString("1"));
    connect(m_actionSelectPiece1, SIGNAL(triggered()),
            this, SLOT(selectPiece1()));

    m_actionSelectPiece2 = new QAction(this);
    m_actionSelectPiece2->setShortcut(QString("2"));
    connect(m_actionSelectPiece2, SIGNAL(triggered()),
            this, SLOT(selectPiece2()));

    m_actionSelectPieceF = new QAction(this);
    m_actionSelectPieceF->setShortcut(QString("F"));
    connect(m_actionSelectPieceF, SIGNAL(triggered()),
            this, SLOT(selectPieceF()));

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

    m_actionShowComment = new QAction(tr("&Comment"), this);
    m_actionShowComment->setCheckable(true);
    connect(m_actionShowComment, SIGNAL(triggered(bool)),
            this, SLOT(showComment(bool)));

    m_actionCustomLevel = new QAction(tr("&Custom..."), this);
    m_actionCustomLevel->setCheckable(true);
    if (m_useTimeLimit)
        m_actionCustomLevel->setChecked(true);
    m_actionCustomLevel->setActionGroup(groupLevel);
    connect(m_actionCustomLevel, SIGNAL(triggered(bool)),
            this, SLOT(customLevel(bool)));

    m_actionTruncate = new QAction(tr("&Truncate"), this);
    connect(m_actionTruncate, SIGNAL(triggered()), this, SLOT(truncate()));

    m_actionUseBook = new QAction(tr("Use &Opening Book"), this);
    m_actionUseBook->setCheckable(true);
    m_actionUseBook->setChecked(m_useBook);
    connect(m_actionUseBook, SIGNAL(triggered(bool)),
            this, SLOT(useBook(bool)));

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
    layout->addWidget(createLeftPanel(), 5);
    layout->addWidget(createRightPanel(), 5);
    // The central widget doesn't do anything with the focus right now, but we
    // allow it to receive the focus such that the user can switch away the
    // focus from the comment field and its blinking cursor.
    widget->setFocusPolicy(Qt::StrongFocus);
    return widget;
}

QWidget* MainWindow::createLeftPanel()
{
    m_splitter = new QSplitter(Qt::Vertical);
    m_guiBoard = new GuiBoard(0, m_game->get_board());
    m_splitter->addWidget(m_guiBoard);
    m_comment = new QPlainTextEdit();
    m_comment->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_comment->setTabChangesFocus(true);
    connect(m_comment, SIGNAL(textChanged()), this, SLOT(commentChanged()));
    m_splitter->addWidget(m_comment);
    m_splitter->setStretchFactor(0, 85);
    m_splitter->setStretchFactor(1, 15);
    m_splitter->setCollapsible(0, false);
    connect(m_splitter, SIGNAL(splitterMoved(int, int)),
            this, SLOT(splitterMoved(int, int)));
    return m_splitter;
}

QAction* MainWindow::createLevelAction(QActionGroup* group, int level,
                                       const QString& text)
{
    LIBBOARDGAME_ASSERT(level >= 1 && level <= maxLevel);
    QAction* action = new QAction(text, this);
    action->setCheckable(true);
    if (level == m_level && ! m_useTimeLimit)
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
    m_menuOpenRecent = menuFile->addMenu(tr("&Open Recent"));
    for (int i = 0; i < maxRecentFiles; ++i)
        m_menuOpenRecent->addAction(m_actionRecentFile[i]);
    menuFile->addAction(m_actionSave);
    menuFile->addAction(m_actionSaveAs);
    QMenu* menuExport = menuFile->addMenu(tr("&Export"));
    menuExport->addAction(m_actionExportImage);
    menuExport->addAction(m_actionExportAsciiArt);
    menuFile->addAction(m_actionQuit);

    QMenu* menuGame = menuBar()->addMenu(tr("G&ame"));
    menuGame->addAction(m_actionNewGame);
    QMenu* menuGameVariant = menuGame->addMenu(tr("Game &Variant"));
    menuGameVariant->addAction(m_actionGameVariantClassic);
    menuGameVariant->addAction(m_actionGameVariantClassic2);
    menuGameVariant->addAction(m_actionGameVariantDuo);
    menuGame->addAction(m_actionComputerColor);
    menuGame->addAction(m_actionFindMove);

    QMenu* menuGo = menuBar()->addMenu(tr("&Go"));
    menuGo->addAction(m_actionBeginning);
    menuGo->addAction(m_actionBackward);
    menuGo->addAction(m_actionForward);
    menuGo->addAction(m_actionEnd);
    menuGo->addAction(m_actionNextVariation);
    menuGo->addAction(m_actionPreviousVariation);
    menuGo->addAction(m_actionGotoMove);
    menuGo->addAction(m_actionBackToMainVariation);

    QMenu* menuEdit = menuBar()->addMenu(tr("&Edit"));
    m_menuMoveAnnotation = menuEdit->addMenu(tr("Move &Annotation"));
    m_menuMoveAnnotation->addAction(m_actionGoodMove);
    m_menuMoveAnnotation->addAction(m_actionVeryGoodMove);
    m_menuMoveAnnotation->addAction(m_actionBadMove);
    m_menuMoveAnnotation->addAction(m_actionVeryBadMove);
    m_menuMoveAnnotation->addAction(m_actionInterestingMove);
    m_menuMoveAnnotation->addAction(m_actionDoubtfulMove);
    m_menuMoveAnnotation->addAction(m_actionNoMoveAnnotation);
    menuEdit->addAction(m_actionMakeMainVariation);
    menuEdit->addAction(m_actionTruncate);

    QMenu* menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(m_actionShowToolbar);
    menuView->addAction(m_actionShowComment);
    menuView->addAction(m_actionFullscreen);
    QMenu* menuMoveNumbers = menuView->addMenu(tr("Move &Numbers"));
    menuMoveNumbers->addAction(m_actionMoveNumbersLast);
    menuMoveNumbers->addAction(m_actionMoveNumbersAll);
    menuMoveNumbers->addAction(m_actionMoveNumbersNone);
    menuView->addAction(m_actionCoordinateLabels);

    QMenu* menuComputer = menuBar()->addMenu(tr("&Computer"));
    menuComputer->addAction(m_actionPlay);
    menuComputer->addAction(m_actionInterrupt);
    menuComputer->addAction(m_actionUseBook);
    QMenu* menuLevel = menuComputer->addMenu(tr("&Level"));
    for (int i = 0; i < maxLevel; ++i)
        menuLevel->addAction(m_actionLevel[i]);
    menuLevel->addAction(m_actionCustomLevel);

    QMenu* menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(m_actionHelp);
    menuHelp->addAction(m_actionAbout);
}

QWidget* MainWindow::createOrientationButtonBoxLeft()
{
    QWidget* outerWidget = new QWidget();
    QVBoxLayout* outerLayout = new QVBoxLayout();
    outerWidget->setLayout(outerLayout);
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(QMargins());
    widget->setLayout(layout);
    layout->addWidget(createOBoxToolButton(m_actionRotatePieceAnticlockwise),
                      0, 0);
    layout->addWidget(createOBoxToolButton(m_actionRotatePieceClockwise),
                      0, 1);
    layout->addWidget(createOBoxToolButton(m_actionFlipPieceHorizontally),
                      1, 0);
    layout->addWidget(createOBoxToolButton(m_actionFlipPieceVertically),
                      1, 1);
    outerLayout->addStretch();
    outerLayout->addWidget(widget);
    outerLayout->addStretch();
    return outerWidget;
}

QWidget* MainWindow::createOrientationButtonBoxRight()
{
    QWidget* outerWidget = new QWidget();
    QVBoxLayout* outerLayout = new QVBoxLayout();
    outerWidget->setLayout(outerLayout);
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(QMargins());
    widget->setLayout(layout);
    layout->addWidget(createOBoxToolButton(m_actionPreviousPiece),
                      0, 0);
    layout->addWidget(createOBoxToolButton(m_actionNextPiece),
                      0, 1);
    layout->addWidget(createOBoxToolButton(m_actionClearSelectedPiece),
                      1, 0, 1, 2, Qt::AlignHCenter);
    outerLayout->addStretch();
    outerLayout->addWidget(widget);
    outerLayout->addStretch();
    return outerWidget;
}

QWidget* MainWindow::createOrientationSelector()
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    widget->setLayout(layout);
    layout->addWidget(createOrientationButtonBoxLeft(), 0);
    QFrame* frame = new QFrame();
    frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    layout->addWidget(frame, 1);
    QHBoxLayout* frameLayout = new QHBoxLayout();
    frame->setLayout(frameLayout);
    m_orientationDisplay = new OrientationDisplay(0, m_game->get_board());
    frameLayout->addWidget(m_orientationDisplay);
    layout->addWidget(createOrientationButtonBoxRight(), 0);
    return widget;
}

QWidget* MainWindow::createRightPanel()
{
    QWidget* widget = new QWidget();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    widget->setLayout(layout);
    layout->addWidget(createOrientationSelector(), 30);
    m_scoreDisplay = new ScoreDisplay();
    layout->addWidget(m_scoreDisplay, 1);
    QWidget* pieceSelectorBox = new QWidget();
    layout->addWidget(pieceSelectorBox, 70);
    SameHeightLayout* pieceSelectorLayout = new SameHeightLayout();
    pieceSelectorBox->setLayout(pieceSelectorLayout);
    for (ColorIterator i(Color::range); i; ++i)
    {
        m_pieceSelector[*i] = new PieceSelector(0, m_game->get_board(), *i);
        connect(m_pieceSelector[*i],
                SIGNAL(pieceSelected(Color, const Piece&, Transform)),
                this, SLOT(selectPiece(Color, const Piece&, Transform)));
        pieceSelectorLayout->addWidget(m_pieceSelector[*i]);
    }
    initPieceSelectors();
    return widget;
}

void MainWindow::doubtfulMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_doubtful_move();
    boardChanged(false);
}

void MainWindow::createToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_toolBar->addAction(m_actionOpen);
    m_toolBar->addAction(m_actionSave);
    m_toolBar->addAction(m_actionNewGame);
    m_toolBar->addAction(m_actionPlay);
    m_toolBar->addAction(m_actionBeginning);
    m_toolBar->addAction(m_actionBackward);
    m_toolBar->addAction(m_actionForward);
    m_toolBar->addAction(m_actionEnd);
    m_toolBar->addAction(m_actionNextVariation);
    m_toolBar->addAction(m_actionPreviousVariation);
    addToolBar(m_toolBar);
    m_actionShowToolbar = m_toolBar->toggleViewAction();
    m_actionShowToolbar->setText(tr("&Toolbar"));
}

void MainWindow::customLevel(bool checked)
{
    if (! checked)
        return;
    bool ok;
    m_timeLimit = QInputDialog::getInt(this, tr("Pentobi"),
                                       tr("Maximum time per move in seconds:"),
                                       m_timeLimit, 1, 3600, 1, &ok);
    if (! ok)
    {
        if (! m_useTimeLimit)
            setLevel(m_level);
        return;
    }
    m_useTimeLimit = true;
    QSettings settings;
    settings.setValue("time_limit", m_timeLimit);
    settings.setValue("use_time_limit", m_useTimeLimit);
}

void MainWindow::end()
{
    gotoNode(get_last_node(m_game->get_current()));
}

void MainWindow::exportAsciiArt()
{
    QString file = QFileDialog::getSaveFileName(this, QString(), QString(),
                                                tr("Text files (*.txt)"));
    if (file.isEmpty())
        return;
    ofstream out(file.toStdString().c_str());
    const Board& bd = m_game->get_board();
    out << bd;
    if (! out)
        showError(strerror(errno));
}

void MainWindow::exportImage()
{
    QSettings settings;
    int size = settings.value("export_image_size", 320).toInt();
    bool ok;
    size = QInputDialog::getInt(this, tr("Pentobi - Export Image"),
                                tr("Image size:"), size, 0, 2147483647, 40,
                                &ok);
    if (! ok)
        return;
    settings.setValue("export_image_size", size);
    bool coordinateLabels = m_actionCoordinateLabels->isChecked();
    BoardPainter boardPainter;
    boardPainter.setDrawCoordLabels(coordinateLabels);
    boardPainter.setCoordLabelColor(QColor(100, 100, 100));
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    if (coordinateLabels)
        painter.fillRect(0, 0, size, size, QColor(216, 216, 216));
    const Board& bd = m_game->get_board();
    boardPainter.paint(painter, size, size, bd.get_game_variant(),
                       bd.get_grid(), &m_guiBoard->getLabels());
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
    const Board& bd = m_game->get_board();
    if (bd.is_game_over())
        return;
    if (m_legalMoves.empty())
        bd.gen_moves(m_toPlay, m_legalMoves);
    if (m_legalMoves.empty())
    {
        // m_toPlay must have moves if game is not over
        LIBBOARDGAME_ASSERT(false);
        return;
    }
    if (m_legalMoveIndex >= m_legalMoves.size())
        m_legalMoveIndex = 0;
    Move mv = m_legalMoves[m_legalMoveIndex];
    selectPiece(m_toPlay, bd.get_piece(bd.get_move_info(mv).piece));
    m_guiBoard->showMove(m_toPlay, mv);
    ++m_legalMoveIndex;
}

void MainWindow::flipPieceHorizontally()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = transform.get_mirrored_horizontally();
    transform = piece->get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::flipPieceVertically()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = transform.get_mirrored_vertically();
    transform = piece->get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
}

void MainWindow::forward()
{
    const Node* node = m_game->get_current().get_first_child();
    if (node == 0)
        return;
    gotoNode(*node);
}

void MainWindow::fullscreen(bool checked)
{
    if (checked)
        showFullScreen();
    else
        showNormal();
}

void MainWindow::gameVariantClassic(bool checked)
{
    if (checked)
        setGameVariant(game_variant_classic);
}

void MainWindow::gameVariantClassic2(bool checked)
{
    if (checked)
        setGameVariant(game_variant_classic_2);
}

void MainWindow::gameVariantDuo(bool checked)
{
    if (checked)
        setGameVariant(game_variant_duo);
}

void MainWindow::genMove()
{
    ++m_genMoveId;
    showStatus(tr("The computer is thinking..."));
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_actionInterrupt->setEnabled(true);
    clearSelectedPiece();
    clear_abort();
    m_player->set_use_book(m_useBook);
    if (m_useTimeLimit)
        m_player->set_fixed_time(m_timeLimit);
    else
        m_player->set_level(m_level);
    QFuture<GenMoveResult> future =
        QtConcurrent::run(this, &MainWindow::asyncGenMove, m_toPlay,
                          m_genMoveId);
    m_genMoveWatcher.setFuture(future);
    m_isGenMoveRunning = true;
}

void MainWindow::genMoveFinished()
{
    GenMoveResult result = m_genMoveWatcher.future().result();
    if (result.genMoveId != m_genMoveId)
        // Callback from a move generation canceled with cancelGenMove()
        return;
    LIBBOARDGAME_ASSERT(m_isGenMoveRunning);
    m_isGenMoveRunning = false;
    QApplication::restoreOverrideCursor();
    m_actionInterrupt->setEnabled(false);
    clearStatus();
    if (get_abort() && computerPlaysAll())
        m_computerColor.fill(false);
    Color c = result.color;
    Move mv = result.move;
    if (mv.is_null())
    {
        showError(tr("Player failed to generate a move."));
        return;
    }
    const Board& bd = m_game->get_board();
    if (! bd.is_legal(c, mv))
    {
        showError(QString(tr("Player generated illegal move: %1"))
                  .arg(bd.to_string(mv).c_str()));
        return;
    }
    if (mv.is_pass())
    {
        showStatus(tr("The computer has no more moves available."));
        boardChanged(false);
        checkComputerMove();
        return;
    }
    play(c, mv);
}

QString MainWindow::getFilter() const
{
    return tr("Blokus games (*.blksgf);;All files (*.*)");
}

QString MainWindow::getVersion() const
{
    QString version;
#ifdef VERSION
    version = VERSION;
#endif
    if (version.isEmpty())
        version = "UNKNOWN";
    // By convention, the version string of unreleased versions contains the
    // string UNKNOWN (appended to the last released version). In this case, or
    // if VERSION was undefined, we append the build date.
    if (version.contains("UNKNOWN"))
    {
        version.append(" (");
        version.append(__DATE__);
        version.append(")");
    }
#if LIBBOARDGAME_DEBUG
    version.append(" (dbg)");
#endif
    return version;
}

void MainWindow::goodMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_good_move();
    boardChanged(false);
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
    node = m_game->get_current().get_first_child();
    while (node != 0)
    {
        if (! tree.get_move(*node).is_null())
            nodes.push_back(node);
        node = node->get_first_child();
    }
    int maxMoves = int(nodes.size());
    if (maxMoves == 0)
        return;
    int defaultValue = m_game->get_board().get_nu_moves();
    if (defaultValue == 0)
        defaultValue = maxMoves;
    bool ok;
    int i = QInputDialog::getInt(this, tr("Pentobi"), tr("Move number:"),
                                 defaultValue, 1, nodes.size(), 1, &ok);
    if (! ok)
        return;
    gotoNode(*nodes[i - 1]);
}

void MainWindow::gotoNode(const Node& node)
{
    cancelGenMove();
    try
    {
        m_game->goto_node(node);
    }
    catch (const InvalidPropertyValue& e)
    {
        showError(tr("Game file node contains invalid property"), e.what());
    }
    m_noMovesAvailableShown.fill(false);
    m_lastMoveByComputer = false;
    boardChanged(true);
}

void MainWindow::help()
{
    if (m_help_window != 0)
    {
        m_help_window->show();
        m_help_window->raise();
        return;
    }
    QString path = HelpWindow::findMainPage(":/pentobi/manual", "index.html",
                                            QLocale::system().name());
    m_help_window = new HelpWindow(this, path);
    m_help_window->show();
}

void MainWindow::humanPlay(Color c, Move mv)
{
    cancelGenMove();
    if (m_computerColor[c])
        // If the user enters a move previously played by the computer (e.g.
        // after undoing moves) then it is unlikely that the user wants to keep
        // the computer color settings.
        m_computerColor.fill(false);
    play(c, mv);
}

void MainWindow::initGame()
{
    m_game->init();
    m_game->set_charset("UTF-8");
    m_game->set_application("Pentobi", getVersion().toStdString());
    m_game->clear_modified();
    m_computerColor.fill(false);
    QSettings settings;
    if (! settings.value("computer_color_none").toBool())
    {
        if (m_game->get_game_variant() == game_variant_duo)
            m_computerColor[Color(1)] = true;
        else if (m_game->get_game_variant() == game_variant_classic_2)
        {
            m_computerColor[Color(1)] = true;
            m_computerColor[Color(3)] = true;
        }
    }
    m_noMovesAvailableShown.fill(false);
    m_lastMoveByComputer = false;
    m_gameFinished = false;
    setFile(QString());
}

void MainWindow::initGameVariantActions()
{
    GameVariant variant = m_game->get_game_variant();
    if (variant == game_variant_classic)
        m_actionGameVariantClassic->setChecked(true);
    else if (variant == game_variant_classic_2)
        m_actionGameVariantClassic2->setChecked(true);
    else
        m_actionGameVariantDuo->setChecked(true);
}

void MainWindow::initPieceSelectors()
{
    const Board& bd = m_game->get_board();
    for (unsigned int i = 0; i < Color::range; ++i)
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
    boardChanged(false);
}

void MainWindow::interrupt()
{
    showStatus(tr("Cancelling move generation..."));
    set_abort();
}

void MainWindow::makeMainVariation()
{
    m_game->make_main_variation();
    boardChanged(false);
}

void MainWindow::nextPiece()
{
    const Board& bd = m_game->get_board();
    if (bd.is_game_over())
        return;
    const ArrayList<unsigned int, Board::nu_pieces>& piecesLeft =
        bd.get_pieces_left(m_toPlay);
    unsigned int nuPiecesLeft = piecesLeft.size();
    if (nuPiecesLeft == 0)
        return;
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        piece = &bd.get_piece(piecesLeft[0]);
    else
    {
        for (unsigned int i = 0; i< nuPiecesLeft; ++i)
            if (&bd.get_piece(piecesLeft[i]) == piece)
            {
                if (i + 1 >= nuPiecesLeft)
                    piece = &bd.get_piece(piecesLeft[0]);
                else
                    piece = &bd.get_piece(piecesLeft[i + 1]);
                break;
            }
    }
    selectPiece(m_toPlay, *piece);
}

void MainWindow::nextTransform()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = piece->get_next_transform(transform);
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

void MainWindow::newGame()
{
    if (! checkSave())
        return;
    cancelGenMove();
    initGame();
    boardChanged(true);
}

void MainWindow::noMoveAnnotation(bool checked)
{
    if (! checked)
        return;
    m_game->remove_move_annotation();
    boardChanged(false);
}

void MainWindow::open()
{
    if (! checkSave())
        return;
    QSettings settings;
    QString dir = settings.value("last_dir", QDir::home().path()).toString();
    open(QFileDialog::getOpenFileName(this, tr("Open"), dir, getFilter()));
}

void MainWindow::open(const QString& file, bool isTemporary)
{
    if (file.isEmpty())
        return;
    cancelGenMove();
    TreeReader reader;
    try
    {
        reader.read(file.toStdString());
    }
    catch (const TreeReader::ReadError& e)
    {
        showError(tr("Read error"), e.what());
        return;
    }
    if (! isTemporary)
    {
        setFile(file);
        QSettings settings;
        settings.setValue("last_dir", QFileInfo(m_file).dir().path());
        QFile autoSaveFile(getAutoSaveFile());
        if (autoSaveFile.exists())
            autoSaveFile.remove();
    }
    try
    {
        unique_ptr<Node> tree = reader.get_tree_transfer_ownership();
        m_game->init(tree);
        m_game->goto_node(get_last_node(m_game->get_root()));
        initPieceSelectors();
    }
    catch (const InvalidPropertyValue& e)
    {
        showError(tr("File contains invalid SGF properties"), e.what());
    }
    m_noMovesAvailableShown.fill(false);
    m_computerColor.fill(false);
    m_lastMoveByComputer = false;
    initGameVariantActions();
    boardChanged(true);
}

void MainWindow::openRecentFile()
{
     QAction* action = qobject_cast<QAction*>(sender());
     if (action)
         open(action->data().toString());
}

void MainWindow::play()
{
    cancelGenMove();
    GameVariant variant = m_game->get_game_variant();
    if (variant != game_variant_classic)
    {
        QSettings settings;
        settings.setValue("computer_color_none", false);
    }
    if (! m_computerColor[m_toPlay])
    {
        m_computerColor.fill(false);
        m_computerColor[m_toPlay] = true;
        if (variant == game_variant_classic_2)
        {
            if (m_toPlay == Color(0) || m_toPlay == Color(2))
                m_computerColor[Color(0)] = m_computerColor[Color(2)] = true;
            else
                m_computerColor[Color(1)] = m_computerColor[Color(3)] = true;
        }
        else
            m_computerColor[m_toPlay] = true;
    }
    genMove();
}

void MainWindow::play(Color c, Move mv)
{
    const Board& bd = m_game->get_board();
    m_lastMoveByComputer = m_computerColor[c];
    m_game->play(c, mv, false);
    c = m_game->get_to_play();
    m_gameFinished = false;
    if (bd.is_game_over())
    {
        boardChanged(true);
        repaint();
        m_gameFinished = true;
        showGameOver();
    }
    else if (! bd.has_moves(c))
    {
        boardChanged(true);
        repaint();
        if (! computerPlaysAll() && ! m_noMovesAvailableShown[c])
        {
            showNoMovesAvailable(c);
            m_noMovesAvailableShown[c] = true;
        }
        checkComputerMove();
    }
    else
    {
        boardChanged(true);
        checkComputerMove();
    }
}

void MainWindow::previousPiece()
{
    const Board& bd = m_game->get_board();
    if (bd.is_game_over())
        return;
    Color c = m_game->get_effective_to_play();
    const ArrayList<unsigned int, Board::nu_pieces>& piecesLeft =
        bd.get_pieces_left(c);
    unsigned int nuPiecesLeft = piecesLeft.size();
    if (nuPiecesLeft == 0)
        return;
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        piece = &bd.get_piece(piecesLeft[nuPiecesLeft - 1]);
    else
    {
        for (unsigned int i = 0; i< nuPiecesLeft; ++i)
            if (&bd.get_piece(piecesLeft[i]) == piece)
            {
                if (i == 0)
                    piece = &bd.get_piece(piecesLeft[nuPiecesLeft - 1]);
                else
                    piece = &bd.get_piece(piecesLeft[i - 1]);
                break;
            }
    }
    selectPiece(c, *piece);
}

void MainWindow::previousTransform()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = piece->get_previous_transform(transform);
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

void MainWindow::quit()
{
    cancelGenMove();
    qApp->quit();
}

void MainWindow::rotatePieceAnticlockwise()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = transform.get_rotated_anticlockwise();
    transform = piece->get_equivalent_transform(transform);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    updateFlipActions();
}

void MainWindow::rotatePieceClockwise()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    transform = transform.get_rotated_clockwise();
    transform = piece->get_equivalent_transform(transform);
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
    ofstream out(m_file.toStdString().c_str());
    write_tree(out, m_game->get_root(), true, 2);
    if (! out)
        showError(tr("The file could not be saved."),
                  QString(
                          /*: Error message if file cannot be saved. %1 is
                            replaced by the file name, %2 by the error message
                            of the operating system. */
                          tr("%1: %2")
                          ).arg(m_file).arg(strerror(errno)));
    else
    {
        showStatus(QString(tr("File saved %1")).arg(m_file), true);
        m_game->clear_modified();
        boardChanged(false);
    }
}

void MainWindow::saveAs()
{
    QString file = m_file;
    if (file.isEmpty())
    {
        file = QDir::home().path();
        file.append(QDir::separator());
        file.append(tr("Unknown.blksgf"));
        if (QFileInfo(file).exists())
            for (unsigned int i = 1; ; ++i)
            {
                file = QDir::home().path();
                file.append(QDir::separator());
                file.append(QString(tr("Unknown-%1.blksgf")).arg(i));
                if (! QFileInfo(file).exists())
                    break;
            }
    }
    file = QFileDialog::getSaveFileName(this, tr("Save"), file, getFilter());
    if (! file.isEmpty())
    {
        setFile(file);
        save();
    }
}

void MainWindow::selectNamedPiece(const char* name1, const char* name2,
                                  const char* name3)
{
    vector<const Piece*> pieces;
    Color c = m_game->get_effective_to_play();
    const Board& bd = m_game->get_board();
    const Piece* piece;
    if (! bd.get_piece_by_name(name1, piece))
        LIBBOARDGAME_ASSERT(false);
    else
    {
        if (bd.is_piece_left(c, *piece))
            pieces.push_back(piece);
    }
    if (name2 != 0)
    {
        if (! bd.get_piece_by_name(name2, piece))
            LIBBOARDGAME_ASSERT(false);
        else
        {
            if (bd.is_piece_left(c, *piece))
                pieces.push_back(piece);
        }
    }
    if (name3 != 0)
    {
        if (! bd.get_piece_by_name(name3, piece))
            LIBBOARDGAME_ASSERT(false);
        else
        {
            if (bd.is_piece_left(c, *piece))
                pieces.push_back(piece);
        }
    }
    if (pieces.empty())
        return;
    piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
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
    selectPiece(c, *piece);
}

void MainWindow::selectPiece(Color c, const Piece& piece)
{
    selectPiece(c, piece, Transform());
}

void MainWindow::selectPiece(Color c, const Piece& piece, Transform transform)
{
    m_guiBoard->selectPiece(c, piece);
    m_guiBoard->setSelectedPieceTransform(transform);
    m_orientationDisplay->selectColor(c);
    m_orientationDisplay->setSelectedPiece(piece);
    m_orientationDisplay->setSelectedPieceTransform(transform);
    bool can_rotate = piece.can_rotate();
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

void MainWindow::selectPieceF()
{
    selectNamedPiece("F");
}

void MainWindow::selectPieceI()
{
    selectNamedPiece("I5", "I4", "I3");
}

void MainWindow::selectPieceL()
{
    selectNamedPiece("L5", "L4");
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
    selectNamedPiece("P");
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
    selectNamedPiece("V5", "V3");
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

void MainWindow::setGameVariant(GameVariant gameVariant)
{
    if (m_game->get_game_variant() == gameVariant)
        return;
    if (! checkSave())
    {
        initGameVariantActions();
        return;
    }
    cancelGenMove();
    QSettings settings;
    switch (gameVariant)
    {
    case game_variant_classic:
        settings.setValue("game_variant", "classic");
        break;
    case game_variant_classic_2:
        settings.setValue("game_variant", "classic_2");
        break;
    case game_variant_duo:
        settings.setValue("game_variant", "duo");
        break;
    }
    clearSelectedPiece();
    m_game->init(gameVariant);
    initPieceSelectors();
    newGame();
}

void MainWindow::setFile(const QString& file)
{
    m_file = file;
    if (m_file.isEmpty())
        setWindowTitle(tr("Pentobi"));
    else
    {
        setWindowTitle(QString(tr("%1[*] - Pentobi"))
                       .arg(QFileInfo(file).fileName()));
        QSettings settings;
        QStringList files = settings.value("recent_files").toStringList();
        files.removeAll(file);
        files.prepend(file);
        while (files.size() > maxRecentFiles)
            files.removeLast();
        settings.setValue("recent_files", files);
        updateRecentFiles();
    }
}

void MainWindow::setLevel(int level)
{
    if (level <= 0 || level > maxLevel)
        return;
    m_useTimeLimit = false;
    m_level = level;
    m_actionLevel[level - 1]->setChecked(true);
    QSettings settings;
    settings.setValue("level", m_level);
    settings.setValue("use_time_limit", m_useTimeLimit);
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
        boardChanged(false);
    }
}

void MainWindow::setMoveNumbersLast(bool checked)
{
    if (checked)
    {
        QSettings settings;
        settings.setValue("move_numbers", "last");
        boardChanged(false);
    }
}

void MainWindow::setMoveNumbersNone(bool checked)
{
    if (checked)
    {
        QSettings settings;
        settings.setValue("move_numbers", "none");
        boardChanged(false);
    }
}

void MainWindow::showComment(bool checked)
{
    int height = m_splitter->height();
    if (checked)
    {
        if (m_comment->height() > 0)
            return;
        QList<int> sizes;
        sizes.push_back(0.85 * height);
        sizes.push_back(0.15 * height);
        m_splitter->setSizes(sizes);
    }
    else
    {
        QList<int> sizes;
        sizes.push_back(height);
        sizes.push_back(0);
        m_splitter->setSizes(sizes);
    }
}

void MainWindow::showError(const QString& text, const QString& infoText)
{
    showMessage(QMessageBox::Critical, text, infoText);
}

void MainWindow::showGameOver()
{
    GameVariant variant = m_game->get_game_variant();
    const Board& bd = m_game->get_board();
    QString info;
    if (variant == game_variant_duo)
    {
        double game_result;
        int score = bd.get_score(Color(0), game_result);
        if (score == 1)
            info = tr("Blue wins with 1 point.");
        else if (score > 0)
            info = QString(tr("Blue wins with %1 points.")).arg(score);
        else if (score == -1)
            info = tr("Green wins with 1 point.");
        else if (score < 0)
            info = QString(tr("Green wins with %1 points.")).arg(-score);
        else
            info = tr("The game ends in a draw.");
    }
    else if (variant == game_variant_classic_2)
    {
        double game_result;
        int score = bd.get_score(Color(0), game_result);
        if (score == 1)
            info = tr("Blue/Red wins with 1 point.");
        else if (score > 0)
            info = QString(tr("Blue/Red wins with %1 points.")).arg(score);
        else if (score == 1)
            info = tr("Yellow/Green wins with 1 point.");
        else if (score < 0)
            info = QString(tr("Yellow/Green wins with %1 points.")).arg(-score);
        else
            info = tr("The game ends in a draw.");
    }
    else
    {
        unsigned int blue = bd.get_points_with_bonus(Color(0));
        unsigned int yellow = bd.get_points_with_bonus(Color(1));
        unsigned int red = bd.get_points_with_bonus(Color(2));
        unsigned int green = bd.get_points_with_bonus(Color(3));
        unsigned int maxPoints = max(blue, max(yellow, max(red, green)));
        if (blue == yellow && yellow == red && red == green)
            info = tr("The game ends in a draw between all colors.");
        else if (blue == maxPoints && blue == yellow && yellow == red)
            info = tr("The game ends in a draw between Blue, Yellow and Red.");
        else if (blue == maxPoints && blue == yellow && yellow == green)
            info =
                tr("The game ends in a draw between Blue, Yellow and Green.");
        else if (blue == maxPoints && blue == red && red == green)
            info = tr("The game ends in a draw between Blue, Red and Green.");
        else if (yellow == maxPoints && yellow == red && red == green)
            info = tr("The game ends in a draw between Yellow, Red and Green.");
        else if (blue == maxPoints && blue == yellow)
            info = tr("The game ends in a draw between Blue and Yellow.");
        else if (blue == maxPoints && blue == red)
            info = tr("The game ends in a draw between Blue and Red.");
        else if (blue == maxPoints && blue == green)
            info = tr("The game ends in a draw between Blue and Green.");
        else if (yellow == maxPoints && yellow == red)
            info = tr("The game ends in a draw between Yellow and Red.");
        else if (yellow == maxPoints && yellow == green)
            info = tr("The game ends in a draw between Yellow and Green.");
        else if (red == maxPoints && red == green)
            info = tr("The game ends in a draw between Red and Green.");
        else if (blue == maxPoints)
            info = tr("Blue wins.");
        else if (yellow == maxPoints)
            info = tr("Yellow wins.");
        else if (red == maxPoints)
            info = tr("Red wins.");
        else
            info = tr("Green wins.");
    }
    showInfo(tr("The game is over."), info);
}

void MainWindow::showInfo(const QString& text, const QString& infoText)
{
    showMessage(QMessageBox::NoIcon, text, infoText);
}

void MainWindow::showMessage(QMessageBox::Icon icon, const QString& text,
                             const QString& infoText)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Pentobi"));
    msgBox.setIcon(icon);
    msgBox.setText(text);
    msgBox.setInformativeText(infoText);
    msgBox.exec();
}

void MainWindow::showNoMovesAvailable(Color c)
{
    GameVariant variant = m_game->get_game_variant();
    if (c == Color(0))
        showInfo(tr("Blue has no more moves available."));
    else if ((variant == game_variant_duo && c == Color(1))
             || (variant != game_variant_duo && c == Color(3)))
        showInfo(tr("Green has no more moves available."));
    else if (c == Color(1))
        showInfo(tr("Yellow has no more moves available."));
    else
        showInfo(tr("Red has no more moves available."));
}

QMessageBox::StandardButton MainWindow::showQuestion(const QString& text,
                                           QMessageBox::StandardButtons buttons)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Pentobi"));
    msgBox.setText(text);
    msgBox.setStandardButtons(buttons);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

void MainWindow::showStatus(const QString& text, bool temporary)
{
    int timeout = (temporary ? 4000 : 0);
    statusBar()->showMessage(text, timeout);
}

void MainWindow::splitterMoved(int pos, int index)
{
    LIBBOARDGAME_UNUSED(pos);
    LIBBOARDGAME_UNUSED(index);
    m_actionShowComment->setChecked(m_comment->height() > 0);
}

void MainWindow::truncate()
{
    if (! m_game->get_current().has_parent())
        return;
    m_game->truncate();
    boardChanged(true);
}

void MainWindow::updateComment()
{
    string comment = m_game->get_comment();
    if (comment.empty())
    {
        m_comment->clear();
        return;
    }
    // We only recognize UTF8 and ISO-8859-1 in the SGF charset property
    const Node& root = m_game->get_root();
    QString charset = QString(root.get_property("CA", "iso-8859-1").c_str());
    charset = charset.toLower();
    if (charset == "utf-8" || charset == "utf8")
        m_comment->setPlainText(QString::fromUtf8(comment.c_str()));
    else if (charset == "iso-8859-1" || charset == "latin1")
        m_comment->setPlainText(QString::fromLatin1(comment.c_str()));
    else
        m_comment->setPlainText(QString::fromAscii(comment.c_str()));
    m_comment->ensureCursorVisible();
    m_comment->clearFocus();
}

void MainWindow::updateFlipActions()
{
    const Piece* piece = m_guiBoard->getSelectedPiece();
    if (piece == 0)
        return;
    Transform transform = m_guiBoard->getSelectedPieceTransform();
    bool can_flip_horizontally = piece->can_flip_horizontally(transform);
    m_actionFlipPieceHorizontally->setEnabled(can_flip_horizontally);
    bool can_flip_vertically = piece->can_flip_vertically(transform);
    m_actionFlipPieceVertically->setEnabled(can_flip_vertically);
}

void MainWindow::updateMoveAnnotationActions()
{
    if (m_game->get_move().is_null())
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
            text = QString(
                           /*: Label in Recent Files menu. The first 10 items
                             are numbered to provide a mnemonic. %1 is replaced
                             by the number, %2 by the file name. */
                           tr("&%1: %2")
                           ).arg(i + 1).arg(name);
        else
            text = QString("%1").arg(name);
        m_actionRecentFile[i]->setText(text);
        m_actionRecentFile[i]->setData(files[i]);
        m_actionRecentFile[i]->setVisible(true);
    }
    for (int j = nuRecentFiles; j < maxRecentFiles; ++j)
        m_actionRecentFile[j]->setVisible(false);
}

void MainWindow::useBook(bool checked)
{
    m_useBook = checked;
    QSettings settings;
    settings.setValue("use_book", m_useBook);
}

void MainWindow::veryBadMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_bad_move(2);
    boardChanged(false);
}

void MainWindow::veryGoodMove(bool checked)
{
    if (! checked)
        return;
    m_game->set_good_move(2);
    boardChanged(false);
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta() / 8 / 15;
    if (delta > 0)
    {
        for (int i = 0; i < delta; ++i)
            nextTransform();
    }
    else if (delta < 0)
    {
        for (int i = 0; i < -delta; ++i)
            previousTransform();
    }
    event->accept();
}

//-----------------------------------------------------------------------------
