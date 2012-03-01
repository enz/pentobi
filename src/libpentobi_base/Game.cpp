//-----------------------------------------------------------------------------
/** @file libpentobi_base/Game.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Game.h"

#include "BoardUtil.h"
#include "libboardgame_sgf/InvalidPropertyValue.h"
#include "libboardgame_sgf/Util.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::util::is_main_variation;
using libpentobi_base::boardutil::get_current_position_as_setup;

//-----------------------------------------------------------------------------

Game::Game(GameVariant game_variant)
  : m_bd(new Board(game_variant)),
    m_tree(game_variant),
    m_updater(m_tree, *m_bd)
{
    init(game_variant);
}

Game::Game(unique_ptr<Node>& root)
  : m_bd(new Board(m_tree.get_game_variant())),
    m_tree(m_tree.get_game_variant()),
    m_updater(m_tree, *m_bd)
{
    init(root);
}

void Game::goto_node(const Node& node)
{
    m_updater.update(node);
    m_current = &node;
}

void Game::init(GameVariant game_variant)
{
    m_bd->init(game_variant);
    m_tree.init_game_variant(game_variant);
    m_current = &m_tree.get_root();
}

void Game::init(unique_ptr<Node>& root)
{
    m_tree.init(root);
    m_bd->init(m_tree.get_game_variant());
    // Set m_current to root such that it has a defined value even if
    // goto_node() throws because of invalid properties in the root node
    m_current = &m_tree.get_root();
    goto_node(m_tree.get_root());
}

void Game::keep_only_position()
{
    Setup setup;
    get_current_position_as_setup(*m_bd, setup);
    m_tree.init(m_tree.get_game_variant(), setup);
    goto_node(m_tree.get_root());
}

void Game::play(ColorMove mv, bool always_create_new_node)
{
    m_bd->play(mv);
    const Node* child = 0;
    if (! always_create_new_node)
        child = m_tree.find_child_with_move(*m_current, mv);
    if (child != 0)
        m_current = child;
    else
    {
        m_current = &m_tree.create_new_child(*m_current);
        m_tree.set_move(*m_current, mv);
    }
}

void Game::set_result(int score)
{
    if (is_main_variation(*m_current))
        m_tree.set_result(m_tree.get_root(), score);
}

void Game::truncate()
{
    goto_node(m_tree.truncate(*m_current));
}

void Game::undo()
{
    LIBBOARDGAME_ASSERT(! m_tree.get_move(*m_current).is_null());
    LIBBOARDGAME_ASSERT(m_current->has_parent());
    goto_node(m_current->get_parent());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
