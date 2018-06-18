//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingGraph.h"

#include <QApplication>
#include <QDesktopWidget>
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
        auto yRange = m_yMax - m_yMin;
        auto yTic = m_yMin;
        auto topMargin = ceil(1.2 * static_cast<double>(metrics.height()));
        auto bottomMargin = ceil(0.3 * static_cast<double>(metrics.height()));
        auto graphHeight =
            static_cast<double>(height) - topMargin - bottomMargin;
        QPen pen(QColor(96, 96, 96));
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        int maxLabelWidth = 0;
        while (yTic <= m_yMax)
        {
            auto y =
                static_cast<int>(round(
                    topMargin
                    + graphHeight - (yTic - m_yMin) / yRange * graphHeight));
            painter.drawLine(0, y, width, y);
            QString label;
            label.setNum(yTic, 'f', 0);
            int labelWidth = metrics.boundingRect(label + " ").width();
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
    auto geo = QApplication::desktop()->availableGeometry(this);
    return QSize(geo.width() / 3, min(geo.width() / 12, geo.height() / 3));
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
        auto rating = info.rating.get();
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
