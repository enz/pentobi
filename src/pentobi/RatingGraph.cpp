//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingGraph.h"

#include <QPainter>
#include <QPen>

//-----------------------------------------------------------------------------

RatingGraph::RatingGraph(QWidget* parent)
    : QFrame(parent)
{
    setMinimumSize(200, 60);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
}

void RatingGraph::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QRect contentsRect = QFrame::contentsRect();
    int width = contentsRect.width();
    int height = contentsRect.height();
    QPainter painter(this);
    painter.translate(contentsRect.x(), contentsRect.y());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRect(0, 0, width, height);
    if (! m_values.empty())
    {
        QFontMetrics metrics(painter.font());
        float yRange = m_yMax - m_yMin;
        float yTic = m_yMin;
        float topMargin = ceil(1.2f * metrics.height());
        float bottomMargin = ceil(0.3f * metrics.height());
        float graphHeight = height - topMargin - bottomMargin;
        QPen pen(QColor(96, 96, 96));
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        int maxLabelWidth = 0;
        while (yTic <= m_yMax)
        {
            qreal y =
                topMargin
                + graphHeight - (yTic - m_yMin) / yRange * graphHeight;
            painter.drawLine(0, y, width, y);
            QString label;
            label.setNum(yTic, 'f', 0);
            int labelWidth = metrics.width(label + " ");
            maxLabelWidth = max(maxLabelWidth, labelWidth);
            painter.drawText(width - labelWidth, y - metrics.descent(),
                             label);
            if (yRange < 600)
                yTic += 100;
            else
                yTic += 200;
        }
        qreal dX = qreal(width - maxLabelWidth) / RatingHistory::maxGames;
        qreal x = 0;
        QPainterPath path;
        for (unsigned i = 0; i < m_values.size(); ++i)
        {
            qreal y =
                topMargin
                + graphHeight - (m_values[i] - m_yMin) / yRange * graphHeight;
            if (i == 0)
                path.moveTo(x, y);
            else
                path.lineTo(x, y);
            x += dX;
        }
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

QSize RatingGraph::sizeHint() const
{
    return QSize(480, 120);
}

void RatingGraph::updateContent(const RatingHistory& history)
{
    m_values.clear();
    auto& games = history.getGameInfos();
    if (games.empty())
    {
        update();
        return;
    }
    m_yMin = games[0].rating.get();
    m_yMax = m_yMin;
    for (const RatingHistory::GameInfo& info : games)
    {
        float rating = info.rating.get();
        m_yMin = min(m_yMin, rating);
        m_yMax = max(m_yMax, rating);
        m_values.push_back(rating);
    }
    m_yMin = floor((m_yMin / 100.f)) * 100;
    m_yMax = ceil((m_yMax / 100.f)) * 100;
    if (m_yMax == m_yMin)
        m_yMax = m_yMin + 100;
    update();
}

//-----------------------------------------------------------------------------
