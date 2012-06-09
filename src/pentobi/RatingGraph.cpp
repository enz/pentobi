//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingGraph.h"

#include <boost/foreach.hpp>

//-----------------------------------------------------------------------------

RatingGraph::RatingGraph(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(200, 60);
}

void RatingGraph::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRect(0, 0, width(), height());
    if (! m_values.empty())
    {
        QFontMetrics metrics(painter.font());
        float yRange = m_yMax - m_yMin;
        float yTic = ceil((m_yMin / 100.f)) * 100;
        QPen pen(QColor(96, 96, 96));
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        int maxLabelWidth = 0;
        while (yTic < m_yMax)
        {
            qreal y = height() - (yTic - m_yMin) / yRange * height();
            painter.drawLine(0, y, width(), y);
            QString label;
            label.setNum(yTic, 'f', 0);
            int labelWidth = metrics.width(label);
            maxLabelWidth = max(maxLabelWidth, labelWidth);
            if (y > metrics.height())
                painter.drawText(width() - labelWidth, y - metrics.descent(),
                                 label);
            yTic += 100;
        }
        qreal dX = qreal(width() - maxLabelWidth) / RatingHistory::maxGames;
        qreal x = 0;
        QPainterPath path;
        for (unsigned int i = 0; i < m_values.size(); ++i)
        {
            qreal y = height() - (m_values[i] - m_yMin) / yRange * height();
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

void RatingGraph::setHistory(const RatingHistory& history)
{
    m_values.clear();
    const vector<RatingHistory::GameInfo>& games = history.get();
    if (games.empty())
        return;
    m_yMin = games[0].rating.get();
    m_yMax = m_yMin;
    BOOST_FOREACH(const RatingHistory::GameInfo& info, games)
    {
        float rating = info.rating.get();
        m_yMin = min(m_yMin, rating);
        m_yMax = max(m_yMax, rating);
        m_values.push_back(rating);
    }
    // Make yMin slightly below a multiple of 100, such that the lowest y tics
    // line is not on the edge of the graph.
    m_yMin = floor((m_yMin / 100.f)) * 100 - 10;
    // Leave some space above yMax
    m_yMax += 10;
    update();
}

//-----------------------------------------------------------------------------
