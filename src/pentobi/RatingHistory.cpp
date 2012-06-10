//-----------------------------------------------------------------------------
/** @file pentobi/RatingHistory.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingHistory.h"

#include <sstream>
#include <boost/filesystem/fstream.hpp>

using libpentobi_base::to_string_id;

//-----------------------------------------------------------------------------

RatingHistory::RatingHistory(GameVariant variant, const path& datadir)
{
    m_games.clear();
    m_dir = datadir;
    m_file = m_dir / "history.dat";
    boost::filesystem::ifstream file(m_file);
    if (! file)
        return;
    string line;
    while (getline(file, line))
    {
        istringstream in(line);
        GameInfo info;
        unsigned int c;
        in >> info.number >> c >> info.place >> info.is_place_shared
           >> info.date >> info.level >> info.rating;
        if (! in || c >= get_nu_colors(variant))
            return;
        info.color = Color(c);
        m_games.push_back(info);
    }
    size_t nuGames = m_games.size();
    if (nuGames > maxGames)
        m_games.erase(m_games.begin(),
                      m_games.begin() + nuGames - maxGames + 1);
}

void RatingHistory::add(unsigned int number, Color color, unsigned int place,
                        bool is_place_shared, const string& date, int level,
                        Rating rating)
{
    LIBBOARDGAME_ASSERT(! m_file.empty());
    GameInfo info;
    info.number = number;
    info.color = color;
    info.place = place;
    info.is_place_shared = is_place_shared;
    info.date = date;
    info.level = level;
    info.rating = rating;
    size_t nuGames = m_games.size();
    if (nuGames > maxGames)
        m_games.erase(m_games.begin(), m_games.begin() + maxGames - nuGames);
    m_games.push_back(info);
}

void RatingHistory::save() const
{
    LIBBOARDGAME_ASSERT(! m_file.empty());
    create_directories(m_dir);
    boost::filesystem::ofstream out(m_file);
    for (size_t i = 0; i < m_games.size(); ++i)
    {
        const GameInfo& info = m_games[i];
        out << info.number << ' ' << info.color.to_int() << ' '
            << info.place << ' ' << info.is_place_shared << ' '
            << info.date << ' ' << info.level << ' ' << info.rating << '\n';
    }
}

//-----------------------------------------------------------------------------
