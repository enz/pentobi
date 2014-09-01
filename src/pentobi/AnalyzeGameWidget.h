//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWidget.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_GAME_WIDGET_H
#define PENTOBI_ANALYZE_GAME_WIDGET_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
#include <QFuture>
#include <QWidget>
#include "libpentobi_mcts/AnalyzeGame.h"

class QProgressDialog;

using namespace std;
using libboardgame_sgf::SgfNode;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::Variant;
using libpentobi_mcts::AnalyzeGame;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class AnalyzeGameWidget
    : public QWidget
{
    Q_OBJECT

public slots:
    /** Cancel a running analysis.
        The function waits for the analysis to finish. The finished() signal
        will still be invoked. */
    void cancel();

public:
    AnalyzeGameWidget(QWidget* parent);

    /** Start an analysis.
        This function will return after the analysis has started but the
        window will be protected by a modal cancelable progress dialog.
        Don't modify the game or use the search from a different thread until
        the signal finished() was emitted. This will walk through every game
        position in the main variation and use the search to evaluate
        positions. During the analysis, the parent window is protected with a
        modal progress dialog. */
    void start(const Game& game, Search& search, size_t nuSimulations);

    /** Mark the current position.
        Will clear the current position if the target node is not in the
        main variation or does not correspond to a move in the move
        sequence when the analysis was done. */
    void setCurrentPosition(const Game& game, const SgfNode& node);

    QSize sizeHint() const;

signals:
    /** Tells that the analysis has finished. */
    void finished();

    void gotoPosition(Variant variant, const vector<ColorMove>& moves);

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private slots:
    void showProgress(int progress);

private:
    bool m_isInitialized;

    bool m_isRunning;

    const Game* m_game;

    Search* m_search;

    size_t m_nuSimulations;

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

    void threadFunction();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_GAME_WIDGET_H
