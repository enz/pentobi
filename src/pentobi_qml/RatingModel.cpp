//-----------------------------------------------------------------------------
/** @file pentobi_qml/RatingModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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
    Color c(static_cast<Color::IntType>(getNextHumanPlayer()));
    gameModel->getBoard().get_place(c, place, isPlaceShared);
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
    if (rating.get() > m_bestRating.get())
        setBestRating(rating.get());
    setNumberGames(m_numberGames + 1);
    saveSettings();
}

void RatingModel::clearRating()
{
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
    float minDiff = 0; // Initialize to avoid compiler warning
    for (int i = 1; i <= maxLevel; ++i)
    {
        float diff = abs(m_rating.get() - Player::get_rating(variant, i).get());
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
    settings.setValue("rated_games_" + m_gameVariant, m_numberGames);
    // Cast rating to double, if float is used, QSettings saves it as a
    // QVariant in the config files on Linux, which is not human-readable
    // (last tested with Qt 5.8-rc)
    settings.setValue("rating_" + m_gameVariant, static_cast<double>(m_rating.get()));
    settings.setValue("best_rating_" + m_gameVariant, static_cast<double>(m_bestRating.get()));
}

void RatingModel::setBestRating(float rating)
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
    // See comment in saveSettings() why ratings are stored as double, not float
    auto rating = static_cast<float>(settings.value("rating_" + gameVariant, 1000).toDouble());
    auto bestRating = static_cast<float>(settings.value("best_rating_" + gameVariant, 0).toDouble());
    // Use same keys as Pentobi 12.x to be compatible
    setNumberGames(settings.value("rated_games_" + gameVariant, 0).toInt());
    setRating(rating);
    setBestRating(bestRating);
    emit gameVariantChanged();
}

void RatingModel::setInitialRating(float rating)
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

void RatingModel::setRating(float rating)
{
    if (m_rating.get() == rating)
        return;
    m_rating = Rating(rating);
    emit ratingChanged();
}

//-----------------------------------------------------------------------------
