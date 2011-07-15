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

    QLabel* m_textBlue;

    QLabel* m_textYellow;

    QLabel* m_textRed;

    QLabel* m_textGreen;

    QSpacerItem* m_spacer1;

    QSpacerItem* m_spacer2;

    QSpacerItem* m_spacer3;

    QSpacerItem* m_spacer4;

    QSpacerItem* m_spacer5;

    QLabel* createIcon(Color c);

    void initGameVariant(GameVariant variant);

    void updateScore(QLabel* label, const Board& bd, Color c);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_SCORE_DISPLAY_H
