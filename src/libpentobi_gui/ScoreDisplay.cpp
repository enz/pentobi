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

QSpacerItem* createSpacer()
{
    return new QSpacerItem(8, 0, QSizePolicy::MinimumExpanding,
                           QSizePolicy::Ignored);
}

} // namespace

//-----------------------------------------------------------------------------

ScoreDisplay::ScoreDisplay(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    setLayout(m_layout);
    m_layout->setSpacing(0);
    m_spacer1 = createSpacer();
    m_layout->addSpacerItem(m_spacer1);
    m_iconBlue = createIcon(Color(0));
    m_layout->addWidget(m_iconBlue);
    m_textBlue = new QLabel("0");
    m_layout->addWidget(m_textBlue);
    m_spacer2 = createSpacer();
    m_layout->addSpacerItem(m_spacer2);
    m_iconYellow = createIcon(Color(1));
    m_layout->addWidget(m_iconYellow);
    m_textYellow = new QLabel("0");
    m_layout->addWidget(m_textYellow);
    m_spacer3 = createSpacer();
    m_layout->addSpacerItem(m_spacer3);
    m_iconRed = createIcon(Color(2));
    m_layout->addWidget(m_iconRed);
    m_textRed = new QLabel("0");
    m_layout->addWidget(m_textRed);
    m_spacer4 = createSpacer();
    m_layout->addSpacerItem(m_spacer4);
    m_iconGreen = createIcon(Color(3));
    m_layout->addWidget(m_iconGreen);
    m_textGreen = new QLabel("0");
    m_layout->addWidget(m_textGreen);
    m_spacer5 = createSpacer();
    m_layout->addSpacerItem(m_spacer5);
    m_gameVariant = game_variant_classic;
}

QLabel* ScoreDisplay::createIcon(Color c)
{
    QImage image(16, 16, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    QColor color = Util::getPaintColor(game_variant_classic, c);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(4, 4, 8, 8);
    painter.end();
    QPixmap pixmap = QPixmap::fromImage(image);
    QIcon icon(pixmap);
    QLabel* label = new QLabel();
    label->setPixmap(pixmap);
    return label;
}

void ScoreDisplay::initGameVariant(GameVariant variant)
{
    m_gameVariant = variant;
    m_layout->removeWidget(m_iconBlue);
    m_layout->removeWidget(m_iconYellow);
    m_layout->removeWidget(m_iconRed);
    m_layout->removeWidget(m_iconGreen);
    m_layout->removeWidget(m_textBlue);
    m_layout->removeWidget(m_textYellow);
    m_layout->removeWidget(m_textRed);
    m_layout->removeWidget(m_textGreen);
    m_layout->removeItem(m_spacer1);
    m_layout->removeItem(m_spacer2);
    m_layout->removeItem(m_spacer3);
    m_layout->removeItem(m_spacer4);
    m_layout->removeItem(m_spacer5);
    if (variant == game_variant_classic)
    {
        m_layout->addSpacerItem(m_spacer1);
        m_layout->addWidget(m_iconBlue);
        m_layout->addWidget(m_textBlue);
        m_layout->addSpacerItem(m_spacer2);
        m_layout->addWidget(m_iconYellow);
        m_layout->addWidget(m_textYellow);
        m_layout->addSpacerItem(m_spacer3);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer5);
        m_iconYellow->setVisible(true);
        m_textYellow->setVisible(true);
        m_iconRed->setVisible(true);
        m_textRed->setVisible(true);
        m_spacer2->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer3->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer4->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
    }
    else if (variant == game_variant_duo)
    {
        m_layout->addSpacerItem(m_spacer1);
        m_layout->addWidget(m_iconBlue);
        m_layout->addWidget(m_textBlue);
        m_layout->addSpacerItem(m_spacer2);
        m_layout->addWidget(m_iconYellow);
        m_layout->addWidget(m_textYellow);
        m_layout->addSpacerItem(m_spacer3);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer5);
        m_iconYellow->setVisible(false);
        m_textYellow->setVisible(false);
        m_iconRed->setVisible(false);
        m_textRed->setVisible(false);
        m_spacer2->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer3->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer4->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
    }
    else
    {
        m_layout->addSpacerItem(m_spacer1);
        m_layout->addWidget(m_iconBlue);
        m_layout->addWidget(m_textBlue);
        m_layout->addSpacerItem(m_spacer2);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer3);
        m_layout->addWidget(m_iconYellow);
        m_layout->addWidget(m_textYellow);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer5);
        m_iconYellow->setVisible(true);
        m_textYellow->setVisible(true);
        m_iconRed->setVisible(true);
        m_textRed->setVisible(true);
        m_spacer2->changeSize(4, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer3->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer4->changeSize(4, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
    }
    m_layout->invalidate();
}

void ScoreDisplay::updateScore(const Board& bd)
{
    GameVariant variant = bd.get_game_variant();
    if (variant != m_gameVariant)
        initGameVariant(variant);
    if (variant == game_variant_classic
        || variant == game_variant_classic_2)
    {
        updateScore(m_textBlue, bd, Color(0));
        updateScore(m_textYellow, bd, Color(1));
        updateScore(m_textRed, bd, Color(2));
        updateScore(m_textGreen, bd, Color(3));
    }
    else
    {
        updateScore(m_textBlue, bd, Color(0));
        updateScore(m_textGreen, bd, Color(1));
    }
}

void ScoreDisplay::updateScore(QLabel* label, const Board& bd, Color c)
{
    unsigned int points = bd.get_points(c);
    unsigned int bonus = bd.get_bonus(c);
    QString text;
    if (bonus == 0)
        text.setNum(points);
    else
        text = QString("%1 (+%2)").arg(points).arg(bonus);
    label->setText(text);
}

//-----------------------------------------------------------------------------
