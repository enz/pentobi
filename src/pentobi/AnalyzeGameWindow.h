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
using libpentobi_base::Node;
using libpentobi_mcts::AnalyzeGame;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class AnalyzeGameWindow
    : public QDialog
{
    Q_OBJECT

public:
    AnalyzeGameWindow(QWidget* parent);

    /** Run the analysis.
        This will temporarily change the current position in the game and use
        the search to evaluate positions. During the analysis, the parent
        window is protected with a modal progress dialog. */
    void init(Game& game, Search& search);

    /** Mark the current position.
        Will clear the current position if the target node is not in the
        main variation or does not correspond to a move in the move
        sequence when the analysis was done. */
    void setCurrentPosition(const Game& game, const Node& node);

    QSize sizeHint() const;

signals:
    void gotoPosition(GameVariant gameVariant, const vector<ColorMove>& moves);

protected:
    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

    void resizeEvent(QResizeEvent* event);

private:
    bool m_isInitialized;

    AnalyzeGame m_analyzeGame;

    QProgressDialog* m_progressDialog;

    int m_borderX;

    int m_borderY;

    qreal m_dX;

    int m_maxX;

    int m_maxY;

    /** Current position that will be marked or -1 if no position is marked. */
    int m_currentPosition;

    void initSize();

    void progressCallback(unsigned int movesAnalyzed, unsigned int totalMoves);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_GAME_WINDOW_H
