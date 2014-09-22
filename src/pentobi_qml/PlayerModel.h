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
    Q_PROPERTY(int levelClassic MEMBER m_levelClassic
               NOTIFY levelClassicChanged)
    Q_PROPERTY(int levelClassic2 MEMBER m_levelClassic2
               NOTIFY levelClassic2Changed)
    Q_PROPERTY(int levelDuo MEMBER m_levelDuo NOTIFY levelDuoChanged)
    Q_PROPERTY(int levelTrigon MEMBER m_levelTrigon
               NOTIFY levelTrigonChanged)
    Q_PROPERTY(int levelTrigon2 MEMBER m_levelTrigon2
               NOTIFY levelTrigon2Changed)
    Q_PROPERTY(int levelTrigon3 MEMBER m_levelTrigon3
               NOTIFY levelTrigon3Changed)
    Q_PROPERTY(int levelJunior MEMBER m_levelJunior NOTIFY levelJuniorChanged)
    Q_PROPERTY(bool isGenMoveRunning MEMBER m_isGenMoveRunning
               NOTIFY isGenMoveRunningChanged)

public:
    explicit PlayerModel(QObject* parent = nullptr);

    ~PlayerModel();

    /** Start a move generation in a background thread.
        The state of the board model may not be changed until the move
        generation was finished (computerPlayed signal) or aborted
        with cancelGenMove() */
    Q_INVOKABLE void startGenMove(BoardModel* boardModel);

    Q_INVOKABLE void startGenMoveAtLevel(BoardModel* boardModel, int level);

    /** Cancel the move generation in the background thread if one is
        running. */
    Q_INVOKABLE void cancelGenMove();

    int isGenMoveRunning() const;

signals:
    void levelClassicChanged(int);

    void levelClassic2Changed(int);

    void levelDuoChanged(int);

    void levelTrigonChanged(int);

    void levelTrigon2Changed(int);

    void levelTrigon3Changed(int);

    void levelJuniorChanged(int);

    void isGenMoveRunningChanged(bool);

    void moveGenerated(int move);

private:
    struct GenMoveResult
    {
        Color color;

        Move move;

        unsigned genMoveId;

        BoardModel* boardModel;
    };

    bool m_isGenMoveRunning;

    int m_levelClassic;

    int m_levelClassic2;

    int m_levelDuo;

    int m_levelTrigon;

    int m_levelTrigon2;

    int m_levelTrigon3;

    int m_levelJunior;

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
