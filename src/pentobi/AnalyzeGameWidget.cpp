//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWidget.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGameWidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QtConcurrentRun>
#include "Util.h"
#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using libboardgame_sgf::util::find_root;
using libboardgame_sgf::util::is_main_variation;
using libboardgame_util::log;
using libboardgame_util::set_abort;
using libboardgame_util::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::PentobiTree;

//-----------------------------------------------------------------------------

AnalyzeGameWidget::AnalyzeGameWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(240, 120);
    m_isInitialized = false;
    m_currentPosition = -1;
}

void AnalyzeGameWidget::cancel()
{
    if (! m_isRunning)
        return;
    set_abort();
    m_future.waitForFinished();
}

void AnalyzeGameWidget::initSize()
{
    m_borderX = width() / 50;
    m_borderY = height() / 20;
    m_maxX = width() - 2 * m_borderX;
    // Assume max. Board::max_nonpass_game_moves. This is not really true
    // because the tree may contain pass moves but such trees are only written
    // in search state dumps and cannot be created by the user. Using
    // max_game_moves would result in too small m_dX (size of the color dots)
    // and adapting m_dX to the number of moves in this game would result in
    // color dots of different sizes for different analyses, which also looks
    // bad.
    m_dX = qreal(m_maxX) / Board::max_nonpass_game_moves;
    m_maxY = height() - 2 * m_borderY;
}

void AnalyzeGameWidget::mousePressEvent(QMouseEvent* event)
{
    if (! m_isInitialized && m_isRunning)
        return;
    unsigned moveNumber =
        static_cast<unsigned>((event->x() - m_borderX) / m_dX);
    if (moveNumber >= m_analyzeGame.get_nu_moves())
        return;
    vector<ColorMove> moves;
    for (unsigned i = 0; i < moveNumber; ++i)
        moves.push_back(m_analyzeGame.get_move(i));
    emit gotoPosition(m_analyzeGame.get_variant(), moves);
}

void AnalyzeGameWidget::paintEvent(QPaintEvent*)
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
    unsigned nu_moves = m_analyzeGame.get_nu_moves();
    if (m_currentPosition >= 0
        && static_cast<unsigned>(m_currentPosition) < nu_moves)
    {
        QPen pen(QColor(96, 96, 96));
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        int x = static_cast<int>(m_currentPosition * m_dX + 0.5 * m_dX);
        painter.drawLine(x, 0, x, m_maxY);
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
    painter.drawLine(0, m_maxY / 2, m_maxX, m_maxY / 2);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (unsigned i = 0; i < nu_moves; ++i)
    {
        if (! m_analyzeGame.has_value(i))
            continue;
        double value = m_analyzeGame.get_value(i);
        // Values can be outside [0..1] due to score/length bonuses
        if (value < 0)
            value = 0;
        else if (value > 1)
            value = 1;
        auto color = Util::getPaintColor(m_analyzeGame.get_variant(),
                                         m_analyzeGame.get_move(i).color);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(QPointF((i + 0.5) * m_dX, (1 - value) * m_maxY),
                            0.5 * m_dX, 0.5 * m_dX);
    }
}

void AnalyzeGameWidget::resizeEvent(QResizeEvent*)
{
    if (! m_isInitialized)
        return;
    initSize();
}

void AnalyzeGameWidget::setCurrentPosition(const Game& game,
                                           const SgfNode& node)
{
    update();
    m_currentPosition = -1;
    if (is_main_variation(node))
    {
        ArrayList<ColorMove,Board::max_game_moves> moves;
        auto& tree = game.get_tree();
        auto current = &find_root(node);
        while (current)
        {
            auto mv = tree.get_move(*current);
            if (! mv.is_null() && moves.size() < Board::max_game_moves)
                moves.push_back(mv);
            if (current == &node)
                break;
            current = current->get_first_child_or_null();
        }
        if (moves.size() <= m_analyzeGame.get_nu_moves())
        {
            for (unsigned i = 0; i < moves.size(); ++i)
                if (moves[i] != m_analyzeGame.get_move(i))
                    return;
            m_currentPosition = moves.size();
        }
    }
}

void AnalyzeGameWidget::showProgress(int progress)
{
    m_progressDialog->setValue(progress);
    // Repaint the window with the current status of the analysis
    update();
}

QSize AnalyzeGameWidget::sizeHint() const
{
    return QSize(800, 240);
}

void AnalyzeGameWidget::start(const Game& game, Search& search,
                              size_t nuSimulations)
{
    m_isInitialized = true;
    m_game = &game;
    m_search = &search;
    m_nuSimulations = nuSimulations;
    initSize();
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setWindowFlags(m_progressDialog->windowFlags()
                                     & ~Qt::WindowContextHelpButtonHint);
    m_progressDialog->setLabel(new QLabel(tr("Running game analysis..."),
                                          this));
    Util::setNoTitle(*m_progressDialog);
    m_progressDialog->setMinimumDuration(0);
    connect(m_progressDialog, SIGNAL(canceled()), SLOT(cancel()));
    m_progressDialog->show();
    m_isRunning = true;
    m_future = QtConcurrent::run(this, &AnalyzeGameWidget::threadFunction);
}

void AnalyzeGameWidget::threadFunction()
{
    // This function and the progress callback are not called from the GUI
    // thread. So we need to invoke showProgress() with invokeMethod().
    auto progressCallback =
        [&](unsigned movesAnalyzed, unsigned totalMoves)
        {
            if (totalMoves == 0)
                return;
            int progress = 100 * movesAnalyzed / totalMoves;
            QMetaObject::invokeMethod(this, "showProgress",
                                      Qt::QueuedConnection,
                                      Q_ARG(int, progress));
        };
    m_analyzeGame.run(*m_game, *m_search, m_nuSimulations, progressCallback);
    QMetaObject::invokeMethod(this, "showProgress", Qt::QueuedConnection,
                              Q_ARG(int, 100));
    m_isRunning = false;
    emit finished();
}

//-----------------------------------------------------------------------------
