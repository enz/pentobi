//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/AnalyzeGame.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "AnalyzeGame.h"

#include "Search.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/WallTimeSource.h"

namespace libpentobi_mcts {

using libboardgame_sgf::SgfError;
using libboardgame_util::clear_abort;
using libboardgame_util::get_abort;
using libboardgame_util::WallTimeSource;
using libpentobi_base::BoardUpdater;

//-----------------------------------------------------------------------------

void AnalyzeGame::clear()
{
    m_moves.clear();
    m_values.clear();
}

void AnalyzeGame::run(const Game& game, Search& search, size_t nu_simulations,
                      const function<void(unsigned,unsigned)>& progress_callback)
{
    m_variant = game.get_variant();
    m_moves.clear();
    m_values.clear();
    auto& tree = game.get_tree();
    unique_ptr<Board> bd(new Board(m_variant));
    BoardUpdater updater;
    auto& root = game.get_root();
    auto node = &root;
    unsigned total_moves = 0;
    do
    {
        if (tree.has_move(*node))
            ++total_moves;
        node = node->get_first_child_or_null();
    }
    while (node != nullptr);
    WallTimeSource time_source;
    clear_abort();
    node = &root;
    unsigned move_number = 0;
    auto tie_value = Search::SearchParamConst::tie_value;
    do
    {
        auto mv = tree.get_move(*node);
        if (! mv.is_null())
        {
            if (! node->has_parent())
            {
                // Root shouldn't contain moves in SGF files
                m_moves.push_back(mv);
                m_values.push_back(static_cast<double>(tie_value));
            }
            else
            {
                progress_callback(move_number, total_moves);
                try
                {
                    updater.update(*bd, tree, node->get_parent());
                    LIBBOARDGAME_LOG("Analyzing move ", bd->get_nu_moves());
                    const auto max_count = Float(nu_simulations);
                    double max_time = 0;
                    // Set min_simulations to a reasonable value because
                    // nu_simulations can be reached without having that many
                    // value updates if a subtree from a previous search is
                    // reused (which re-initializes the value and value count
                    // of the new root from the best child)
                    size_t min_simulations = min(size_t(100), nu_simulations);
                    Move computer_mv;
                    search.search(computer_mv, *bd, mv.color, max_count,
                                  min_simulations, max_time, time_source);
                    if (get_abort())
                        break;
                    m_moves.push_back(mv);
                    m_values.push_back(static_cast<double>(
                                           search.get_root_val().get_mean()));
                }
                catch (const SgfError&)
                {
                    // BoardUpdater::update() can throw on invalid SGF tree
                    // read from external file. We simply abort the analysis.
                    break;
                }
            }
            ++move_number;
        }
        node = node->get_first_child_or_null();
    }
    while (node != nullptr);
}

void AnalyzeGame::set(Variant variant, const vector<ColorMove>& moves,
                      const vector<double>& values)
{
    LIBBOARDGAME_ASSERT(moves.size() == values.size());
    m_variant = variant;
    m_moves = moves;
    m_values = values;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
