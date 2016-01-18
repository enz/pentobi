//-----------------------------------------------------------------------------
/** @file pentobi_qml/PlayerModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PlayerModel.h"

#include <QElapsedTimer>
#include <QFile>
#include <QtConcurrentRun>
#include <QSettings>

using namespace std;
using libboardgame_util::clear_abort;
using libboardgame_util::set_abort;

//-----------------------------------------------------------------------------

namespace {

unsigned maxLevel = 7;

void getLevel(QSettings& settings, const char* key, unsigned& level)
{
    level = settings.value(key, 1).toUInt();
    if (level < 1)
    {
        qDebug() << "PlayerModel: invalid level in settings:" << level;
        level = 1;
    }
    else if (level > maxLevel)
    {
        qDebug() << "PlayerModel: level in settings too high, using level"
                 << maxLevel;
        level = maxLevel;
    }
}

} // namespace

//-----------------------------------------------------------------------------

PlayerModel::PlayerModel(QObject* parent)
    : QObject(parent),
      m_player(GameModel::getInitialGameVariant(), maxLevel, "")
{
    QSettings settings;
    getLevel(settings, "level_classic", m_levelClassic);
    getLevel(settings, "level_classic_2", m_levelClassic2);
    getLevel(settings, "level_classic_3", m_levelClassic3);
    getLevel(settings, "level_duo", m_levelDuo);
    getLevel(settings, "level_trigon", m_levelTrigon);
    getLevel(settings, "level_trigon_2", m_levelTrigon2);
    getLevel(settings, "level_trigon_3", m_levelTrigon3);
    getLevel(settings, "level_junior", m_levelJunior);
    getLevel(settings, "level_nexos", m_levelNexos);
    getLevel(settings, "level_nexos_2", m_levelNexos2);
    connect(&m_genMoveWatcher, SIGNAL(finished()), SLOT(genMoveFinished()));
}

PlayerModel::~PlayerModel()
{
    cancelGenMove();
    QSettings settings;
    settings.setValue("level_classic", m_levelClassic);
    settings.setValue("level_classic_2", m_levelClassic2);
    settings.setValue("level_classic_3", m_levelClassic3);
    settings.setValue("level_duo", m_levelDuo);
    settings.setValue("level_trigon", m_levelTrigon);
    settings.setValue("level_trigon_2", m_levelTrigon2);
    settings.setValue("level_trigon_3", m_levelTrigon3);
    settings.setValue("level_junior", m_levelJunior);
    settings.setValue("level_nexos", m_levelNexos);
    settings.setValue("level_nexos_2", m_levelNexos2);
}

PlayerModel::GenMoveResult PlayerModel::asyncGenMove(GameModel* gm,
                                                     unsigned genMoveId)
{
    QElapsedTimer timer;
    timer.start();
    auto& bd = gm->getBoard();
    GenMoveResult result;
    result.genMoveId = genMoveId;
    result.gameModel = gm;
    result.move = m_player.genmove(bd, bd.get_effective_to_play());
    auto elapsed = timer.elapsed();
    // Enforce minimum thinking time of 1 sec
    if (elapsed < 1000)
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
    m_genMoveWatcher.waitForFinished();
    setIsGenMoveRunning(false);
}

void PlayerModel::genMoveFinished()
{
    auto result = m_genMoveWatcher.future().result();
    if (result.genMoveId != m_genMoveId)
        // Callback from a canceled move generation
        return;
    setIsGenMoveRunning(false);
    auto& bd = result.gameModel->getBoard();
    auto mv = result.move;
    if (mv.is_null())
    {
        qWarning("PlayerModel: failed to generate move");
        return;
    }
    Color c = bd.get_effective_to_play();
    if (! bd.is_legal(c, mv))
    {
        qWarning("PlayerModel: player generated illegal move");
        return;
    }
    emit moveGenerated(mv.to_int());
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
    m_player.load_book(in);
}

void PlayerModel::setIsGenMoveRunning(bool isGenMoveRunning)
{
    if (m_isGenMoveRunning == isGenMoveRunning)
        return;
    m_isGenMoveRunning = isGenMoveRunning;
    emit isGenMoveRunningChanged(isGenMoveRunning);
}

void PlayerModel::startGenMove(GameModel* gm)
{
    unsigned level;
    switch (gm->getBoard().get_variant())
    {
    case Variant::classic_2:
        level = m_levelClassic2;
        break;
    case Variant::classic_3:
        level = m_levelClassic3;
        break;
    case Variant::duo:
        level = m_levelDuo;
        break;
    case Variant::trigon:
        level = m_levelTrigon;
        break;
    case Variant::trigon_2:
        level = m_levelTrigon2;
        break;
    case Variant::trigon_3:
        level = m_levelTrigon3;
        break;
    case Variant::nexos:
        level = m_levelNexos;
        break;
    case Variant::nexos_2:
        level = m_levelNexos2;
        break;
    default:
        level = m_levelClassic;
    }
    startGenMoveAtLevel(gm, level);
}

void PlayerModel::startGenMoveAtLevel(GameModel* gm, unsigned level)
{
    cancelGenMove();
    m_player.set_level(level);
    auto variant = gm->getBoard().get_variant();
    if (! m_player.is_book_loaded(variant))
        loadBook(variant);
    clear_abort();
    ++m_genMoveId;
    QFuture<GenMoveResult> future =
            QtConcurrent::run(this, &PlayerModel::asyncGenMove, gm,
                              m_genMoveId);
    m_genMoveWatcher.setFuture(future);
    setIsGenMoveRunning(true);
}

//-----------------------------------------------------------------------------
