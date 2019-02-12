//-----------------------------------------------------------------------------
/** @file libpentobi_base/PentobiTree.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PENTOBI_TREE_H
#define LIBPENTOBI_BASE_PENTOBI_TREE_H

#include "ColorMove.h"
#include "BoardConst.h"
#include "NodeUtil.h"
#include "Variant.h"
#include "Setup.h"
#include "PentobiSgfUtil.h"
#include "libboardgame_sgf/SgfTree.h"

namespace libpentobi_base {

using libboardgame_sgf::SgfNode;
using libboardgame_sgf::SgfTree;

//-----------------------------------------------------------------------------

/** Blokus SGF tree.
    See also doc/blksgf/Pentobi-SGF.html in the Pentobi distribution for
    a description of the properties used. */
class PentobiTree
    : public SgfTree
{
public:
    /** Parse the GM property of a root node.
        @throws MissingProperty
        @throws InvalidProperty */
    static Variant get_variant(const SgfNode& root);


    explicit PentobiTree(Variant variant);

    explicit PentobiTree(unique_ptr<SgfNode>& root);

    void init(unique_ptr<SgfNode>& root) override;

    void init_variant(Variant variant);

    void set_move(const SgfNode& node, ColorMove mv);

    void set_move(const SgfNode& node, Color c, Move mv);

    bool has_move(const SgfNode& node) const;

    /** Return move or ColorMove::null() if node has no move property.
        @throws SgfError if the node has a move property with an invalid
        value. */
    ColorMove get_move(const SgfNode& node) const;

    const SgfNode* find_child_with_move(const SgfNode& node,
                                        ColorMove mv) const;

    void set_result(const SgfNode& node, int score);

    const SgfNode* get_node_before_move_number(unsigned move_number) const;

    Variant get_variant() const;

    string get_player_name(Color c) const;

    void set_player_name(Color c, const string& name);

    const BoardConst& get_board_const() const;

    void keep_only_subtree(const SgfNode& node);

    /** Add a piece as setup.
        @pre ! mv.is_null()
        If the node already contains a move, a new child will be created.
        @pre The piece points must be empty on the board
        @return The node or the new child if one was created. */
    const SgfNode& add_setup(const SgfNode& node, Color c, Move mv);

    /** Remove a piece using setup properties.
        @pre ! mv.is_null()
        If the node already contains a move, a new child will be created.
        @pre The move must exist on the board
        @return The node or the new child if one was created. */
    const SgfNode& remove_setup(const SgfNode& node, Color c, Move mv);

    /** Set the color to play in a setup position (PL property). */
    void set_player(const SgfNode& node, Color c);

    /** Remove the PL property.
        @see set_player() */
    bool remove_player(const SgfNode& node);

private:
    Variant m_variant;

    const BoardConst* m_bc;

    const char* get_color(Color c) const;

    Setup::PlacementList get_setup_property(const SgfNode& node,
                                            const char* id) const;

    const char* get_setup_prop_id(Color c) const;

    void set_setup(const SgfNode& node, const Setup& setup);

    void init_board_const(Variant variant);

    void set_game_property();

    void set_setup_property(const SgfNode& node, const char* id,
                            const Setup::PlacementList& placements);
};

inline const BoardConst& PentobiTree::get_board_const() const
{
    return *m_bc;
}

inline const char* PentobiTree::get_color(Color c) const
{
    return get_color_id(m_variant, c);
}

inline const char* PentobiTree::get_setup_prop_id(Color c) const
{
    return get_setup_id(m_variant, c);
}

inline Variant PentobiTree::get_variant() const
{
    return m_variant;
}

inline bool PentobiTree::has_move(const SgfNode& node) const
{
    return libpentobi_base::has_move(node, m_variant);
}

inline void PentobiTree::set_move(const SgfNode& node, ColorMove mv)
{
    set_move(node, mv.color, mv.move);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PENTOBI_TREE_H
