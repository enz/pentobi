//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ScoreDisplay.h"

#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_util::log;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;

//-----------------------------------------------------------------------------

ScoreDisplay::ScoreDisplay(QWidget* parent)
    : QWidget(parent)
{
    m_gameVariant = game_variant_classic;
    m_points.fill(0);
    m_bonus.fill(0);
    setMinimumWidth(300);
    setMinimumHeight(15);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void ScoreDisplay::drawScore(QPainter& painter, Color c, int x)
{
    QColor color = Util::getPaintColor(m_gameVariant, c);
    QString text = getScoreText(c);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    int y = static_cast<int>(ceil(0.5 * (height() - m_colorDotSize)));
    painter.drawEllipse(x,  y, m_colorDotSize, m_colorDotSize);
    painter.setPen(QApplication::palette().text().color());
    painter.drawText(x + m_colorDotWidth, 0, getTextWidth(text), height(),
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
}

void ScoreDisplay::drawScore2(QPainter& painter, Color c1, Color c2, int x)
{
    QColor color = Util::getPaintColor(m_gameVariant, c1);
    QString text = getScoreText2(c1, c2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    int y = static_cast<int>(ceil(0.5 * (height() - m_colorDotSize)));
    painter.drawEllipse(x, y, m_colorDotSize, m_colorDotSize);
    color = Util::getPaintColor(m_gameVariant, c2);
    painter.setBrush(color);
    painter.drawEllipse(x + m_colorDotSize, y, m_colorDotSize, m_colorDotSize);
    painter.setPen(QApplication::palette().text().color());
    painter.drawText(x + m_twoColorDotWidth, 0, getTextWidth(text), height(),
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
}

int ScoreDisplay::getMaxScoreTextWidth() const
{
    return getTextWidth(getScoreText(188, 20));
}

int ScoreDisplay::getMaxScoreTextWidth2() const
{
    return getTextWidth(getScoreText(88, 20));
}

QString ScoreDisplay::getScoreText(unsigned int points,
                                   unsigned int bonus) const
{
    QString text;
    text.setNum(points + bonus);
    return text;
}

QString ScoreDisplay::getScoreText(Color c)
{
    return getScoreText(m_points[c], m_bonus[c]);
}

QString ScoreDisplay::getScoreText2(Color c1, Color c2)
{
    return getScoreText(m_points[c1] + m_points[c2], m_bonus[c1] + m_bonus[c2]);
}

int ScoreDisplay::getScoreTextWidth(Color c)
{
    return getTextWidth(getScoreText(c));
}

int ScoreDisplay::getScoreTextWidth2(Color c1, Color c2)
{
    return getTextWidth(getScoreText2(c1, c2));
}

int ScoreDisplay::getTextWidth(QString text) const
{
    // Make text width only depend on number of digits to avoid frequent small
    // changes to the layout
    QFontMetrics metrics(font());
    int maxDigitWidth = 0;
    maxDigitWidth = max(maxDigitWidth, metrics.width('0'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('1'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('2'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('3'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('4'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('5'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('6'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('7'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('8'));
    maxDigitWidth = max(maxDigitWidth, metrics.width('9'));
    return max(text.length() * maxDigitWidth,
                metrics.boundingRect(text).width());
}

void ScoreDisplay::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font;
    int defaultFontSize = font.pointSize();
    // Try to use the default font size, which usually renders better,
    // unless height is significantly larg or small
    int fontSize = 0.7 * height();
    if (fontSize > 0.9 * defaultFontSize && fontSize < 1.5 * defaultFontSize)
        fontSize = defaultFontSize;
    else
        font.setPointSize(fontSize);
    painter.setFont(font);
    m_colorDotSize = 0.8 * fontSize;
    m_colorDotSpace = 0.3 * fontSize;
    m_colorDotWidth = m_colorDotSize + m_colorDotSpace;
    m_twoColorDotWidth = 2 * m_colorDotSize + m_colorDotSpace;
    if (m_gameVariant == game_variant_duo)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthGreen = getScoreTextWidth(Color(1));
        int totalWidth = textWidthBlue + textWidthGreen + 2 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 3.f;
        qreal x = pad;
        drawScore(painter, Color(0), x);
        x+= m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
    }
    else if (m_gameVariant == game_variant_classic
             || m_gameVariant == game_variant_trigon)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int textWidthGreen = getScoreTextWidth(Color(3));
        int totalWidth =
            textWidthBlue + textWidthRed + textWidthYellow + textWidthGreen
            + 4 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 5.f;
        qreal x = pad;
        drawScore(painter, Color(0), x);
        x+= m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
        x+= m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), x);
        x+= m_colorDotWidth + textWidthRed + pad;
        drawScore(painter, Color(3), x);
    }
    else if (m_gameVariant == game_variant_trigon_3)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int totalWidth =
            textWidthBlue + textWidthRed + textWidthYellow
            + 3 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 4.f;
        qreal x = pad;
        drawScore(painter, Color(0), x);
        x+= m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
        x+= m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), x);
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_gameVariant == game_variant_classic_2
                            || m_gameVariant == game_variant_trigon_2);
        int textWidthBlueRed = getScoreTextWidth2(Color(0), Color(2));
        int textWidthYellowGreen = getScoreTextWidth2(Color(1), Color(3));
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int textWidthGreen = getScoreTextWidth(Color(3));
        int totalWidth =
            textWidthBlueRed + textWidthYellowGreen
            + textWidthBlue + textWidthRed + textWidthYellow + textWidthGreen
            + 2 * m_twoColorDotWidth + 4 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 7.f;
        qreal x = pad;
        drawScore2(painter, Color(0), Color(2), x);
        x+= m_twoColorDotWidth + textWidthBlueRed + pad;
        drawScore2(painter, Color(1), Color(3), x);
        x+= m_twoColorDotWidth + textWidthYellowGreen + pad;
        drawScore(painter, Color(0), x);
        x+= m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), x);
        x+= m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), x);
        x+= m_colorDotWidth + textWidthRed + pad;
        drawScore(painter, Color(3), x);
    }
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
