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

public slots:
    /** Cancel a running analysis.
        The function waits for the analysis to finish. The finished() signal
        will still be invoked. */
    void cancel();

public:
    AnalyzeGameWindow(QWidget* parent);

    /** Start an analysis.
        This function will return after the analysis has started but the
        window will be protected by a modal cancelable progress dialog.
        Don't modify the game or use the search from a different thread until
        the signal finished() was emitted. This will walk through every game
        position in the main variation and use the search to evaluate
        positions. During the analysis, the parent window is protected with a
        modal progress dialog. */
    void start(const Game& game, Search& search);

    /** Mark the current position.
        Will clear the current position if the target node is not in the
        main variation or does not correspond to a move in the move
        sequence when the analysis was done. */
    void setCurrentPosition(const Game& game, const Node& node);

    QSize sizeHint() const;

signals:
    /** Tells that the analysis has finished. */
    void finished();

    void gotoPosition(GameVariant gameVariant, const vector<ColorMove>& moves);

protected:
    void mousePressEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

    void resizeEvent(QResizeEvent* event);

private slots:
    void showProgress(int progress);

private:
    bool m_isInitialized;

    bool m_isRunning;

    const Game* m_game;

    Search* m_search;

    AnalyzeGame m_analyzeGame;

    QProgressDialog* m_progressDialog;

    QFuture<void> m_future;

    int m_borderX;

    int m_borderY;

    qreal m_dX;

    int m_maxX;

    int m_maxY;

    /** Current position that will be marked or -1 if no position is marked. */
    int m_currentPosition;

    void initSize();

    void progressCallback(unsigned int movesAnalyzed, unsigned int totalMoves);

    void threadFunction();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_GAME_WINDOW_H
