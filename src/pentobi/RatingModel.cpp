//-----------------------------------------------------------------------------
/** @file pentobi/RatingModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingModel.h"

#include <random>
#include <QSettings>
#include "GameModel.h"
#include "libpentobi_base/Variant.h"
#include "libpentobi_mcts/Player.h"

using namespace std;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

RatedGameInfo::RatedGameInfo(QObject* parent, int number, int color,
                             double result, const QString& date, int level,
                             double rating, const QByteArray& sgf)
    : QObject(parent),
      m_number(number),
      m_color(color),
      m_level(level),
      m_result(result),
      m_rating(rating),
      m_date(date),
      m_sgf(sgf)
{
}

//-----------------------------------------------------------------------------

RatingModel::RatingModel(QObject* parent)
    : QObject(parent)
{
}

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
    float gameResult;
    if (place == 0 && ! isPlaceShared)
        gameResult = 1;
    else if (place == 0 && isPlaceShared)
        gameResult = 0.5;
    else
        gameResult = 0;
    auto nuOpponents = get_nu_players(variant) - 1;
    Rating opponentRating = Player::get_rating(variant, level);
    float kValue = (m_numberGames < 30 ? 40.f : 20.f);
    Rating rating = m_rating;
    rating.update(gameResult, opponentRating, kValue, nuOpponents);
    setRating(rating.get());
    auto numberGames = m_numberGames + 1;
    if (numberGames == 1 || rating.get() > m_bestRating.get())
        setBestRating(rating.get());
    auto date = QDate::currentDate().toString("yyyy-MM-dd");
    m_history.prepend(new RatedGameInfo(this, numberGames, color, gameResult,
                                       date, level, m_rating.get(),
                                       gameModel->getSgf()));
    emit historyChanged();
    // Call setNumberGames() after m_history was updated because
    // RatingDialog.qml calls resizeColumnsToContents() if numberGames changes.
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
    setRating(1000);
    setBestRating(1000);
    setNumberGames(0);
    saveSettings();
}

int RatingModel::getNextHumanPlayer() const
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return 0;
    mt19937 generator;
    generator.discard(m_numberGames);
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
        auto diff = abs(m_rating.get() - Player::get_rating(variant, i).get());
        if (i == 1 || diff < minDiff)
        {
            minDiff = diff;
            level = i;
        }
    }
    return level;
}

void RatingModel::saveSettings() const
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
        settings.setValue("best_rating_" + m_gameVariant, m_bestRating.get());
    }

    // Don't store games earlier than the last 50 games
    const int maxSavedGames = 50;
    settings.remove("rated_game_info_" + m_gameVariant);
    settings.beginWriteArray("rated_game_info_" + m_gameVariant);
    int n = 0;
    for (auto& i : m_history)
    {
        auto& info = dynamic_cast<RatedGameInfo&>(*i);
        if (info.number() >= m_numberGames - maxSavedGames)
        {
            ++n;
            break;
        }
    }
    if (n == 0)
        return;
    n = 0;
    for (auto& i : m_history)
    {
        auto& info = dynamic_cast<RatedGameInfo&>(*i);
        if (info.number() < m_numberGames - maxSavedGames)
            continue;
        settings.setArrayIndex(n++);
        settings.setValue("number", info.number());
        settings.setValue("color", info.color());
        settings.setValue("result", info.result());
        settings.setValue("date", info.date());
        settings.setValue("level", info.level());
        settings.setValue("rating", info.rating());
        settings.setValue("sgf", info.sgf());
    }
    settings.endArray();
}

void RatingModel::setBestRating(double rating)
{
    if (m_bestRating.get() == rating)
        return;
    m_bestRating = Rating(rating);
    emit bestRatingChanged();
}

void RatingModel::setGameVariant(const QString& gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    m_gameVariant = gameVariant;
    QSettings settings;
    auto rating = settings.value("rating_" + gameVariant, 1000).toDouble();
    auto bestRating = settings.value("best_rating_" + gameVariant, 0).toDouble();

    // Use same keys as Pentobi 12.x to be compatible
    setRating(rating);
    setBestRating(bestRating);
    m_history.clear();
    auto size = settings.beginReadArray("rated_game_info_" + m_gameVariant);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto number = settings.value("number").toInt();
        auto color = settings.value("color").toInt();
        auto result = settings.value("result").toDouble();
        auto date = settings.value("date").toString();
        auto level = settings.value("level").toInt();
        auto rating = settings.value("rating").toDouble();
        auto sgf = settings.value("sgf").toByteArray();
        m_history.append(new RatedGameInfo(this, number, color, result, date,
                                           level, rating, sgf));
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
    if (m_rating.get() == rating)
        return;
    m_rating = Rating(rating);
    emit ratingChanged();
}

//-----------------------------------------------------------------------------
