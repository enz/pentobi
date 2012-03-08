//-----------------------------------------------------------------------------
/** @file libpentobi_base/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_H
#define LIBPENTOBI_BASE_TREE_H

#include <boost/format.hpp>
#include "ColorMove.h"
#include "BoardConst.h"
#include "GameVariant.h"
#include "Setup.h"
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

    void init(unique_ptr<Node>& root);

    void init_game_variant(GameVariant game_variant);

    void init_game_variant(GameVariant game_variant, const Setup& setup);

    void set_move(const Node& node, ColorMove mv);

    void set_move(const Node& node, Color c, Move mv);

    /** Return move or ColorMove::null() if node has no move property.
        @throws Exception if the node has a move property with an invalid
        value. */
    ColorMove get_move(const Node& node) const;

    /** Like get_move() but returns ColorMove::null() on invalid property
        value. */
    ColorMove get_move_ignore_invalid(const Node& node) const;

    /** Static version of get_move(const Node&).
        Provided for use cases in which it is too much overhead to use
        a libpentobi_base::Tree because of the costly BoardConst construction
        (e.g. the Pentobi thumbnailer).
        @param node
        @param game_variant
        @param[out] c The move color (only defined if return value is true)
        @param[out] points The move points (only defined if return value is
        true)
        @return true if the node has a move property and the move is not a pass
        move. */
    static bool get_move(const Node& node, GameVariant game_variant, Color& c,
                         MovePoints& points);

    /** Same as ! get_move.is_null() */
    bool has_move(const Node& node) const;

    /** Same as ! get_move_ignore_invalid.is_null() */
    bool has_move_ignore_invalid(const Node& node) const;

    static bool has_setup_properties(const Node& node);

    const Node* find_child_with_move(const Node& node, ColorMove mv) const;

    void set_result(const Node& node, int score);

    const Node* get_node_before_move_number(unsigned int move_number) const;

    GameVariant get_game_variant() const;

    string get_player_name(Color c) const;

    void set_player_name(Color c, const string& name);

    const BoardConst& get_board_const() const;

    /** Check if any node in the main variation has a move.
        Invalid move properties are ignored. */
    bool has_main_variation_moves() const;

    void set_setup_property(const Node& node, const char* id,
                            const Setup::PlacementList& placements);

private:
    GameVariant m_game_variant;

    const BoardConst* m_board_const;

    void init_board_const(GameVariant game_variant);
};

inline const BoardConst& Tree::get_board_const() const
{
    return *m_board_const;
}

inline GameVariant Tree::get_game_variant() const
{
    return m_game_variant;
}

inline bool Tree::has_move(const Node& node) const
{
    return ! get_move(node).is_null();
}

inline bool Tree::has_move_ignore_invalid(const Node& node) const
{
    return ! get_move_ignore_invalid(node).is_null();
}

inline void Tree::set_move(const Node& node, ColorMove mv)
{
    set_move(node, mv.color, mv.move);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TREE_H
