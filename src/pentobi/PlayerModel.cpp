//-----------------------------------------------------------------------------
/** @file pentobi/PlayerModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PlayerModel.h"

#include <QElapsedTimer>
#include <QFile>
#include <QtConcurrentRun>
#include <QSettings>
#include "GameModel.h"

using namespace std;
using libboardgame_util::clear_abort;
using libboardgame_util::set_abort;

//-----------------------------------------------------------------------------

bool PlayerModel::noBook = false;

bool PlayerModel::noDelay = false;

unsigned PlayerModel::nuThreads = 0;

#ifdef Q_OS_ANDROID
unsigned PlayerModel::maxLevel = 7;
#else
unsigned PlayerModel::maxLevel = 9;
#endif

PlayerModel::PlayerModel(QObject* parent)
    : QObject(parent)
{
    try
    {
        m_player = make_unique<Player>(GameModel::getInitialGameVariant(),
                                       maxLevel, "", nuThreads);
        m_notEnoughMemory = false;
    }
    catch (const bad_alloc&)
    {
        m_notEnoughMemory = true;
        return;
    }
    if (noBook)
        m_player->set_use_book(false);
    m_player->get_search().set_callback(
                [this](double elapsedSeconds, double remainingSeconds) {
        emit searchCallback(elapsedSeconds, remainingSeconds);
    });
    connect(&m_watcher, &QFutureWatcher<GenMoveResult>::finished,
            this, &PlayerModel::genMoveFinished);
}

PlayerModel::~PlayerModel()
{
    cancelGenMove();
}

PlayerModel::GenMoveResult PlayerModel::asyncGenMove(
        GameModel* gm, Color c, unsigned genMoveId)
{
    QElapsedTimer timer;
    timer.start();
    auto& bd = gm->getBoard();
    GenMoveResult result;
    result.color = c;
    result.genMoveId = genMoveId;
    result.gameModel = gm;
    result.move = m_player->genmove(bd, bd.get_effective_to_play());
    auto elapsed = timer.elapsed();
    // Enforce minimum thinking time of 1 sec
    if (elapsed < 1000 && ! noDelay)
        QThread::msleep(1000 - elapsed);
    return result;
}

void PlayerModel::cancelGenMove()
{
    if (! m_isGenMoveRunning)
        return;
    // After waitForFinished() returns, we can be sure that the move generation
    // is no longer running, but we will still receive the finished event.
    // Increasing m_genMoveId will make genMoveFinished() ignore the event.
    ++m_genMoveId;
    set_abort();
    m_watcher.waitForFinished();
    setIsGenMoveRunning(false);
}

void PlayerModel::genMoveFinished()
{
    auto result = m_watcher.future().result();
    if (result.genMoveId != m_genMoveId)
        // Callback from a canceled move generation
        return;
    setIsGenMoveRunning(false);
    auto& bd = result.gameModel->getBoard();
    ColorMove mv(result.color, result.move);
    if (mv.is_null())
    {
        qWarning("PlayerModel: failed to generate move");
        return;
    }
    if (! bd.is_legal(mv.color, mv.move))
    {
        qWarning("PlayerModel: player generated illegal move");
        return;
    }
    emit moveGenerated(new GameMove(this, mv));
}

void PlayerModel::loadBook(Variant variant)
{
    QFile file(QString(":/pentobi_books/book_%1.blksgf")
               .arg(to_string_id(variant)));
    if (! file.open(QIODevice::ReadOnly))
    {
        qWarning() << "PlayerModel: could not open " << file.fileName();
        return;
    }
    QTextStream stream(&file);
    QString text = stream.readAll();
    istringstream in(text.toLocal8Bit().constData());
    m_player->load_book(in);
}

bool PlayerModel::getLevelKey(const QString& gameVariant, QString& key)
{
    Variant variant;
    if (! parse_variant_id(gameVariant.toLocal8Bit().constData(), variant))
    {
        qWarning("PlayerModel: invalid game variant");
        return false;
    }
    key = QString("level_%1").arg(to_string_id(variant));
    return true;
}

void PlayerModel::loadLevel(const QString& gameVariant)
{
    QString key;
    if (! getLevelKey(gameVariant, key))
        return;
    QSettings settings;
    auto level = settings.value(key, 1).toUInt();
    if (level < 1)
    {
        qDebug() << "PlayerModel: invalid level in settings:" << level;
        level = 1;
    }
    else if (level > maxLevel)
    {
        qDebug() << "PlayerModel: level in settings too high, using" << maxLevel;
        level = maxLevel;
    }
    if (m_level != level)
    {
        m_level = level;
        emit levelChanged();
    }
}

void PlayerModel::setGameVariant(const QString& gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    loadLevel(gameVariant);
    m_gameVariant = gameVariant;
    emit gameVariantChanged();
}

void PlayerModel::setIsGenMoveRunning(bool isGenMoveRunning)
{
    if (m_isGenMoveRunning == isGenMoveRunning)
        return;
    m_isGenMoveRunning = isGenMoveRunning;
    emit isGenMoveRunningChanged();
}

void PlayerModel::setLevel(unsigned level)
{
    if (m_level == level)
        return;
    QString key;
    if (! getLevelKey(m_gameVariant, key))
        return;
    {
        QSettings settings;
        settings.setValue(key, level);
    }
    m_level = level;
    emit levelChanged();
}

void PlayerModel::startGenMove(GameModel* gm)
{
    auto& bd = gm->getBoard();
    if (gm->gameVariant() != m_gameVariant)
        loadLevel(gm->gameVariant());
    cancelGenMove();
    m_player->set_level(min(m_level, maxLevel));
    auto variant = gm->getBoard().get_variant();
    if (! m_player->is_book_loaded(variant))
        loadBook(variant);
    clear_abort();
    ++m_genMoveId;
    QFuture<GenMoveResult> future =
            QtConcurrent::run(this, &PlayerModel::asyncGenMove, gm,
                              bd.get_effective_to_play(), m_genMoveId);
    m_watcher.setFuture(future);
    setIsGenMoveRunning(true);
}

//-----------------------------------------------------------------------------
