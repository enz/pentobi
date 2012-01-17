//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_GAME_WINDOW_H
#define PENTOBI_ANALYZE_GAME_WINDOW_H

#include <vector>
#include <QtGui>
#include "libpentobi_mcts/AnalyzeGame.h"

using namespace std;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::GameVariant;
using libpentobi_mcts::AnalyzeGame;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class AnalyzeGameWindow
    : public QDialog
{
    Q_OBJECT

public:
    AnalyzeGameWindow(QWidget* parent, Game& game, Search& search);

signals:
    void gotoPosition(GameVariant gameVariant, const vector<ColorMove>& moves);

protected:
    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

    void resizeEvent(QResizeEvent* event);

private:
    unsigned int m_maxMoves;

    AnalyzeGame m_analyzeGame;

    QProgressDialog* m_progressDialog;

    int m_borderX;

    int m_borderY;

    qreal m_dX;

    int m_maxX;

    int m_maxY;

    void progressCallback(unsigned int movesAnalyzed, unsigned int totalMoves);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_GAME_WINDOW_H
