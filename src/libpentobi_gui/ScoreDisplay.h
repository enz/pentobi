//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_SCORE_DISPLAY_H
#define LIBPENTOBI_GUI_SCORE_DISPLAY_H

#include <QtGui>
#include "libpentobi_base/Board.h"

using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

class ScoreDisplay
    : public QWidget
{
    Q_OBJECT

public:
    ScoreDisplay(QWidget* parent = 0);

    void updateScore(const Board& bd);

private:
    GameVariant m_gameVariant;

    QBoxLayout* m_layout;

    QLabel* m_iconBlue;

    QLabel* m_iconYellow;

    QLabel* m_iconRed;

    QLabel* m_iconGreen;

    QLabel* m_iconBlueRed;

    QLabel* m_iconYellowGreen;

    QLabel* m_textBlue;

    QLabel* m_textYellow;

    QLabel* m_textRed;

    QLabel* m_textGreen;

    QLabel* m_textBlueRed;

    QLabel* m_textYellowGreen;

    QSpacerItem* m_spacer1;

    QSpacerItem* m_spacer2;

    QSpacerItem* m_spacer3;

    QSpacerItem* m_spacer4;

    QSpacerItem* m_spacer5;

    QSpacerItem* m_spacer6;

    QSpacerItem* m_spacer7;

    QSpacerItem* m_spacer8;

    void addWidgets(GameVariant variant);

    QLabel* createColorIcon(Color c);

    QLabel* createTwoColorIcon(Color c1, Color c2);

    void initGameVariant(GameVariant variant);

    void removeAllWidgets();

    void updateScore(QLabel* label, unsigned int points, unsigned int bonus);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_SCORE_DISPLAY_H
