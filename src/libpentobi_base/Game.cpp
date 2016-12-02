//-----------------------------------------------------------------------------
/** @file libpentobi_base/Game.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Game.h"

#include "BoardUtil.h"
#include "libboardgame_sgf/InvalidTree.h"
#include "libboardgame_sgf/SgfUtil.h"

namespace libpentobi_base {

using libboardgame_sgf::InvalidTree;
using libboardgame_sgf::util::back_to_main_variation;
using libboardgame_sgf::util::is_main_variation;
using libpentobi_base::boardutil::get_current_position_as_setup;

//-----------------------------------------------------------------------------

Game::Game(Variant variant)
  : m_bd(new Board(variant)),
    m_tree(variant)
{
    init(variant);
}

Game::~Game() = default;

void Game::add_setup(Color c, Move mv)
{
    auto& node = m_tree.add_setup(*m_current, c, mv);
    goto_node(node);
}

void Game::delete_all_variations()
{
    goto_node(back_to_main_variation(*m_current));
    m_tree.delete_all_variations();
}

Color Game::get_to_play_default(const Game& game)
{
    auto& tree = game.get_tree();
    auto& bd = game.get_board();
    auto node = &game.get_current();
    Color next = Color(0);
    do
    {
        auto mv = tree.get_move(*node);
        if (! mv.is_null())
        {
            next = bd.get_next(mv.color);
            break;
        }
        Color c;
        if (libpentobi_base::node_util::get_player(*node, c))
            return c;
        node = node->get_parent_or_null();
    }
    while (node);
    return bd.get_effective_to_play(next);
}

void Game::goto_node(const SgfNode& node)
{
    auto old = m_current;
    try
    {
        update(node);
    }
    catch (const InvalidTree&)
    {
        // Try to restore the old state.
        if (! old)
            m_current = &node;
        else
        {
            try
            {
                update(*old);
            }
            catch (const InvalidTree&)
            {
            }
        }
        throw;
    }
}

void Game::init(Variant variant)
{
    m_bd->init(variant);
    m_tree.init_variant(variant);
    m_current = &m_tree.get_root();
}

void Game::init(unique_ptr<SgfNode>& root)
{
    m_tree.init(root);
    m_bd->init(m_tree.get_variant());
    m_current = nullptr;
    goto_node(m_tree.get_root());
}

void Game::keep_only_position()
{
    m_tree.keep_only_subtree(*m_current);
    m_tree.remove_children(m_tree.get_root());
    m_current = nullptr;
    goto_node(m_tree.get_root());
}

void Game::keep_only_subtree()
{
    m_tree.keep_only_subtree(*m_current);
    m_current = nullptr;
    goto_node(m_tree.get_root());
}

void Game::play(ColorMove mv, bool always_create_new_node)
{
    m_bd->play(mv);
    const SgfNode* child = nullptr;
    if (! always_create_new_node)
        child = m_tree.find_child_with_move(*m_current, mv);
    if (child)
        m_current = child;
    else
    {
        m_current = &m_tree.create_new_child(*m_current);
        m_tree.set_move(*m_current, mv);
    }
    set_to_play(get_to_play_default(*this));
}

void Game::remove_player()
{
    m_tree.remove_player(*m_current);
    update(*m_current);
}

void Game::remove_setup(Color c, Move mv)
{
    auto& node = m_tree.remove_setup(*m_current, c, mv);
    goto_node(node);
}

void Game::set_player(Color c)
{
    m_tree.set_player(*m_current, c);
    update(*m_current);
}

void Game::set_result(int score)
{
    if (is_main_variation(*m_current))
        m_tree.set_result(m_tree.get_root(), score);
}

void Game::set_to_play(Color c)
{
    m_bd->set_to_play(c);
}

void Game::truncate()
{
    goto_node(m_tree.truncate(*m_current));
}

void Game::undo()
{
    LIBBOARDGAME_ASSERT(! m_tree.get_move(*m_current).is_null());
    LIBBOARDGAME_ASSERT(m_current->has_parent());
    truncate();
}

void Game::update(const SgfNode& node)
{
    m_updater.update(*m_bd, m_tree, node);
    m_current = &node;
    set_to_play(get_to_play_default(*this));
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
