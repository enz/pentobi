//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ScoreDisplay.h"

#include <QApplication>
#include <QPainter>
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_util::log;
using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;

//-----------------------------------------------------------------------------

ScoreDisplay::ScoreDisplay(QWidget* parent)
    : QWidget(parent),
      m_hasMoves(false),
      m_points(0),
      m_bonus(0)
{
    m_variant = variant_classic;
    m_font.setStyleStrategy(QFont::PreferOutline);
    m_fontUnderlined = m_font;
    m_fontUnderlined.setUnderline(true);
    setMinimumWidth(300);
    setMinimumHeight(20);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void ScoreDisplay::drawScore(QPainter& painter, Color c, int x)
{
    QColor color = Util::getPaintColor(m_variant, c);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    int y = static_cast<int>(ceil(0.5 * (height() - m_colorDotSize)));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x,  y, m_colorDotSize, m_colorDotSize);
    QString text = getScoreText(c);
    if (m_hasMoves[c])
        painter.setFont(m_font);
    else
        painter.setFont(m_fontUnderlined);
    painter.setPen(QApplication::palette().color(QPalette::WindowText));
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawText(x + m_colorDotWidth, 0, getTextWidth(text), height(),
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
}

void ScoreDisplay::drawScore2(QPainter& painter, Color c1, Color c2, int x)
{
    QColor color = Util::getPaintColor(m_variant, c1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    int y = static_cast<int>(ceil(0.5 * (height() - m_colorDotSize)));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x, y, m_colorDotSize, m_colorDotSize);
    color = Util::getPaintColor(m_variant, c2);
    painter.setBrush(color);
    painter.drawEllipse(x + m_colorDotSize, y, m_colorDotSize, m_colorDotSize);
    QString text = getScoreText2(c1, c2);
    if (m_hasMoves[c1] || m_hasMoves[c2])
        painter.setFont(m_font);
    else
        painter.setFont(m_fontUnderlined);
    painter.setPen(QApplication::palette().text().color());
    painter.setRenderHint(QPainter::Antialiasing, false);
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
    QFontMetrics metrics(m_fontUnderlined);
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

void ScoreDisplay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    m_colorDotSize = static_cast<int>(0.8 * m_fontSize);
    m_colorDotSpace = static_cast<int>(0.3 * m_fontSize);
    m_colorDotWidth = m_colorDotSize + m_colorDotSpace;
    m_twoColorDotWidth = 2 * m_colorDotSize + m_colorDotSpace;
    if (m_variant == variant_duo || m_variant == variant_junior)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthGreen = getScoreTextWidth(Color(1));
        int totalWidth = textWidthBlue + textWidthGreen + 2 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 3.f;
        qreal x = pad;
        drawScore(painter, Color(0), static_cast<int>(x));
        x += m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), static_cast<int>(x));
    }
    else if (m_variant == variant_classic || m_variant == variant_trigon)
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
        drawScore(painter, Color(0), static_cast<int>(x));
        x += m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), static_cast<int>(x));
        x += m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), static_cast<int>(x));
        x += m_colorDotWidth + textWidthRed + pad;
        drawScore(painter, Color(3), static_cast<int>(x));
    }
    else if (m_variant == variant_trigon_3)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int totalWidth =
            textWidthBlue + textWidthRed + textWidthYellow
            + 3 * m_colorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 4.f;
        qreal x = pad;
        drawScore(painter, Color(0), static_cast<int>(x));
        x += m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), static_cast<int>(x));
        x += m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), static_cast<int>(x));
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == variant_classic_2
                            || m_variant == variant_trigon_2);
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
        drawScore2(painter, Color(0), Color(2), static_cast<int>(x));
        x += m_twoColorDotWidth + textWidthBlueRed + pad;
        drawScore2(painter, Color(1), Color(3), static_cast<int>(x));
        x += m_twoColorDotWidth + textWidthYellowGreen + pad;
        drawScore(painter, Color(0), static_cast<int>(x));
        x += m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), static_cast<int>(x));
        x += m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), static_cast<int>(x));
        x += m_colorDotWidth + textWidthRed + pad;
        drawScore(painter, Color(3), static_cast<int>(x));
    }
}

void ScoreDisplay::resizeEvent(QResizeEvent*)
{
    m_fontSize = static_cast<int>(floor(0.7 * height()));
    m_font.setPixelSize(m_fontSize);
    m_fontUnderlined.setPixelSize(m_fontSize);
}

void ScoreDisplay::updateScore(const Board& bd)
{
    Variant variant = bd.get_variant();
    bool hasChanged = (m_variant != variant);
    m_variant = variant;
    for (unsigned int i = 0; i < bd.get_nu_colors(); ++i)
    {
        Color c(i);
        bool hasMoves = bd.has_moves(c);
        unsigned int points = bd.get_points(c);
        unsigned int bonus = bd.get_bonus(c);
        if (hasMoves != m_hasMoves[c] || m_points[c] != points
            || m_bonus[c] != bonus)
        {
            hasChanged = true;
            m_hasMoves[c] = hasMoves;
            m_points[c] = points;
            m_bonus[c] = bonus;
        }
    }
    if (hasChanged)
        update();
}

//-----------------------------------------------------------------------------
