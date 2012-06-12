//-----------------------------------------------------------------------------
/** @file libpentobi_base/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_H
#define LIBPENTOBI_BASE_TREE_H

#include <boost/format.hpp>
#include "ColorMove.h"
#include "BoardConst.h"
#include "Variant.h"
#include "Setup.h"
#include "SgfUtil.h"
#include "libboardgame_sgf/Tree.h"

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
    Tree(Variant variant);

    Tree(unique_ptr<Node>& root);

    void init(unique_ptr<Node>& root);

    void init_variant(Variant variant);

    void set_move(const Node& node, ColorMove mv);

    void set_move(const Node& node, Color c, Move mv);

    /** Return move or ColorMove::null() if node has no move property.
        @throws InvalidTree if the node has a move property with an invalid
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
        @param variant
        @param[out] c The move color (only defined if return value is true)
        @param[out] points The move points (only defined if return value is
        true)
        @return true if the node has a move property and the move is not a pass
        move. */
    static bool get_move(const Node& node, Variant variant, Color& c,
                         MovePoints& points);

    /** Same as ! get_move.is_null() */
    bool has_move(const Node& node) const;

    /** Same as ! get_move_ignore_invalid.is_null() */
    bool has_move_ignore_invalid(const Node& node) const;

    /** Check if a node has setup properties (not including the PL property). */
    static bool has_setup(const Node& node);

    const Node* find_child_with_move(const Node& node, ColorMove mv) const;

    void set_result(const Node& node, int score);

    const Node* get_node_before_move_number(unsigned int move_number) const;

    Variant get_variant() const;

    string get_player_name(Color c) const;

    void set_player_name(Color c, const string& name);

    const BoardConst& get_board_const() const;

    /** Check if any node in the main variation has a move.
        Invalid move properties are ignored. */
    bool has_main_variation_moves() const;

    void keep_only_subtree(const Node& node);

    /** Add a piece as setup.
        @pre mv.is_regular()
        If the node already contains a move, a new child will be created.
        @pre The piece points must be empty on the board
        @return The node or the new child if one was created. */
    const Node& add_setup(const Node& node, Color c, Move mv);

    /** Remove a piece using setup properties.
        @pre mv.is_regular()
        If the node already contains a move, a new child will be created.
        @pre The move must exist on the board
        @return The node or the new child if one was created. */
    const Node& remove_setup(const Node& node, Color c, Move mv);

    /** Set the color to play in a setup position (PL property). */
    void set_player(const Node& node, Color c);

    /** Get the color to play in a setup position (PL property). */
    static bool get_player(const Node& node, Color& c);

    /** Remove the PL property.
        @see set_player() */
    void remove_player(const Node& node);

private:
    Variant m_variant;

    const BoardConst* m_board_const;

    const char* get_color(Color c) const;

    Setup::PlacementList get_setup_property(const Node& node,
                                            const char* id) const;

    const char* get_setup_prop_id(Color c) const;

    void set_setup(const Node& node, const Setup& setup);

    void init_board_const(Variant variant);

    void set_game_property();

    void set_setup_property(const Node& node, const char* id,
                            const Setup::PlacementList& placements);
};

inline const BoardConst& Tree::get_board_const() const
{
    return *m_board_const;
}

inline const char* Tree::get_color(Color c) const
{
    return sgf_util::get_color_id(m_variant, c);
}

inline const char* Tree::get_setup_prop_id(Color c) const
{
    return sgf_util::get_setup_id(m_variant, c);
}

inline Variant Tree::get_variant() const
{
    return m_variant;
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
