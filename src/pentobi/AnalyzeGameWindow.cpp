//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGameWindow.h"

#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using libboardgame_sgf::util::find_root;
using libboardgame_sgf::util::is_main_variation;
using libboardgame_util::log;
using libboardgame_util::set_abort;
using libboardgame_util::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::Tree;

//-----------------------------------------------------------------------------

AnalyzeGameWindow::AnalyzeGameWindow(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Pentobi - Game Analysis"));
    setMinimumSize(240, 120);
    m_isInitialized = false;
    m_currentPosition = -1;
}

void AnalyzeGameWindow::cancel()
{
    if (! m_isRunning)
        return;
    set_abort();
    m_future.waitForFinished();
}

void AnalyzeGameWindow::initSize()
{
    m_borderX = width() / 50;
    m_borderY = height() / 20;
    m_maxX = width() - 2 * m_borderX;
    m_dX = qreal(m_maxX) / Board::max_game_moves;
    m_maxY = height() - 2 * m_borderY;
}

void AnalyzeGameWindow::mousePressEvent(QMouseEvent* event)
{
    if (! m_isInitialized && m_isRunning)
        return;
    unsigned int moveNumber = (event->x() - m_borderX) / m_dX;
    if (moveNumber >= m_analyzeGame.get_nu_moves())
        return;
    vector<ColorMove> moves;
    for (unsigned int i = 0; i < moveNumber; ++i)
        moves.push_back(m_analyzeGame.get_move(i));
    emit gotoPosition(m_analyzeGame.get_game_variant(), moves);
}

void AnalyzeGameWindow::paintEvent(QPaintEvent*)
{
    if (! m_isInitialized)
        return;
    QPainter painter(this);
    QFont font;
    font.setStyleStrategy(QFont::PreferOutline);
    font.setPointSizeF(0.05 * height());
    QFontMetrics metrics(font);
    painter.translate(m_borderX, m_borderY);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(240, 240, 240));
    painter.drawRect(0, 0, m_maxX, m_maxY);
    unsigned int nu_moves = m_analyzeGame.get_nu_moves();
    if (m_currentPosition >= 0
        && static_cast<unsigned int>(m_currentPosition) < nu_moves)
    {
        painter.setBrush(QColor(224, 224, 224));
        painter.drawRect(m_currentPosition * m_dX, 0, m_dX, m_maxY);
    }
    painter.setPen(QColor(32, 32, 32));
    painter.drawLine(0, 0, m_maxX, 0);
    painter.drawLine(0, m_maxY, m_maxX, m_maxY);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QString labelWin = tr("Win");
    QRect boundingRectWin = metrics.boundingRect(labelWin);
    painter.drawText(QRect(0, 0, boundingRectWin.width(),
                           boundingRectWin.height()),
                     Qt::AlignLeft | Qt::AlignTop | Qt::TextDontClip,
                     labelWin);
    QString labelLoss = tr("Loss");
    QRect boundingRectLoss = metrics.boundingRect(labelLoss);
    painter.drawText(QRect(0, m_maxY - boundingRectLoss.height(),
                           boundingRectLoss.width(), boundingRectLoss.height()),
                     Qt::AlignLeft | Qt::AlignBottom | Qt::TextDontClip,
                     labelLoss);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QColor(128, 128, 128));
    painter.drawLine(0, 0.5 * m_maxY, m_maxX, 0.5 * m_maxY);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (unsigned int i = 0; i < nu_moves; ++i)
    {
        if (! m_analyzeGame.has_value(i))
            continue;
        double value = m_analyzeGame.get_value(i);
        QColor color = Util::getPaintColor(m_analyzeGame.get_game_variant(),
                                           m_analyzeGame.get_move(i).color);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(QPointF((i + 0.5) * m_dX, (1 - value) * m_maxY),
                            0.5 * m_dX, 0.5 * m_dX);
    }
}

void AnalyzeGameWindow::progressCallback(unsigned int movesAnalyzed,
                                         unsigned int totalMoves)
{
    if (totalMoves == 0)
        return;
    // This function is called from a diffrent thread. Invoke showProgress()
    // in the GUI thread.
    QMetaObject::invokeMethod(this, "showProgress", Qt::QueuedConnection,
                              Q_ARG(int, 100 * movesAnalyzed / totalMoves));
}

void AnalyzeGameWindow::resizeEvent(QResizeEvent*)
{
    if (! m_isInitialized)
        return;
    initSize();
}

void AnalyzeGameWindow::setCurrentPosition(const Game& game, const Node& node)
{
    update();
    m_currentPosition = -1;
    if (is_main_variation(node))
    {
        ArrayList<ColorMove,Board::max_game_moves> moves;
        const Tree& tree = game.get_tree();
        const Node* current = &find_root(node);
        while (current != 0)
        {
            ColorMove mv = tree.get_move(*current);
            if (! mv.is_null() && moves.size() < Board::max_game_moves)
                moves.push_back(mv);
            if (current == &node)
                break;
            current = current->get_first_child_or_null();
        }
        if (moves.size() <= m_analyzeGame.get_nu_moves())
        {
            for (unsigned int i = 0; i < moves.size(); ++i)
                if (moves[i] != m_analyzeGame.get_move(i))
                    return;
            m_currentPosition = moves.size();
        }
    }
}

void AnalyzeGameWindow::showProgress(int progress)
{
    m_progressDialog->setValue(progress);
    // Repaint the window with the current status of the analysis
    update();
}

QSize AnalyzeGameWindow::sizeHint() const
{
    return QSize(800, 240);
}

void AnalyzeGameWindow::start(Game& game, Search& search)
{
    m_isInitialized = true;
    m_game = &game;
    m_search = &search;
    initSize();
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setLabel(new QLabel(tr("Running game analysis..."),
                                          this));
    m_progressDialog->setWindowTitle(tr("Pentobi - Progress"));
    m_progressDialog->setMinimumDuration(0);
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancel()));
    m_progressDialog->show();
    m_isRunning = true;
    m_future = QtConcurrent::run(this, &AnalyzeGameWindow::threadFunction);
}

void AnalyzeGameWindow::threadFunction()
{
    m_analyzeGame.run(*m_game, *m_search,
                      bind(&AnalyzeGameWindow::progressCallback, this,
                           placeholders::_1, placeholders::_2));
    // This function is called from a diffrent thread. Invoke showProgress()
    // in the GUI thread.
    QMetaObject::invokeMethod(this, "showProgress", Qt::QueuedConnection,
                              Q_ARG(int, 100));
    m_isRunning = false;
    emit finished();
}

//-----------------------------------------------------------------------------
