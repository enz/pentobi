//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LOCAL_VALUE_H
#define LIBPENTOBI_MCTS_LOCAL_VALUE_H

#include "libboardgame_util/Log.h"
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using libboardgame_base::ArrayList;
using libboardgame_util::log;
using libpentobi_base::AdjIterator;
using libpentobi_base::AttachPoints;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointList;

//-----------------------------------------------------------------------------

/** Classify playout moves to prefer local response moves.
    In the playout policy, local response moves are preferred. That is, a
    random move is chosen from the set of moves with the highest local response
    value. The value is higher if the move occupies more attach points of the
    last three moves by the opponent (i.e. the last move of each opponent in
    four-player game variants, the last two opponent moves in Duo, and the last
    two opponent moves, one of each opponent color, in the game variants with
    two players and four colors). Within moves with the same number of attach
    points occupied, the value is higher if it also occupies any point
    adjacent to the attach points (no matter how many). Using these values
    will prefer local responses in the playouts without becoming too
    deterministic. Attach points that are forbidden to the color of the
    opponent move are not considered. */
class LocalValue
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        Compute(const LocalValue& local_value);

        /** Add a point of the move. */
        void add_move_point(Point p);

        /** Finish the computation and return the value. */
        unsigned finish();

    private:
        const LocalValue& m_local_value;

        unsigned m_value;
    };

    friend class Compute;

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

private:
    /** Points with point value greater zero. */
    PointList m_points;

    Grid<unsigned> m_point_value;
};

inline LocalValue::Compute::Compute(const LocalValue& local_value)
    : m_local_value(local_value)
{
    m_value = 0;
}

inline void LocalValue::Compute::add_move_point(Point p)
{
    m_value += m_local_value.m_point_value[p];
}

inline unsigned LocalValue::Compute::finish()
{
    // We only care if it occupied any point ajacent to the attach points, not
    // how many (attach points use a value of 0x10 adjacent points a value of
    // 0x01 during computation). This works only as long as there are not more
    // than 0x10 points covered by a piece.
    static_assert(PieceInfo::max_size < 0x10, "");
    if ((m_value & 0xf) != 0)
        return (m_value & 0xf0) + 1;
    else
        return m_value;
}

inline void LocalValue::clear()
{
    for (auto i = m_points.begin(); i != m_points.end(); ++i)
        m_point_value[*i] = 0;
    m_points.clear();
}

inline void LocalValue::init(const Board& bd)
{
    if (! m_points.empty())
        clear();
    Color to_play = bd.get_to_play();
    Color second_color = bd.get_second_color(to_play);
    unsigned move_number = bd.get_nu_moves();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-player variants)
    for (unsigned i = 0; i < 3; ++i)
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
        const AttachPoints& attach_points
            = bd.get_move_info_ext(mv).attach_points;
        auto j = attach_points.begin();
        auto end = attach_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            if (! bd.is_forbidden(*j, c))
            {
                if (m_point_value[*j] == 0)
                    m_points.push_back(*j);
                m_point_value[*j] = 0x10;
                for (AdjIterator k(bd, *j); k; ++k)
                    if (! bd.is_forbidden(*k, c) && m_point_value[*k] == 0)
                    {
                        m_points.push_back(*k);
                        m_point_value[*k] = 0x01;
                    }
            }
            ++j;
        }
        while (j != end);
    }
}

inline void LocalValue::init_geometry(const Geometry& geometry)
{
    m_points.clear();
    m_point_value.init(geometry, 0);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
