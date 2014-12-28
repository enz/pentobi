//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LOCAL_VALUE_H
#define LIBPENTOBI_MCTS_LOCAL_VALUE_H

#include <algorithm>
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::AdjIterator;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Classify moves for the playout policy to prefer local response moves.
    A local response move is a move that occupies attach points of recent
    opponent moves or points that are adjacent to them. If there is only one
    adjacent point to such an opponent attach point missing to make it a
    1-hole, the missing adjacent point counts as an attach point. */
class LocalValue
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        Compute()
            : m_value(0)
        { }

        /** Contruct and already add the first point. */
        Compute(Point p, const LocalValue& local_value)
            : m_value(local_value.m_point_value[p])
        { }

        /** Add a point of the move. */
        void add_move_point(Point p, const LocalValue& local_value)
        {
            m_value += local_value.m_point_value[p];
        }

        /** Does the move occupy any local points? */
        bool has_local() const
        {
            return m_value != 0;
        }

        /** Get the number of local opponent attach points occupied by this
            move. */
        unsigned get_nu_attach() const
        {
            return m_value & 0x00fu;
        }

        /** Does the move occupy any points adjacent to local opponent attach
            points? */
        bool has_adj_attach() const
        {
            return ((m_value & 0x0f0u) != 0);
        }

    private:
        unsigned m_value;
    };

    friend class Compute;

    LocalValue();

    /** Find the attach points of the last opponent moves in a given position.
        @param bd The board. */
    void init(const Board& bd);

private:
    Grid<unsigned> m_point_value;

    /** Points with point value greater zero. */
    PointList m_points;
};

inline void LocalValue::init(const Board& bd)
{
    // Clear the stored last opponent attach moves
    for (Point p: m_points)
        m_point_value[p] = 0;
    m_points.clear();

    Color to_play = bd.get_to_play();
    Color second_color;
    if (bd.get_variant() == Variant::classic_3 && to_play.to_int() == 3)
        second_color = Color(bd.get_alt_player());
    else
        second_color = bd.get_second_color(to_play);
    auto& geo = bd.get_geometry();
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
        auto mv = move.move;
        if (mv.is_pass())
            continue;
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = bd.get_move_info_ext(mv);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
        {
            if (is_forbidden[*j])
                continue;
            // Don't check if m_point_value[*j] == 0x001u, it's faster to
            // handle this rare case twice than to check for it.
            if (m_point_value[*j] == 0)
                m_points.push_back(*j);
            // Opponent attach point
            m_point_value[*j] = 0x001u;
            unsigned nu_adj = 0;
            for (AdjIterator k(geo, *j); k; ++k)
                if (! is_forbidden[*k])
                {
                    ++nu_adj;
                    if (m_point_value[*k] == 0)
                    {
                        m_points.push_back(*k);
                        // Adjacent to opp. attach point
                        m_point_value[*k] = 0x010u;
                    }
                }
            // If occupying the attach point is forbidden for us but there is
            // only 1 adj. point missing to make it a 1-point hole for the
            // opponent, then occupying this adj. point is (almost) as good as
            // occupying the attach point. (This is done only for 1-point holes
            // that are forbidden for to_play.)
            if (nu_adj == 1 && bd.is_forbidden(*j, to_play))
                for (AdjIterator k(geo, *j); k; ++k)
                    if (! is_forbidden[*k])
                    {
                        m_point_value[*k] = 0x001u;
                        break;
                    }
        }
        while (++j != end);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
