//-----------------------------------------------------------------------------
/** @file twogtp/OutputTree.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_OUTPUT_TREE_H
#define TWOGTP_OUTPUT_TREE_H

#include "libpentobi_base/Board.h"
#include "libpentobi_base/PentobiTree.h"

using namespace std;
using libboardgame_base::PointTransform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Move;
using libpentobi_base::PentobiTree;
using libpentobi_base::Point;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Merges opening moves played by the players into a tree.

    Keeps statistics of the average game result for each move and player. */
class OutputTree
{
public:
    explicit OutputTree(Variant variant);

    ~OutputTree();

    void load(const string& file);

    void save(const string& file);

    /** Add the moves of a game to the tree and update the move counters. */
    void add_game(const Board& bd, unsigned player_black, float result,
                  const array<bool, Board::max_moves>& is_real_move);

private:
    using PointTransform = libboardgame_base::PointTransform<Point>;


    PentobiTree m_tree;

    vector<unique_ptr<PointTransform>> m_transforms;

    vector<unique_ptr<PointTransform>> m_inv_transforms;
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_OUTPUT_TREE_H
