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
    Q_PROPERTY(unsigned levelClassic MEMBER m_levelClassic
               NOTIFY levelClassicChanged)
    Q_PROPERTY(unsigned levelClassic2 MEMBER m_levelClassic2
               NOTIFY levelClassic2Changed)
    Q_PROPERTY(unsigned levelClassic3 MEMBER m_levelClassic3
               NOTIFY levelClassic3Changed)
    Q_PROPERTY(unsigned levelDuo MEMBER m_levelDuo NOTIFY levelDuoChanged)
    Q_PROPERTY(unsigned levelTrigon MEMBER m_levelTrigon
               NOTIFY levelTrigonChanged)
    Q_PROPERTY(unsigned levelTrigon2 MEMBER m_levelTrigon2
               NOTIFY levelTrigon2Changed)
    Q_PROPERTY(unsigned levelTrigon3 MEMBER m_levelTrigon3
               NOTIFY levelTrigon3Changed)
    Q_PROPERTY(unsigned levelJunior MEMBER m_levelJunior
               NOTIFY levelJuniorChanged)
    Q_PROPERTY(unsigned levelNexos MEMBER m_levelNexos NOTIFY
               levelNexosChanged)
    Q_PROPERTY(unsigned levelNexos2 MEMBER m_levelNexos2 NOTIFY
               levelNexos2Changed)
    Q_PROPERTY(unsigned levelCallisto MEMBER m_levelCallisto
               NOTIFY levelCallistoChanged)
    Q_PROPERTY(unsigned levelCallisto2 MEMBER m_levelCallisto2
               NOTIFY levelCallisto2Changed)
    Q_PROPERTY(unsigned levelCallisto3 MEMBER m_levelCallisto3
               NOTIFY levelCallisto3Changed)
    Q_PROPERTY(bool isGenMoveRunning MEMBER m_isGenMoveRunning
               NOTIFY isGenMoveRunningChanged)

public:
    /** Global variable to disable opening books. */
    static bool no_book;

    /** Global variable to disable the minimum thinking time. */
    static bool no_delay;

    /** Global variable to set the number of threads the player is constructed
        with.
        The default value 0 means that the number of threads depends on the
        hardware. */
    static unsigned nu_threads;


    explicit PlayerModel(QObject* parent = nullptr);

    ~PlayerModel();

    /** Start a move generation in a background thread.
        The state of the board model may not be changed until the move
        generation was finished (computerPlayed signal) or aborted
        with cancelGenMove() */
    Q_INVOKABLE void startGenMove(GameModel* gameModel);

    Q_INVOKABLE void startGenMoveAtLevel(GameModel* gameModel, unsigned level);

    /** Cancel the move generation in the background thread if one is
        running. */
    Q_INVOKABLE void cancelGenMove();

signals:
    void levelCallistoChanged(unsigned);

    void levelCallisto2Changed(unsigned);

    void levelCallisto3Changed(unsigned);

    void levelClassicChanged(unsigned);

    void levelClassic2Changed(unsigned);

    void levelClassic3Changed(unsigned);

    void levelDuoChanged(unsigned);

    void levelTrigonChanged(unsigned);

    void levelTrigon2Changed(unsigned);

    void levelTrigon3Changed(unsigned);

    void levelJuniorChanged(unsigned);

    void levelNexosChanged(unsigned);

    void levelNexos2Changed(unsigned);

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

    unsigned m_levelCallisto;

    unsigned m_levelCallisto2;

    unsigned m_levelCallisto3;

    unsigned m_levelClassic;

    unsigned m_levelClassic2;

    unsigned m_levelClassic3;

    unsigned m_levelDuo;

    unsigned m_levelTrigon;

    unsigned m_levelTrigon2;

    unsigned m_levelTrigon3;

    unsigned m_levelJunior;

    unsigned m_levelNexos;

    unsigned m_levelNexos2;

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
