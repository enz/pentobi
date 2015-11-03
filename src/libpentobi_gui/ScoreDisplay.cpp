//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ScoreDisplay.h"

#include <cmath>
#include <QApplication>
#include <QPainter>
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

ScoreDisplay::ScoreDisplay(QWidget* parent)
    : QWidget(parent)
{
    m_variant = Variant::classic;
    m_font.setStyleStrategy(QFont::StyleStrategy(QFont::PreferOutline
                                                 | QFont::PreferQuality));
    setMinimumWidth(300);
    setMinimumHeight(20);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void ScoreDisplay::drawScore(QPainter& painter, Color c, int x)
{
    QColor color = Util::getPaintColor(m_variant, c);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    QFontMetrics metrics(m_font);
    int ascent = metrics.ascent();
    // y is baseline
    int y = static_cast<int>(ceil(0.5 * (height() - ascent)) + ascent);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x,  y - m_colorDotSize, m_colorDotSize,
                        m_colorDotSize);
    QString text = getScoreText(c);
    bool underline = ! m_hasMoves[c];
    bool hasBonus = m_bonus[c] != 0;
    drawText(painter, text, x + m_colorDotWidth, y, underline, hasBonus);
}

void ScoreDisplay::drawScore2(QPainter& painter, Color c1, Color c2, int x)
{
    auto color = Util::getPaintColor(m_variant, c1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    QFontMetrics metrics(m_font);
    int ascent = metrics.ascent();
    // y is baseline
    int y = static_cast<int>(ceil(0.5 * (height() - ascent)) + ascent);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x, y - m_colorDotSize, m_colorDotSize, m_colorDotSize);
    color = Util::getPaintColor(m_variant, c2);
    painter.setBrush(color);
    painter.drawEllipse(x + m_colorDotSize, y - m_colorDotSize, m_colorDotSize,
                        m_colorDotSize);
    QString text = getScoreText2(c1, c2);
    bool underline = (! m_hasMoves[c1] && ! m_hasMoves[c2]);
    drawText(painter, text, x + m_twoColorDotWidth, y, underline, false);
}

void ScoreDisplay::drawScore3(QPainter& painter, int x)
{
    auto color = Util::getPaintColor(m_variant, Color(3));
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    QFontMetrics metrics(m_font);
    int ascent = metrics.ascent();
    // y is baseline
    int y = static_cast<int>(ceil(0.5 * (height() - ascent)) + ascent);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (m_hasMoves[Color(3)])
    {
        painter.drawEllipse(x, y - m_colorDotSize,
                            m_colorDotSize, m_colorDotSize);
        color = Util::getPaintColor(m_variant, m_altPlayer);
        painter.setBrush(color);
        painter.drawEllipse(x + m_colorDotSize, y - m_colorDotSize,
                            m_colorDotSize, m_colorDotSize);
    }
    else
        painter.drawEllipse(x + m_colorDotSize, y - m_colorDotSize,
                            m_colorDotSize, m_colorDotSize);
    QString text = getScoreText3();
    bool underline = ! m_hasMoves[Color(3)];
    drawText(painter, text, x + m_twoColorDotWidth, y, underline, false);
}

void ScoreDisplay::drawText(QPainter& painter, const QString& text, int x,
                            int y, bool underline, bool hasBonus)
{
    painter.setFont(m_font);
    QFontMetrics metrics(m_font);
    auto color = QApplication::palette().color(QPalette::WindowText);
    painter.setPen(color);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawText(x, y, text);
    if (underline)
    {
        // Draw underline (instead of using an underlined font because the
        // underline of some fonts is too close to the text and we want it
        // to be very visible)
        int lineWidth = metrics.lineWidth();
        QPen pen(color);
        pen.setWidth(lineWidth);
        painter.setPen(pen);
        y += 2 * lineWidth;
        if (y > height() - 1)
            y = height() - 1;
        painter.drawLine(x, y,
                         x + metrics.width(hasBonus ?
                                               text.left(text.length() - 1) :
                                               text),
                         y);
    }
}

QString ScoreDisplay::getScoreText(unsigned points, unsigned bonus) const
{
    QString text;
    text.setNum(points);
    if (bonus == 15)
        text.append("\u00B9");
    else if (bonus == 20)
        text.append("\u00B2");
    return text;
}

QString ScoreDisplay::getScoreText(Color c)
{
    return getScoreText(m_points[c], m_bonus[c]);
}

QString ScoreDisplay::getScoreText2(Color c1, Color c2)
{
    return getScoreText(m_points[c1] + m_points[c2], 0);
}

QString ScoreDisplay::getScoreText3()
{
    return "(" + getScoreText(Color(3)) + ")";
}

int ScoreDisplay::getScoreTextWidth(Color c)
{
    return getTextWidth(getScoreText(c));
}

int ScoreDisplay::getScoreTextWidth2(Color c1, Color c2)
{
    return getTextWidth(getScoreText2(c1, c2));
}

int ScoreDisplay::getScoreTextWidth3()
{
    return getTextWidth(getScoreText3());
}

int ScoreDisplay::getTextWidth(QString text) const
{
    // Make text width only depend on number of digits to avoid frequent small
    // changes to the layout
    QFontMetrics metrics(m_font);
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
    if (m_variant == Variant::duo || m_variant == Variant::junior)
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
    else if (m_variant == Variant::classic || m_variant == Variant::trigon)
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
    else if (m_variant == Variant::classic_3)
    {
        int textWidthBlue = getScoreTextWidth(Color(0));
        int textWidthYellow = getScoreTextWidth(Color(1));
        int textWidthRed = getScoreTextWidth(Color(2));
        int textWidthGreen = getScoreTextWidth3();
        int totalWidth =
            textWidthBlue + textWidthRed + textWidthYellow + textWidthGreen
            + 3 * m_colorDotWidth + m_twoColorDotWidth;
        qreal pad = qreal(width() - totalWidth) / 5.f;
        qreal x = pad;
        drawScore(painter, Color(0), static_cast<int>(x));
        x += m_colorDotWidth + textWidthBlue + pad;
        drawScore(painter, Color(1), static_cast<int>(x));
        x += m_colorDotWidth + textWidthYellow + pad;
        drawScore(painter, Color(2), static_cast<int>(x));
        x += m_colorDotWidth + textWidthRed + pad;
        drawScore3(painter, static_cast<int>(x));
    }
    else if (m_variant == Variant::trigon_3)
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
        LIBBOARDGAME_ASSERT(m_variant == Variant::classic_2
                            || m_variant == Variant::trigon_2);
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
    m_fontSize = static_cast<int>(floor(0.6 * height()));
    m_font.setPixelSize(m_fontSize);
}

void ScoreDisplay::updateScore(const Board& bd)
{
    auto variant = bd.get_variant();
    bool hasChanged = (m_variant != variant);
    m_variant = variant;
    for (Color c : bd.get_colors())
    {
        bool hasMoves = bd.has_moves(c);
        auto points = bd.get_points(c);
        auto bonus = bd.get_bonus(c);
        if (hasMoves != m_hasMoves[c] || m_points[c] != points
                || m_bonus[c] != points)
        {
            hasChanged = true;
            m_hasMoves[c] = hasMoves;
            m_points[c] = points;
            m_bonus[c] = bonus;
        }
    }
    if (variant == Variant::classic_3)
        m_altPlayer = Color(bd.get_alt_player());
    if (hasChanged)
        update();
}

//-----------------------------------------------------------------------------
