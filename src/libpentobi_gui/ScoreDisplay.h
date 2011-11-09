//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_SCORE_DISPLAY_H
#define LIBPENTOBI_GUI_SCORE_DISPLAY_H

#include <QtGui>
#include "libpentobi_base/Board.h"

using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

class ScoreDisplay
    : public QWidget
{
    Q_OBJECT

public:
    ScoreDisplay(QWidget* parent = 0);

    QSize sizeHint() const;

    void updateScore(const Board& bd);

protected:
    void paintEvent(QPaintEvent* event);

private:
    GameVariant m_gameVariant;

    ColorMap<unsigned int> m_points;

    ColorMap<unsigned int> m_bonus;

    QString getScoreText(Color c);

    QString getScoreText2(Color c1, Color c2);

    int getScoreTextWidth(Color c);

    int getScoreTextWidth2(Color c1, Color c2);

    void drawScore(QPainter& painter, Color c, int x);

    void drawScore2(QPainter& painter, Color c1, Color c2, int x);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_SCORE_DISPLAY_H
