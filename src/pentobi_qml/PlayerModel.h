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
using libpentobi_mcts::Search;
using libpentobi_base::Variant;

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

public:
    /** Global variable to disable opening books. */
    static bool noBook;

    /** Global variable to disable the minimum thinking time. */
    static bool noDelay;

    /** Global variable to set the number of threads the player is constructed
        with.
        The default value 0 means that the number of threads depends on the
        hardware. */
    static unsigned nuThreads;


    explicit PlayerModel(QObject* parent = nullptr);

    ~PlayerModel();


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

    bool isGenMoveRunning() const { return m_isGenMoveRunning; }

    Search& getSearch() { return m_player.get_search(); }

signals:
    void gameVariantChanged();

    void levelChanged();

    void isGenMoveRunningChanged();

    void moveGenerated(GameMove* move);

private:
    struct GenMoveResult
    {
        Color color;

        Move move;

        unsigned genMoveId;

        GameModel* gameModel;
    };


    bool m_isGenMoveRunning = false;

    QString m_gameVariant;

    unsigned m_level = 1;

    unsigned m_genMoveId = 0;

    Player m_player;

    QFutureWatcher<GenMoveResult> m_watcher;


    GenMoveResult asyncGenMove(GameModel* gm, Color c, unsigned genMoveId);

    bool getLevelKey(const QString& gameVariant, QString& key);

    void loadBook(Variant variant);

    void loadLevel(const QString& gameVariant);

    void setIsGenMoveRunning(bool isGenMoveRunning);

private slots:
    void genMoveFinished();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PLAYER_MODEL_H
