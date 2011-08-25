//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ScoreDisplay.h"

#include "libpentobi_gui/Util.h"

using namespace std;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;

//-----------------------------------------------------------------------------

namespace {

const int colorDotSize = 16;

const int twoColorDotWidth = 25;

int getTextWidth(QString text)
{
    QFont font = QApplication::font();
    QFontMetrics metrics(font);
    return metrics.boundingRect(text).width();
}

QString getScoreText(unsigned int points, unsigned int bonus)
{
    if (bonus == 0)
    {
        QString text;
        text.setNum(points);
        return text;
    }
    return QString("%1 (+%2)").arg(points).arg(bonus);
}

int getMaxScoreTextWidth()
{
    return getTextWidth(getScoreText(188, 20));
}

int getMaxScoreTextWidth2()
{
    return getTextWidth(getScoreText(88, 20));
}

} // namespace

//-----------------------------------------------------------------------------

ScoreDisplay::ScoreDisplay(QWidget* parent)
    : QWidget(parent)
{
    m_gameVariant = game_variant_classic;
    m_points.fill(0);
    m_bonus.fill(0);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void ScoreDisplay::drawScore(QPainter& painter, Color c, int x)
{
    QColor color = Util::getPaintColor(m_gameVariant, c);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(x + 4, 4, 8, 8);
    painter.setPen(QApplication::palette().text().color());
    painter.drawText(QPoint(x + colorDotSize, 12), getScoreText(c));
}

void ScoreDisplay::drawScore2(QPainter& painter, Color c1, Color c2, int x)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor color = Util::getPaintColor(m_gameVariant, c1);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(x + 4, 4, 8, 8);
    color = Util::getPaintColor(m_gameVariant, c2);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(x + 13, 4, 8, 8);
    painter.setPen(QApplication::palette().text().color());
    painter.drawText(QPoint(x + twoColorDotWidth, 12), getScoreText2(c1, c2));
}

QString ScoreDisplay::getScoreText(Color c)
{
    return ::getScoreText(m_points[c], m_bonus[c]);
}

QString ScoreDisplay::getScoreText2(Color c1, Color c2)
{
    return ::getScoreText(m_points[c1] + m_points[c2],
                          m_bonus[c1] + m_bonus[c2]);
}

int ScoreDisplay::getScoreTextWidth(Color c)
{
    return getTextWidth(getScoreText(c));
}

int ScoreDisplay::getScoreTextWidth2(Color c1, Color c2)
{
    return getTextWidth(getScoreText2(c1, c2));
}

void ScoreDisplay::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (m_gameVariant == game_variant_duo)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthGreen = getScoreTextWidth(Color(1));
        int totalWidth = textWidthBlue + textWidthGreen + 2 * colorDotSize;
        float pad = float(width() - totalWidth) / 3.f;
        float x = pad;
        drawScore(painter, Color(0), x);
        x+= colorDotSize + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
    }
    else if (m_gameVariant == game_variant_classic)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int textWidthGreen = getScoreTextWidth(Color(3));
        int totalWidth =
            textWidthBlue + textWidthRed + textWidthYellow + textWidthGreen
            + 4 * colorDotSize;
        float pad = float(width() - totalWidth) / 5.f;
        float x = pad;
        drawScore(painter, Color(0), x);
        x+= colorDotSize + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
        x+= colorDotSize + textWidthYellow + pad;
        drawScore(painter, Color(2), x);
        x+= colorDotSize + textWidthRed + pad;
        drawScore(painter, Color(3), x);
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_gameVariant == game_variant_classic_2);
        int textWidthBlueRed = getScoreTextWidth2(Color(0), Color(2));
        int textWidthYellowGreen = getScoreTextWidth2(Color(1), Color(3));
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int textWidthGreen = getScoreTextWidth(Color(3));
        int totalWidth =
            textWidthBlueRed + textWidthYellowGreen
            + textWidthBlue + textWidthRed + textWidthYellow + textWidthGreen
            + 2 * twoColorDotWidth + 4 * colorDotSize;
        float pad = float(width() - totalWidth) / 8.f;
        float x = pad;
        drawScore2(painter, Color(0), Color(2), x);
        x+= twoColorDotWidth + textWidthBlueRed + pad;
        drawScore2(painter, Color(1), Color(3), x);
        x+= twoColorDotWidth + textWidthYellowGreen + 2 * pad;
        drawScore(painter, Color(0), x);
        x+= colorDotSize + textWidthBlue + pad;
        drawScore(painter, Color(2), x);
        x+= colorDotSize + textWidthRed + pad;
        drawScore(painter, Color(1), x);
        x+= colorDotSize + textWidthYellow + pad;
        drawScore(painter, Color(3), x);
    }
}

QSize ScoreDisplay::sizeHint() const
{
    // worst case is classic_2
    int width =
        2 * twoColorDotWidth + 4 * colorDotSize
        + 2 * getMaxScoreTextWidth2() + 4 * getMaxScoreTextWidth();
    QFont font = QApplication::font();
    QFontMetrics metrics(font);
    int height = max(colorDotSize, metrics.height());
    return QSize(width, height);
}

void ScoreDisplay::updateScore(const Board& bd)
{
    GameVariant variant = bd.get_game_variant();
    bool hasChanged = (m_gameVariant != variant);
    m_gameVariant = variant;
    for (unsigned int i = 0; i < bd.get_nu_colors(); ++i)
    {
        Color c(i);
        unsigned int points = bd.get_points(c);
        unsigned int bonus = bd.get_bonus(c);
        if (m_points[c] != points || m_bonus[c] != bonus)
        {
            hasChanged = true;
            m_points[c] = points;
            m_bonus[c] = bonus;
        }
    }
    if (hasChanged)
        update();
}

//-----------------------------------------------------------------------------
