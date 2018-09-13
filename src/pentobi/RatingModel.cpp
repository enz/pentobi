//-----------------------------------------------------------------------------
/** @file pentobi/RatingModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RatingModel.h"

#include <random>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include "GameModel.h"
#include "libpentobi_base/Variant.h"
#include "libpentobi_mcts/Player.h"

using namespace std;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace {

const int maxSavedGames = 50;

} // namespace

//-----------------------------------------------------------------------------

RatedGameInfo::RatedGameInfo(QObject* parent, int number, int color,
                             double result, const QString& date, int level,
                             double rating)
    : QObject(parent),
      m_number(number),
      m_color(color),
      m_level(level),
      m_result(result),
      m_rating(rating),
      m_date(date)
{
}

//-----------------------------------------------------------------------------

void RatingModel::addResult(GameModel* gameModel, int level)
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return;
    unsigned place;
    bool isPlaceShared;
    auto color = getNextHumanPlayer();
    gameModel->getBoard().get_place(Color(static_cast<Color::IntType>(color)),
                                    place, isPlaceShared);
    double gameResult;
    if (place == 0 && ! isPlaceShared)
        gameResult = 1;
    else if (place == 0 && isPlaceShared)
        gameResult = 0.5;
    else
        gameResult = 0;
    auto nuOpponents = static_cast<unsigned>(get_nu_players(variant) - 1);
    Rating opponentRating =
            Player::get_rating(variant, static_cast<unsigned>(level));
    double kValue = (m_numberGames < 30 ? 40 : 20);
    Rating rating = m_rating;
    rating.update(gameResult, opponentRating, kValue, nuOpponents);
    setRating(rating.get());
    auto numberGames = m_numberGames + 1;
    if (numberGames == 1 || rating.get() > m_bestRating.get())
        setBestRating(rating.get());
    auto date = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    m_history.prepend(new RatedGameInfo(this, numberGames, color, gameResult,
                                       date, level, m_rating.get()));
    auto file = getFile(numberGames);
    QFileInfo(file).dir().mkpath(QStringLiteral("."));
    gameModel->save(file);
    emit historyChanged();
    setNumberGames(numberGames);
    saveSettings();
}

void RatingModel::clearRating()
{
    if (! m_history.isEmpty())
    {
        m_history.clear();
        emit historyChanged();
    }
    QDir(getDir()).removeRecursively();
    setRating(1000);
    setBestRating(1000);
    setNumberGames(0);
    saveSettings();
}

QString RatingModel::getDir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
            + "/rated_games/" + m_gameVariant;
}

QString RatingModel::getFile(int gameNumber) const
{
    return getDir() + "/" + QString::number(gameNumber) + ".blksgf";
}

int RatingModel::getGameNumberOfFile(const QString& file) const
{
    QString left = getDir() + QStringLiteral("/");
    if (! file.startsWith(left))
        return 0;
    QString right = QStringLiteral(".blksgf");
    if (! file.endsWith(right))
        return 0;
    auto leftLen = left.length();
    auto rightLen = right.length();
    int n;
    bool ok;
    n = QStringRef(&file, leftLen,
                   file.length() - leftLen - rightLen).toInt(&ok);
    return ok && n >= 1 && n <= m_numberGames ? n : 0;
}

int RatingModel::getNextHumanPlayer() const
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return 0;
    mt19937 generator;
    generator.discard(static_cast<unsigned>(m_numberGames));
    uniform_int_distribution<> distribution(0, get_nu_players(variant) - 1);
    return distribution(generator);
}

int RatingModel::getNextLevel(int maxLevel) const
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return 1;
    int level = 1; // Initialize to avoid compiler warning
    double minDiff = 0; // Initialize to avoid compiler warning
    for (int i = 1; i <= maxLevel; ++i)
    {
        auto diff =
                abs(m_rating.get()
                    - Player::get_rating(variant, static_cast<unsigned>(i)).get());
        if (i == 1 || diff < minDiff)
        {
            minDiff = diff;
            level = i;
        }
    }
    return level;
}

void RatingModel::saveSettings()
{
    QSettings settings;
    if (m_numberGames == 0)
    {
        settings.remove("rated_games_" + m_gameVariant);
        settings.remove("rating_" + m_gameVariant);
        settings.remove("best_rating_" + m_gameVariant);
    }
    else
    {
        settings.setValue("rated_games_" + m_gameVariant, m_numberGames);
        settings.setValue("rating_" + m_gameVariant, m_rating.get());
        settings.setValue("best_rating_" + m_gameVariant,
                          round(m_bestRating.get()));
    }
    QList<QObject*> newHistory;
    newHistory.reserve(m_history.size());
    for (auto& i : m_history)
    {
        auto& info = dynamic_cast<RatedGameInfo&>(*i);
        if (info.number() <= m_numberGames - maxSavedGames)
            QFile::remove(getFile(info.number()));
        else
            newHistory.append(&info);
    }
    if (newHistory.size() != m_history.size())
    {
        m_history = newHistory;
        emit historyChanged();
    }
    settings.remove("rated_game_info_" + m_gameVariant);
    if (m_numberGames > 0)
    {
        settings.beginWriteArray("rated_game_info_" + m_gameVariant);
        int n = 0;
        for (auto& i : m_history)
        {
            auto& info = dynamic_cast<RatedGameInfo&>(*i);
            if (info.number() <= m_numberGames - maxSavedGames)
                continue;
            settings.setArrayIndex(n++);
            settings.setValue(QStringLiteral("number"), info.number());
            settings.setValue(QStringLiteral("color"), info.color());
            settings.setValue(QStringLiteral("result"), info.result());
            settings.setValue(QStringLiteral("date"), info.date());
            settings.setValue(QStringLiteral("level"), info.level());
            settings.setValue(QStringLiteral("rating"), round(info.rating()));
        }
        settings.endArray();
    }
}

void RatingModel::setBestRating(double rating)
{
    m_bestRating = Rating(rating);
    emit bestRatingChanged();
}

void RatingModel::setGameVariant(const QString& gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    m_gameVariant = gameVariant;
    QSettings settings;
    auto currentRating =
            settings.value("rating_" + gameVariant, 1000).toDouble();
    auto bestRating =
            settings.value("best_rating_" + gameVariant, 0).toDouble();
    setRating(currentRating);
    setBestRating(bestRating);
    m_history.clear();
    auto size = settings.beginReadArray("rated_game_info_" + m_gameVariant);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto number = settings.value(QStringLiteral("number")).toInt();
        auto color = settings.value(QStringLiteral("color")).toInt();
        auto result = settings.value(QStringLiteral("result")).toDouble();
        auto date = settings.value(QStringLiteral("date")).toString();
        auto level = settings.value(QStringLiteral("level")).toInt();
        auto rating = settings.value(QStringLiteral("rating")).toDouble();
        m_history.append(new RatedGameInfo(this, number, color, result, date,
                                           level, rating));
    }
    settings.endArray();
    sort(m_history.begin(), m_history.end(),
         [](const QObject* o1, const QObject* o2)
         {
             return dynamic_cast<const RatedGameInfo&>(*o1).number()
                     > dynamic_cast<const RatedGameInfo&>(*o2).number();
         });
    emit historyChanged();
    setNumberGames(settings.value("rated_games_" + gameVariant, 0).toInt());
    emit gameVariantChanged();
}

void RatingModel::setInitialRating(double rating)
{
    setRating(rating);
    setBestRating(rating);
    saveSettings();
}

void RatingModel::setNumberGames(int numberGames)
{
    if (numberGames < 0)
    {
        qWarning("RatingModel: invalid number of games");
        return;
    }
    if (m_numberGames == numberGames)
        return;
    m_numberGames = numberGames;
    emit numberGamesChanged();
}

void RatingModel::setRating(double rating)
{
    m_rating = Rating(rating);
    emit ratingChanged();
}

//-----------------------------------------------------------------------------
