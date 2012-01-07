//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LastAttachPoints.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H
#define LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H

#include "libpentobi_base/Board.h"

namespace libpentobi_mcts {

using libpentobi_base::Board;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::Point;
using libpentobi_base::PointList;

//-----------------------------------------------------------------------------

/** Finds the attach points of the last opponent moves.
    This feature can be used to find moves that are local responses to the last
    opponent moves and occupy the attach points of those moves. Only opponent
    attach points that are not forbidden to the opponent are used. The last
    three moves are considered (i.e. the last move of each opponent in
    four-player game variants, the last two opponent moves in Duo, and the
    last two opponent moves, one of each opponent color, in the game variants
    with two players and four colors). */
class LastAttachPoints
{
public:
    /** Initialize geometry.
        Must be called before using this class and whenever the board geometry
        changes. */
    void init_geometry(const Geometry& geometry);

    /** Find the attach points of the last opponent moves in a given position.
        @param bd The board. Must have the same geometry as in the last call
        of init_geometry(). */
    void init(const Board& bd);

    /** Clear the stored last opponent attach moves. */
    void clear();

    /** Check if a point is an attach points of the last opponent moves.
        Thr returned value is an integer (0=yes, 1=no) to allow the user to
        quickly compute the number of these points that a move by the current
        player occupies while iterating over the points of the move. */
    int operator[](Point p) const;

    bool is_adj(Point p) const;

private:
    PointList m_points;

    Grid<int> m_marker;

    PointList m_adj_points;

    Grid<bool> m_adj_marker;
};

inline int LastAttachPoints::operator[](Point p) const
{
    return m_marker[p];
}

inline void LastAttachPoints::clear()
{
    for (auto i = m_points.begin(); i != m_points.end(); ++i)
        m_marker[*i] = 0;
    m_points.clear();
    for (auto i = m_adj_points.begin(); i != m_adj_points.end(); ++i)
        m_adj_marker[*i] = false;
    m_adj_points.clear();
}

inline void LastAttachPoints::init_geometry(const Geometry& geometry)
{
    m_points.clear();
    m_adj_points.clear();
    m_marker.init(geometry, 0);
    m_adj_marker.init(geometry, false);
}

inline bool LastAttachPoints::is_adj(Point p) const
{
    return m_adj_marker[p];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H
