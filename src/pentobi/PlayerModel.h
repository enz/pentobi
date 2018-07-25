//-----------------------------------------------------------------------------
/** @file pentobi/PlayerModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_PLAYER_MODEL_H
#define PENTOBI_PLAYER_MODEL_H

// Needed in the header because moc_*.cpp does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QFutureWatcher>
#include "libpentobi_mcts/Player.h"

class GameModel;
class GameMove;

using namespace std;
using libpentobi_base::Color;
using libpentobi_base::Move;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class PlayerModel
    : public QObject
{
    Q_OBJECT

    /** Game variant should be bound to GameModel.gameVariant.
        This automatically updates the level property to the stored level for
        the current game variant. The level will also be updated on
        startGenMove() but the user interface might want to display the current
        level immediately after changing the game variant. */
    Q_PROPERTY(QString gameVariant READ gameVariant WRITE setGameVariant NOTIFY gameVariantChanged)

    Q_PROPERTY(unsigned level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(bool isGenMoveRunning READ isGenMoveRunning NOTIFY isGenMoveRunningChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused)
    Q_PROPERTY(unsigned maxLevel MEMBER maxLevel CONSTANT)

public:
    /** Global variable to disable opening books.
        Must be set before creating any instances of PlayerModel and not be
        changed afterwards. */
    static bool noBook;

    /** Global variable to disable the minimum thinking time.
        Must be set before creating any instances of PlayerModel and not be
        changed afterwards. */
    static bool noDelay;

    /** Global variable to set the number of threads the player is constructed
        with.
        The default value 0 means that the number of threads depends on the
        hardware. Must be set before creating any instances of PlayerModel and
        not be changed afterwards. */
    static unsigned nuThreads;

    /** Global variable to set the maximum level.
        Must be set before creating any instances of PlayerModel and not be
        changed afterwards. */
    static unsigned maxLevel;


    explicit PlayerModel(QObject* parent = nullptr);

    ~PlayerModel();


    /** Check if the player creation failed because of low memory.
        This is an expected error condition because the player allocates a
        large amount of memory. This function must be checked after object
        creation and no functions may be called if it returns true. */
    Q_INVOKABLE bool notEnoughMemory() const { return m_notEnoughMemory; }

    /** Start a move generation in a background thread.
        The state of the board model may not be changed until the move
        generation was finished (computerPlayed signal) or aborted
        with cancelGenMove() */
    Q_INVOKABLE void startGenMove(GameModel* gameModel);

    /** Cancel the move generation in the background thread if one is
        running. */
    Q_INVOKABLE void cancelGenMove();

    const QString& gameVariant() const { return m_gameVariant; }

    void setGameVariant(const QString& gameVariant);

    unsigned level() const { return m_level; }

    void setLevel(unsigned level);

    unsigned paused() const { return libboardgame_util::get_pause(); }

    void setPaused(bool paused);

    bool isGenMoveRunning() const { return m_isGenMoveRunning; }

    Search& getSearch() { return m_player->get_search(); }

signals:
    void gameVariantChanged();

    void levelChanged();

    void isGenMoveRunningChanged();

    void moveGenerated(GameMove* move);

    void searchCallback(double elapsedSeconds, double remainingSeconds);

private:
    struct GenMoveResult
    {
        Color color;

        Move move;

        unsigned genMoveId;

        GameModel* gameModel;
    };


    bool m_notEnoughMemory;

    bool m_isGenMoveRunning = false;

    QString m_gameVariant;

    unsigned m_level = 1;

    unsigned m_genMoveId = 0;

    unique_ptr<Player> m_player;

    QFutureWatcher<GenMoveResult> m_watcher;


    GenMoveResult asyncGenMove(GameModel* gm, Color c, unsigned genMoveId);

    void genMoveFinished();

    void loadBook(Variant variant);

    void setIsGenMoveRunning(bool isGenMoveRunning);

};

//-----------------------------------------------------------------------------

#endif // PENTOBI_PLAYER_MODEL_H
