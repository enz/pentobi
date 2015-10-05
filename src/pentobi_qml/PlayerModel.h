//-----------------------------------------------------------------------------
/** @file pentobi_qml/PlayerModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_PLAYER_MODEL_H
#define PENTOBI_QML_PLAYER_MODEL_H

#include <QFutureWatcher>
#include "GameModel.h"
#include "libpentobi_mcts/Player.h"

using namespace std;
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
    Q_INVOKABLE void startGenMove(GameModel* gameModel);

    Q_INVOKABLE void startGenMoveAtLevel(GameModel* gameModel, int level);

    /** Cancel the move generation in the background thread if one is
        running. */
    Q_INVOKABLE void cancelGenMove();

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

        GameModel* gameModel;
    };

    bool m_isGenMoveRunning = false;

    int m_levelClassic;

    int m_levelClassic2;

    int m_levelDuo;

    int m_levelTrigon;

    int m_levelTrigon2;

    int m_levelTrigon3;

    int m_levelJunior;

    unsigned m_genMoveId = 0;

    Player m_player;

    QFutureWatcher<GenMoveResult> m_genMoveWatcher;

    GenMoveResult asyncGenMove(GameModel* gm, unsigned genMoveId);

    void loadBook(Variant variant);

    void setIsGenMoveRunning(bool isGenMoveRunning);

private slots:
    void genMoveFinished();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PLAYER_MODEL_H
