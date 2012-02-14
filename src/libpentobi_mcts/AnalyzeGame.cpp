//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/AnalyzeGame.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGame.h"

#include "libboardgame_util/Log.h"
#include "libboardgame_util/WallTime.h"

namespace libpentobi_mcts {

using libboardgame_sgf::Node;
using libboardgame_util::log;
using libboardgame_util::clear_abort;
using libboardgame_util::get_abort;
using libboardgame_util::Exception;
using libboardgame_util::WallTime;
using libpentobi_base::Tree;

//-----------------------------------------------------------------------------

namespace {

class Restorer
{
public:
    Restorer(Game& game);

    ~Restorer();

private:
    Game& m_game;

    const Node& m_old_current;
};

Restorer::Restorer(Game& game)
    : m_game(game),
      m_old_current(game.get_current())
{
}

Restorer::~Restorer()
{
    m_game.goto_node(m_old_current);
}

} // namespace

//-----------------------------------------------------------------------------

void AnalyzeGame::run(Game& game, Search& search,
                    function<void(unsigned int,unsigned int)> progress_callback)
{
    m_game_variant = game.get_game_variant();
    m_moves.clear();
    m_has_value.clear();
    m_values.clear();
    Restorer restorer(game);
    const Tree& tree = game.get_tree();
    const Node& root = game.get_root();
    const Node* node = &root;
    unsigned int total_moves = 0;
    while (node != 0)
    {
        if (tree.has_move(*node))
            ++total_moves;
        node = node->get_first_child_or_null();
    }
    WallTime time_source;
    clear_abort();
    node = &root;
    unsigned int move_number = 0;
    while (node != 0)
    {
        ColorMove mv = tree.get_move(*node);
        if (! mv.is_null() && ! mv.move.is_pass())
        {
            if (! node->has_parent())
            {
                m_moves.push_back(mv);
                m_has_value.push_back(false);
                m_values.push_back(0);
            }
            else
            {
                progress_callback(move_number, total_moves);
                try
                {
                    game.goto_node(node->get_parent());
                    log() << game.get_board();
                    const ValueType max_count = 1000;
                    double max_time = 0;
                    size_t min_simulations = 1;
                    Move computer_mv;
                    search.search(computer_mv, mv.color, max_count,
                                  min_simulations, max_time, time_source);
                    if (get_abort())
                        break;
                    const Search::Node& search_root =
                        search.get_tree().get_root();
                    if (search_root.get_count() == 0)
                    {
                        m_moves.push_back(mv);
                        m_has_value.push_back(false);
                        m_values.push_back(0);
                    }
                    else
                    {
                        m_moves.push_back(mv);
                        m_has_value.push_back(true);
                        m_values.push_back(search_root.get_value());
                    }
                }
                catch (const Exception&)
                {
                    // Game::goto_node() can throw on invalid SGF tree read
                    // from external file. We simply abort the analysis.
                    break;
                }
            }
            ++move_number;
        }
        node = node->get_first_child_or_null();
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
