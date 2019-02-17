//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/AnalyzeGame.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_ANALYZE_GAME_H
#define LIBPENTOBI_MCTS_ANALYZE_GAME_H

#include <functional>
#include <vector>
#include "libpentobi_base/Game.h"

namespace libpentobi_mcts {

class Search;

using namespace std;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Evaluate each position in the main variation of a game. */
class AnalyzeGame
{
public:
    void clear();

    /** Run the analysis.
        The analysis can be aborted from a different thread with
        Search::abort().
        @param game
        @param search
        @param nu_simulations
        @param progress_callback Function that will be called at the beginning
        of the analysis of a position. Arguments: number moves analyzed so far,
        total number of moves. */
    void run(const Game& game, Search& search, size_t nu_simulations,
             const function<void(unsigned,unsigned)>& progress_callback);

    Variant get_variant() const;

    unsigned get_nu_moves() const;

    ColorMove get_move(unsigned i) const;

    double get_value(unsigned i) const;

    void set(Variant variant, const vector<ColorMove>& moves,
             const vector<double>& values);
private:
    Variant m_variant;

    vector<ColorMove> m_moves;

    vector<double> m_values;
};


inline ColorMove AnalyzeGame::get_move(unsigned i) const
{
    LIBBOARDGAME_ASSERT(i < m_moves.size());
    return m_moves[i];
}

inline unsigned AnalyzeGame::get_nu_moves() const
{
    return static_cast<unsigned>(m_moves.size());
}

inline double AnalyzeGame::get_value(unsigned i) const
{
    LIBBOARDGAME_ASSERT(i < m_values.size());
    return m_values[i];
}

inline Variant AnalyzeGame::get_variant() const
{
    return m_variant;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_ANALYZE_GAME_H
