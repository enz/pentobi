//-----------------------------------------------------------------------------
/** @file pentobi/RatingHistory.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingHistory.h"

#include <fstream>
#include <sstream>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QString>
#include "Util.h"
#include "libpentobi_base/TreeWriter.h"
#include "libpentobi_mcts/Player.h"

using libpentobi_base::to_string_id;
using libpentobi_base::TreeWriter;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace {

/** 1000 Elo represents a beginner level. */
const double startRating = 1000;

QString getRatedGamesDir(Variant variant)
{
    return
        Util::getDataDir() + "/rated_games/" + QString(to_string_id(variant));
}

} // namespace

//-----------------------------------------------------------------------------

RatingHistory::RatingHistory(Variant variant)
{
    load(variant);
}

void RatingHistory::addGame(float score, Rating opponentRating,
                            unsigned nuOpponents, Color color,
                            float result, const QString& date, int level,
                            const Tree& tree)
{
    m_rating.update_multiplayer(score, opponentRating, nuOpponents, 32);
    if (m_rating.get() > m_bestRating.get())
        m_bestRating = m_rating;
    ++m_nuGames;
    GameInfo info;
    info.number = m_nuGames;
    info.color = color;
    info.result = result;
    info.date = date;
    info.level = level;
    info.rating = m_rating;
    m_games.push_back(info);
    size_t nuGames = m_games.size();
    if (nuGames > maxGames)
        m_games.erase(m_games.begin(), m_games.begin() + nuGames - maxGames);
    save();
    ofstream out(getFile(m_nuGames).toLocal8Bit().constData());
    TreeWriter writer(out, tree);
    writer.set_one_prop_per_line(true);
    writer.set_one_prop_value_per_line(true);
    writer.set_indent(2);
    writer.write();
    // Only save the last RatingHistory::maxGames games
    if (m_nuGames > maxGames)
        QFile::remove(getFile(m_nuGames - maxGames));
}

void RatingHistory::clear()
{
    QString variantStr = QString(to_string_id(m_variant));
    QSettings settings;
    settings.remove("rated_games_" + variantStr);
    settings.remove("rating_" + variantStr);
    settings.remove("best_rating_" + variantStr);
    for (const RatingHistory::GameInfo& info : getGameInfos())
        QFile::remove(getFile(info.number));
    QFile::remove(m_file);
    m_nuGames = 0;
    m_rating = Rating(startRating);
    m_bestRating = Rating(startRating);
    m_games.clear();
}

QString RatingHistory::getFile(unsigned n) const
{
    return QString("%1/%2.blksgf").arg(m_dir).arg(n);
}

void RatingHistory::getNextRatedGameSettings(int maxLevel, unsigned random,
                                             int& level, Color& color)
{
    color =
        Color(static_cast<Color::IntType>(random % get_nu_players(m_variant)));
    float minDiff = 0; // Initialize to avoid compiler warning
    for (int i = 1; i <= maxLevel; ++i)
    {
        float diff =
            abs(m_rating.get() - Player::get_rating(m_variant, i).get());
        if (i == 1 || diff < minDiff)
        {
            minDiff = diff;
            level = i;
        }
    }
}

void RatingHistory::init(Rating rating)
{
    m_rating = rating;
    m_bestRating = rating;
    m_nuGames = 0;
    m_games.clear();
    save();
}

void RatingHistory::load(Variant variant)
{
    m_variant = variant;
    QString variantStr = QString(to_string_id(variant));
    QSettings settings;
    m_nuGames = settings.value("rated_games_" + variantStr, 0).toUInt();
    // Default value is 1000 (Elo-rating for beginner-level play)
    m_rating =
        Rating(settings.value("rating_" + variantStr, startRating).toFloat());
    m_bestRating =
        Rating(settings.value("best_rating_" + variantStr, 0).toFloat());
    m_games.clear();
    m_dir = getRatedGamesDir(variant);
    m_file = m_dir + "/history.dat";
    ifstream file(m_file.toLocal8Bit().constData());
    if (! file)
        return;
    string line;
    while (getline(file, line) && m_games.size() < maxGames)
    {
        istringstream in(line);
        GameInfo info;
        unsigned c;
        string date;
        in >> info.number >> c >> info.result >> date >> info.level
           >> info.rating;
        info.date = QString(date.c_str());
        if (! in || c >= get_nu_colors(variant))
            return;
        info.color = Color(static_cast<Color::IntType>(c));
        if (info.number >= 1 && info.number <= m_nuGames)
            m_games.push_back(info);
    }
    size_t nuGames = m_games.size();
    if (nuGames > maxGames)
        m_games.erase(m_games.begin(), m_games.begin() + nuGames - maxGames);
    // Make the all-time best rating consistent with the rating history. Older
    // versions of Pentobi (up to version 3) did not save the all-time best
    // rating, so after an upgrade to a newer version of Pentobi, the history
    // of recent rated games can contain a higher rating than the stored
    // all-time best rating.
    for (const RatingHistory::GameInfo& info : getGameInfos())
        if (info.rating.get() > m_bestRating.get())
            m_bestRating = info.rating;
}

void RatingHistory::save() const
{
    QString variantStr = QString(to_string_id(m_variant));
    QSettings settings;
    settings.setValue("rated_games_" + variantStr, m_nuGames);
    settings.setValue("rating_" + variantStr,
                      static_cast<double>(m_rating.get()));
    settings.setValue("best_rating_" + variantStr,
                      static_cast<double>(m_bestRating.get()));
    LIBBOARDGAME_ASSERT(! m_file.isEmpty());
    QDir dir("");
    dir.mkpath(m_dir);
    ofstream out(m_file.toLocal8Bit().constData());
    for (size_t i = 0; i < m_games.size(); ++i)
    {
        auto& info = m_games[i];
        out << info.number << ' ' << static_cast<unsigned>(info.color.to_int())
            << ' ' << info.result << ' '
            << info.date.toLocal8Bit().constData() << ' ' << info.level
            << ' ' << info.rating << '\n';
    }
}

//-----------------------------------------------------------------------------
