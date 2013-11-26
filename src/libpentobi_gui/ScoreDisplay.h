//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ScoreDisplay.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_SCORE_DISPLAY_H
#define LIBPENTOBI_GUI_SCORE_DISPLAY_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QWidget>
#include "libpentobi_base/Board.h"

using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class ScoreDisplay
    : public QWidget
{
    Q_OBJECT

public:
    ScoreDisplay(QWidget* parent = nullptr);

    void updateScore(const Board& bd);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    int m_fontSize;

    QFont m_font;

    Variant m_variant;

    ColorMap<bool> m_hasMoves;

    ColorMap<unsigned> m_points;

    ColorMap<unsigned> m_bonus;

    int m_colorDotSize;

    int m_colorDotSpace;

    int m_colorDotWidth;

    int m_twoColorDotWidth;

    QString getScoreText(Color c);

    QString getScoreText2(Color c1, Color c2);

    int getScoreTextWidth(Color c);

    int getScoreTextWidth2(Color c1, Color c2);

    void drawScore(QPainter& painter, Color c, int x);

    void drawScore2(QPainter& painter, Color c1, Color c2, int x);

    QString getScoreText(unsigned points, unsigned bonus) const;

    int getTextWidth(QString text) const;

    void drawText(QPainter& painter, const QString& text, int x, int y,
                  bool underline);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_SCORE_DISPLAY_H
