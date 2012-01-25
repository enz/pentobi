//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGameWindow.h"

#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using libboardgame_util::log;
using libboardgame_util::set_abort;
using libpentobi_base::Board;

//-----------------------------------------------------------------------------

AnalyzeGameWindow::AnalyzeGameWindow(QWidget* parent, Game& game,
                                     Search& search)
    : QDialog(parent)
{
    setWindowTitle(tr("Pentobi - Game Analysis"));
    setMinimumSize(240, 120);
    const Board& bd = game.get_board();
    m_maxMoves = bd.get_nu_pieces() * bd.get_nu_colors();
    show();
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setLabel(new QLabel(tr("Running game analysis..."),
                                          this));
    m_progressDialog->setCancelButton(0);
    m_progressDialog->setWindowTitle(tr("Pentobi - Progress"));
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->show();
    m_analyzeGame.run(game, search,
                      bind(&AnalyzeGameWindow::progressCallback, this,
                           placeholders::_1, placeholders::_2));
    m_progressDialog->setValue(100);
}

void AnalyzeGameWindow::mousePressEvent(QMouseEvent* event)
{
    unsigned int moveNumber = (event->x() - m_borderX) / m_dX;
    if (moveNumber >= m_analyzeGame.get_nu_moves())
        return;
    vector<ColorMove> moves;
    for (unsigned int i = 0; i < moveNumber; ++i)
        moves.push_back(m_analyzeGame.get_move(i));
    emit gotoPosition(m_analyzeGame.get_game_variant(), moves);
}

void AnalyzeGameWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QFont font;
    font.setStyleStrategy(QFont::PreferOutline);
    font.setPointSizeF(0.05 * height());
    QFontMetrics metrics(font);
    painter.translate(m_borderX, m_borderY);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(240, 240, 240));
    painter.drawRect(0, 0, m_maxX, m_maxY);
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
    for (unsigned int i = 0; i < m_analyzeGame.get_nu_moves(); ++i)
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
    m_progressDialog->setValue(100 * movesAnalyzed / totalMoves);
    update();
}

void AnalyzeGameWindow::resizeEvent(QResizeEvent* event)
{
    m_borderX = width() / 20;
    m_borderY = height() / 20;
    m_maxX = width() - 2 * m_borderX;
    m_dX = qreal(m_maxX) / m_maxMoves;
    m_maxY = height() - 2 * m_borderY;
}

//-----------------------------------------------------------------------------
