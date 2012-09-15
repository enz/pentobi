//-----------------------------------------------------------------------------
/** @file pentobi/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <QUrl>
#include "libpentobi_mcts/Player.h"

using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace {

void saveRating(Variant variant, Rating rating, unsigned int nuGames,
                Rating bestRating)
{
    QString variantStr = QString(to_string_id(variant));
    QSettings settings;
    settings.setValue("rated_games_" + variantStr, nuGames);
    settings.setValue("rating_" + variantStr,
                      static_cast<double>(rating.get()));
    settings.setValue("best_rating_" + variantStr,
                      static_cast<double>(bestRating.get()));
}

} // namespace

//-----------------------------------------------------------------------------

namespace Util
{

void getNextRatedGameSettings(Variant variant, int maxLevel, int& level,
                              Color& userColor)
{
    Rating rating;
    Rating bestRating;
    unsigned int nuGames;
    getRating(variant, rating, nuGames, bestRating);
    userColor = Color(nuGames % get_nu_players(variant));
    float minDiff = 0; // Initialize to avoid compiler warning
    for (int i = 1; i <= maxLevel; ++i)
    {
        float diff = abs(rating.get() - Player::get_rating(variant, i).get());
        if (i == 1 || diff < minDiff)
        {
            minDiff = diff;
            level = i;
        }
    }
}

void fixRating(const RatingHistory& history, Rating& bestRating)
{
    BOOST_FOREACH(const RatingHistory::GameInfo& info, history.get())
        if (info.rating.get() > bestRating.get())
            bestRating = info.rating;
}

void getRating(Variant variant, Rating& rating, unsigned int& nuGames,
               Rating& bestRating)
{
    QString variantStr = QString(to_string_id(variant));
    QSettings settings;
    nuGames = settings.value("rated_games_" + variantStr, 0).toUInt();
    // Default value is 1000 (Elo-rating for beginner-level play)
    rating = Rating(settings.value("rating_" + variantStr, 1000).toFloat());
    bestRating = Rating(settings.value("best_rating_" + variantStr, 0).toFloat());
}

void initRating(Variant variant, Rating rating)
{
    saveRating(variant, rating, 0, Rating(0));
}

void removeThumbnail(const QString& file)
{
    // Note: in the future, it might be possible to trigger a thumbnail
    // update via D-Bus instead of removing it, but this is not yet
    // implemented in Gnome
    QFileInfo info(file);
    QString canonicalFile = info.canonicalFilePath();
    if (canonicalFile.isEmpty())
        canonicalFile = info.absoluteFilePath();
    QByteArray url = QUrl::fromLocalFile(canonicalFile).toEncoded();
    QByteArray md5 =
        QCryptographicHash::hash(url, QCryptographicHash::Md5).toHex();
    QString home = QDir::home().path();
    QFile::remove(home + "/.thumbnails/normal/" + md5 + ".png");
    QFile::remove(home + "/.thumbnails/large/" + md5 + ".png");
}

void updateRating(Variant variant, float score, Rating opponentRating,
                  unsigned int nuOpponents)
{
    Rating rating;
    Rating bestRating;
    unsigned int nuGames;
    getRating(variant, rating, nuGames, bestRating);
    rating.update_multiplayer(score, opponentRating, nuOpponents, 32);
    if (rating.get() > bestRating.get())
        bestRating = rating;
    saveRating(variant, rating, nuGames + 1, bestRating);
}

} // namespace Util

//-----------------------------------------------------------------------------
