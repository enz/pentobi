//-----------------------------------------------------------------------------
/** @file pentobi_qml/PlayerModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_PLAYER_MODEL_H
#define PENTOBI_QML_PLAYER_MODEL_H

#include <QFutureWatcher>
#include "BoardModel.h"
#include "libpentobi_mcts/Player.h"

using libpentobi_mcts::Player;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class PlayerModel
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(bool isGenMoveRunning READ isGenMoveRunning
               NOTIFY isGenMoveRunningChanged)

public:
    explicit PlayerModel(QObject* parent = nullptr);

    ~PlayerModel();

    /** Start a move generation in a background thread.
        The state of the board model may not be changed until the move
        generation was finished (computerPlayed signal) or aborted
        with cancelGenMove() */
    Q_INVOKABLE void startGenMove(BoardModel* boardModel);

    /** Cancel the move generation in the background thread if one is
        running. */
    Q_INVOKABLE void cancelGenMove();

    int level() const;

    int isGenMoveRunning() const;

    void setLevel(int level);

signals:
    void levelChanged(int);

    void isGenMoveRunningChanged(bool);

    void computerPlayed();

private:
    struct GenMoveResult
    {
        Move move;

        unsigned genMoveId;

        BoardModel* boardModel;
    };

    bool m_isGenMoveRunning;

    int m_level;

    unsigned m_genMoveId;

    Player m_player;

    QFutureWatcher<GenMoveResult> m_genMoveWatcher;

    GenMoveResult asyncGenMove(BoardModel* bm, unsigned genMoveId);

    void loadBook(Variant variant);

    void setIsGenMoveRunning(bool isGenMoveRunning);

private slots:
    void genMoveFinished();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PLAYER_MODEL_H
