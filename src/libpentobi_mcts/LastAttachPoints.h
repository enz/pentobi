//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LastAttachPoints.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H
#define LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H

#include "libpentobi_base/Board.h"

namespace libpentobi_mcts {

using libboardgame_base::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::Piece;
using libpentobi_base::Point;

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
        The returned value is an integer (1=yes, 0=no) to allow the user to
        quickly compute the number of these points that a move by the current
        player occupies while iterating over the points of the move. */
    int operator[](Point p) const;

private:
    /** List of last attach points, may contain duplicates. */
    ArrayList<Point,4 * Point::range_onboard_end> m_points;

    Grid<int> m_marker;
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
}

inline void LastAttachPoints::init(const Board& bd)
{
    if (! m_points.empty())
        clear();
    Color to_play = bd.get_to_play();
    Color second_color = bd.get_second_color(to_play);
    unsigned int move_number = bd.get_nu_moves();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-player variants)
    for (unsigned int i = 0; i < 3; ++i)
    {
        if (move_number == 0)
            return;
        --move_number;
        ColorMove move = bd.get_move(move_number);
        Color c = move.color;
        if (c == to_play || c == second_color)
            continue;
        Move mv = move.move;
        if (mv.is_pass())
            continue;
        const ArrayList<Point,Piece::max_attach>& attach_points
            = bd.get_move_info(mv).attach_points;
        auto j = attach_points.begin();
        auto end = attach_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            if (! bd.is_forbidden(*j, c))
            {
                m_points.push_back(*j);
                m_marker[*j] = 1;
            }
            ++j;
        }
        while (j != end);
    }
}

inline void LastAttachPoints::init_geometry(const Geometry& geometry)
{
    m_points.clear();
    m_marker.init(geometry, 0);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LAST_ATTACH_POINTS_H
