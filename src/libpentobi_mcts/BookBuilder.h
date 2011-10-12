//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/BookBuilder.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_BOOK_BUILDER_H
#define LIBPENTOBI_MCTS_BOOK_BUILDER_H

#include <boost/filesystem.hpp>
#include "libpentobi_base/GameVariant.h"
#include "libpentobi_base/BoardUpdater.h"
#include "libpentobi_mcts/Player.h"

namespace libpentobi_mcts {

using boost::filesystem::path;
using libboardgame_sgf::Node;
using libpentobi_base::Board;
using libpentobi_base::BoardUpdater;
using libpentobi_base::GameVariant;
using libboardgame_base::Transform;
using libpentobi_base::Tree;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

/** Automatic generation of opening books.
    The algorithm for the book generation is a modified version of the algorithm
    described in T. R. Lincke, "Strategies for the Automatic Construction of
    Opening Books", Computers and Games Conference 2000. For each node in the
    tree, a heuristic value h, a value v, and an expansion priority p are stored
    (as comment properties). The value v is computed by backing up the h values
    of the leaf nodes using minimax. The next node to expand is the leaf node
    that can be reached from the root by always chosing the child with the
    lowest priority. The priority p is 1+w(v*-v) for leaf nodes and
    1+w(v*-v)+min(p(s)) for other nodes. The minimum is over all children s,
    v* is the value of the best sibling node (including this node). This will
    expand nodes first that have shallow subtrees and/or good values. The
    expansion parameter w determines how deep (large w) or shallow (small w) the
    tree becomes.
    @todo Separate out game and heuristic-specific parts to make this class
    usable for other games and heuristic generators. */
class BookBuilder
{
public:
    BookBuilder(GameVariant game_variant);

    void build(const path& file);

    /** Make the book smaller by pruning nodes that are not necessary when
        using the book.
        Prunes nodes that are a child of a parent where the heuristic evaluation
        of the children was not finished yet, and nodes that have values that
        have a greater difference than prune_book_diff to the best value among
        their siblings. Also removes all comment properties but "v". */
    void prune(const path& file, double prune_book_diff);

private:
    GameVariant m_game_variant;

    Tree m_tree;

    double m_expansion_parameter;

    Board m_bd;

    BoardUpdater m_updater;

    Player m_player;

    unique_ptr<ArrayList<Move, Move::range>> m_moves;

    void add_leaf(const vector<ColorMove>& sequence, ColorMove mv);

    void add_leaf(const vector<ColorMove>& sequence, ColorMove mv,
                  Transform transform);

    bool are_children_evaluated(const Node& node) const;

    double create_heuristic();

    const Node* find_node(const vector<ColorMove>& sequence) const;

    const Node& find_node_to_expand() const;

    const Node* find_unevaluated_node() const;

    vector<ColorMove> generate_moves() const;

    vector<ColorMove> get_sequence(const Node& node);

    ColorMove get_transformed(ColorMove mv, Transform transform) const;

    void add_value(const vector<ColorMove>& sequence, double value);

    void add_value(const vector<ColorMove>& sequence, double value,
                   Transform transform);

    double inv_value(double value) const;

    void sort_children(const Node& node);

    void update_priority(const Node& node, double min_sibling_value);

    void update_value(const Node& node);

    void write(const path& file) const;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_BOOK_BUILDER_H
