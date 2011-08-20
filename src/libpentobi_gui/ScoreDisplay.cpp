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
    m_iconBlue = createColorIcon(Color(0));
    m_iconYellow = createColorIcon(Color(1));
    m_iconRed = createColorIcon(Color(2));
    m_iconGreen = createColorIcon(Color(3));
    m_iconBlueRed = createTwoColorIcon(Color(0), Color(2));
    m_iconYellowGreen = createTwoColorIcon(Color(1), Color(3));
    m_textBlue = new QLabel("0");
    m_textYellow = new QLabel("0");
    m_textGreen = new QLabel("0");
    m_textRed = new QLabel("0");
    m_textBlueRed = new QLabel("0");
    m_textYellowGreen = new QLabel("0");
    m_spacer1 = createSpacer();
    m_spacer2 = createSpacer();
    m_spacer3 = createSpacer();
    m_spacer4 = createSpacer();
    m_spacer5 = createSpacer();
    m_spacer6 = createSpacer();
    m_spacer7 = createSpacer();
    m_spacer8 = createSpacer();
    m_gameVariant = game_variant_classic;
    addWidgets(m_gameVariant);
}

void ScoreDisplay::addWidgets(GameVariant variant)
{
    if (variant == game_variant_classic)
    {
        m_layout->addSpacerItem(m_spacer1);
        m_layout->addWidget(m_iconBlue);
        m_layout->addWidget(m_textBlue);
        m_layout->addSpacerItem(m_spacer2);
        m_layout->addWidget(m_iconYellow);
        m_layout->addWidget(m_textYellow);
        m_layout->addSpacerItem(m_spacer3);
        m_layout->addWidget(m_iconBlueRed);
        m_layout->addWidget(m_textBlueRed);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer5);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer6);
        m_layout->addWidget(m_iconYellowGreen);
        m_layout->addWidget(m_textYellowGreen);
        m_layout->addSpacerItem(m_spacer7);
        m_layout->addSpacerItem(m_spacer8);
        m_iconYellow->setVisible(true);
        m_textYellow->setVisible(true);
        m_iconRed->setVisible(true);
        m_textRed->setVisible(true);
        m_iconBlueRed->setVisible(false);
        m_textBlueRed->setVisible(false);
        m_iconYellowGreen->setVisible(false);
        m_textYellowGreen->setVisible(false);
        m_spacer2->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer3->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer4->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer5->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer6->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer7->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
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
        m_layout->addWidget(m_iconBlueRed);
        m_layout->addWidget(m_textBlueRed);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer5);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer6);
        m_layout->addWidget(m_iconYellowGreen);
        m_layout->addWidget(m_textYellowGreen);
        m_layout->addSpacerItem(m_spacer7);
        m_layout->addSpacerItem(m_spacer8);
        m_iconYellow->setVisible(false);
        m_textYellow->setVisible(false);
        m_iconRed->setVisible(false);
        m_textRed->setVisible(false);
        m_iconBlueRed->setVisible(false);
        m_textBlueRed->setVisible(false);
        m_iconYellowGreen->setVisible(false);
        m_textYellowGreen->setVisible(false);
        m_spacer2->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer3->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer4->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer5->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer6->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
        m_spacer7->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == game_variant_classic_2);
        m_layout->addSpacerItem(m_spacer1);
        m_layout->addWidget(m_iconBlueRed);
        m_layout->addWidget(m_textBlueRed);
        m_layout->addSpacerItem(m_spacer2);
        m_layout->addWidget(m_iconYellowGreen);
        m_layout->addWidget(m_textYellowGreen);
        m_layout->addSpacerItem(m_spacer3);
        m_layout->addSpacerItem(m_spacer4);
        m_layout->addWidget(m_iconBlue);
        m_layout->addWidget(m_textBlue);
        m_layout->addSpacerItem(m_spacer5);
        m_layout->addWidget(m_iconRed);
        m_layout->addWidget(m_textRed);
        m_layout->addSpacerItem(m_spacer6);
        m_layout->addWidget(m_iconYellow);
        m_layout->addWidget(m_textYellow);
        m_layout->addSpacerItem(m_spacer7);
        m_layout->addWidget(m_iconGreen);
        m_layout->addWidget(m_textGreen);
        m_layout->addSpacerItem(m_spacer8);
        m_iconYellow->setVisible(true);
        m_textYellow->setVisible(true);
        m_iconRed->setVisible(true);
        m_textRed->setVisible(true);
        m_iconBlueRed->setVisible(true);
        m_textBlueRed->setVisible(true);
        m_iconYellowGreen->setVisible(true);
        m_textYellowGreen->setVisible(true);
        m_spacer2->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer3->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer4->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer5->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer6->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
        m_spacer7->changeSize(8, 0, QSizePolicy::MinimumExpanding,
                              QSizePolicy::Ignored);
    }
    m_layout->invalidate();
}

QLabel* ScoreDisplay::createColorIcon(Color c)
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

QLabel* ScoreDisplay::createTwoColorIcon(Color c1, Color c2)
{
    QImage image(32, 16, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor color1 = Util::getPaintColor(game_variant_classic, c1);
    painter.setPen(color1);
    painter.setBrush(color1);
    painter.drawEllipse(7, 4, 8, 8);
    QColor color2 = Util::getPaintColor(game_variant_classic, c2);
    painter.setPen(color2);
    painter.setBrush(color2);
    painter.drawEllipse(17, 4, 8, 8);
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
    removeAllWidgets();
    addWidgets(variant);
}

void ScoreDisplay::removeAllWidgets()
{
    m_layout->removeWidget(m_iconBlue);
    m_layout->removeWidget(m_iconYellow);
    m_layout->removeWidget(m_iconRed);
    m_layout->removeWidget(m_iconGreen);
    m_layout->removeWidget(m_iconBlueRed);
    m_layout->removeWidget(m_iconYellowGreen);
    m_layout->removeWidget(m_textBlue);
    m_layout->removeWidget(m_textYellow);
    m_layout->removeWidget(m_textRed);
    m_layout->removeWidget(m_textGreen);
    m_layout->removeWidget(m_textBlueRed);
    m_layout->removeWidget(m_textYellowGreen);
    m_layout->removeItem(m_spacer1);
    m_layout->removeItem(m_spacer2);
    m_layout->removeItem(m_spacer3);
    m_layout->removeItem(m_spacer4);
    m_layout->removeItem(m_spacer5);
    m_layout->removeItem(m_spacer6);
    m_layout->removeItem(m_spacer7);
    m_layout->removeItem(m_spacer8);
}

void ScoreDisplay::updateScore(const Board& bd)
{
    GameVariant variant = bd.get_game_variant();
    if (variant != m_gameVariant)
        initGameVariant(variant);
    if (variant == game_variant_classic
        || variant == game_variant_classic_2)
    {
        updateScore(m_textBlue,
                    bd.get_points(Color(0)), bd.get_bonus(Color(0)));
        updateScore(m_textYellow,
                    bd.get_points(Color(1)), bd.get_bonus(Color(1)));
        updateScore(m_textRed,
                    bd.get_points(Color(2)), bd.get_bonus(Color(2)));
        updateScore(m_textGreen,
                    bd.get_points(Color(3)), bd.get_bonus(Color(3)));
    }
    if (variant == game_variant_classic_2)
    {
        updateScore(m_textBlueRed,
                    bd.get_points(Color(0)) + bd.get_points(Color(2)),
                    bd.get_bonus(Color(0)) + bd.get_bonus(Color(2)));
        updateScore(m_textYellowGreen,
                    bd.get_points(Color(1)) + bd.get_points(Color(3)),
                    bd.get_bonus(Color(1)) + bd.get_bonus(Color(3)));
    }
    if (variant == game_variant_duo)
    {
        updateScore(m_textBlue,
                    bd.get_points(Color(0)), bd.get_bonus(Color(0)));
        updateScore(m_textGreen,
                    bd.get_points(Color(1)), bd.get_bonus(Color(1)));
    }
}

void ScoreDisplay::updateScore(QLabel* label, unsigned int points,
                               unsigned int bonus)
{
    QString text;
    if (bonus == 0)
        text.setNum(points);
    else
        text = QString("%1 (+%2)").arg(points).arg(bonus);
    label->setText(text);
}

//-----------------------------------------------------------------------------
