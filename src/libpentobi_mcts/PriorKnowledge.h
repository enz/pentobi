//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PriorKnowledge.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
#define LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H

#include "Float.h"
#include "LocalValue.h"
#include "libboardgame_mcts/Tree.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/MoveList.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::Board;
using libpentobi_base::ColorMap;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveList;

//-----------------------------------------------------------------------------

/** Initializes newly created nodes with heuristic prior count and value. */
class PriorKnowledge
{
public:
    typedef libboardgame_mcts::Node<Move, Float> Node;

    typedef libboardgame_mcts::Tree<Node> Tree;

    PriorKnowledge();

    void start_search(const Board& bd);

    /** Generate children nodes initialized with prior knowledge. */
    void gen_children(const Board& bd, const MoveList& moves,
                      bool is_symmetry_broken, Tree::NodeExpander& expander,
                      Float init_val);

private:
    struct MoveFeatures
    {
        /** Heuristic value of the move expressed in score points. */
        Float heuristic;

        bool is_local;

        /** Does the move touch a piece of the same player? */
        bool connect;

        /** Only used on Classic and Trigon boards. */
        unsigned short dist_to_center;
    };

    array<MoveFeatures, Move::range> m_features;

    /** Maximum of Features::heuristic for all moves. */
    Float m_max_heuristic;

    bool m_has_connect_move;

    ColorMap<bool> m_check_dist_to_center;

    unsigned m_dist_to_center_max_pieces;

    unsigned short m_min_dist_to_center;

    /** Marker for points close to attach points of recent opponent moves.
        0: point is not local, 1: (non-forbidden) oppponent attach point,
        2: adj. to attach point, 3: 2nd order adj. to attach point. */
    Grid<uint_fast8_t> m_local_value;

    /** Points in m_local_value with value greater zero. */
    PointList m_local_points;

    /** Distance to center heuristic. */
    Grid<unsigned short> m_dist_to_center;

    void compute_features(const Board& bd, const MoveList& moves,
                          bool check_dist_to_center, bool check_connect);

    void init_local(const Board& bd);
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
