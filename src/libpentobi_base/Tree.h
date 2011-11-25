//-----------------------------------------------------------------------------
/** @file libpentobi_base/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_H
#define LIBPENTOBI_BASE_TREE_H

#include <boost/format.hpp>
#include "ColorMove.h"
#include "BoardConst.h"
#include "GameVariant.h"
#include "libboardgame_sgf/Tree.h"
#include "libboardgame_sgf/InvalidPropertyValue.h"

namespace libpentobi_base {

using namespace std;
using boost::format;
using libboardgame_sgf::Node;

//-----------------------------------------------------------------------------

/** Blokus SGF tree.
    See also doc/blksgf/Blokus-SGF.html in the Pentobi distribution for
    a description of the properties used. */
class Tree
    : public libboardgame_sgf::Tree
{
public:
    Tree(GameVariant game_variant);

    Tree(unique_ptr<Node>& root);

    void init(GameVariant game_variant);

    void init(unique_ptr<Node>& root);

    void set_move(const Node& node, ColorMove mv);

    void set_move(const Node& node, Color c, Move mv);

    /** Return move or ColorMove::null() if node has no move property. */
    ColorMove get_move(const Node& node) const;

    bool has_move(const Node& node) const;

    const Node* find_child_with_move(const Node& node, ColorMove mv) const;

    void set_result(const Node& node, int score);

    const Node* get_node_with_move_number(unsigned int move_number) const;

    GameVariant get_game_variant() const;

    string get_player_name(Color c) const;

    void set_player_name(Color c, const string& name);

private:
    GameVariant m_game_variant;

    const BoardConst* m_board_const;

    void init_board_const(GameVariant game_variant);
};

inline GameVariant Tree::get_game_variant() const
{
    return m_game_variant;
}

inline bool Tree::has_move(const Node& node) const
{
    return ! get_move(node).is_null();
}

inline void Tree::set_move(const Node& node, ColorMove mv)
{
    set_move(node, mv.color, mv.move);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TREE_H
