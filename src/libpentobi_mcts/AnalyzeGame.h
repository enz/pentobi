//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/AnalyzeGame.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_ANALYZE_GAME_H
#define LIBPENTOBI_MCTS_ANALYZE_GAME_H

#include "Search.h"
#include "libpentobi_base/Game.h"

namespace libpentobi_mcts {

using libpentobi_base::Game;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

/** Evaluate each position in the main variation of a game. */
class AnalyzeGame
{
public:
    /** Run the analysis.
        @param game
        @param search
        @param progress_callback Function that will be called at the beginning
        of the analysis of a position. Arguments: number moves analyzed so far,
        total number of moves. */
    void run(Game& game, Search& search,
             function<void(unsigned int,unsigned int)> progress_callback);

    GameVariant get_game_variant() const;

    unsigned int get_nu_moves() const;

    bool has_value(unsigned int i) const;

    ColorMove get_move(unsigned int i) const;

    double get_value(unsigned int i) const;

private:
    GameVariant m_game_variant;

    vector<ColorMove> m_moves;

    vector<bool> m_has_value;

    vector<double> m_values;
};

inline GameVariant AnalyzeGame::get_game_variant() const
{
    return m_game_variant;
}

inline ColorMove AnalyzeGame::get_move(unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < m_moves.size());
    return m_moves[i];
}

inline unsigned int AnalyzeGame::get_nu_moves() const
{
    return m_moves.size();
}

inline double AnalyzeGame::get_value(unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < m_values.size());
    LIBBOARDGAME_ASSERT(has_value(i));
    return m_values[i];
}

inline bool AnalyzeGame::has_value(unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < m_has_value.size());
    return m_has_value[i];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_ANALYZE_GAME_H
