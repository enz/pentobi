//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PlayoutFeatures.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H
#define LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H

#include <algorithm>
#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::BoardIterator;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Compute move features for the playout policy.
    A local move is a move that occupies attach points of recent opponent moves
    or points that are adjacent to them. If there is only one adjacent point to
    such an opponent attach point missing to make it a 1-hole, the missing
    adjacent point counts as an attach point.
    The features also include if a point is forbidden, such that the legality
    of a move can be quickly checked in the playouts. The forbidden status
    is updated incrementally. */
class PlayoutFeatures
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        /** Constructor.
            @param p The first point of the move */
        Compute(Point p, const PlayoutFeatures& playout_features)
            : m_value(playout_features.m_point_value[p])
        { }

        /** Add a point of the move.
            @return false if point is forbidden */
        bool add(Point p, const PlayoutFeatures& playout_features)
        {
            auto value = playout_features.m_point_value[p];
            if ((value & 0xf000u) != 0)
                return false;
            m_value += value;
            return true;
        }

        bool is_forbidden() const
        {
            return (m_value & 0xf000u) != 0;
        }

        /** Does the move occupy any local points?
            @pre ! is_forbidden() */
        bool has_local() const
        {
            LIBBOARDGAME_ASSERT(! is_forbidden());
            return (m_value != 0);
        }

        /** Get the number of local opponent attach points occupied by this
            move. */
        unsigned get_nu_attach() const
        {
            return m_value & 0x000fu;
        }

        /** Does the move occupy any points adjacent to local opponent attach
            points? */
        bool has_adj_attach() const
        {
            return ((m_value & 0x00f0u) != 0);
        }

    private:
        unsigned m_value;
    };

    friend class Compute;

    PlayoutFeatures();

    /** Initialize snapshot with forbidden stae. */
    void init_snapshot(const Board& bd, Color c);

    void restore_snapshot(const Board& bd);

    /** Set points of move to forbidden. */
    void set_forbidden(const MoveInfo& info);

    /** Set adjacent points of move to forbidden. */
    void set_forbidden(const MoveInfoExt& info_ext);

    void set_local(const Board& bd);

private:
    Grid<unsigned> m_point_value;

    Grid<unsigned> m_snapshot;

    /** Points with point value greater zero. */
    PointList m_local_points;
};

inline void PlayoutFeatures::init_snapshot(const Board& bd, Color c)
{
    auto& is_forbidden = bd.is_forbidden(c);
    for (BoardIterator i(bd); i; ++i)
        m_snapshot[*i] = (is_forbidden[*i] ? 0x1000u : 0);
}


inline void PlayoutFeatures::restore_snapshot(const Board& bd)
{
    m_point_value.memcpy_from(m_snapshot, bd.get_geometry());
}

inline void PlayoutFeatures::set_forbidden(const MoveInfo& info)
{
    auto i = info.begin();
    auto end = info.end();
    do
        m_point_value[*i] = 0x1000u;
    while (++i != end);
}

inline void PlayoutFeatures::set_forbidden(const MoveInfoExt& info_ext)
{
    auto i = info_ext.begin_adj();
    auto end = info_ext.end_adj();
    do
        m_point_value[*i] = 0x1000u;
    while (++i != end);
}
inline void PlayoutFeatures::set_local(const Board& bd)
{
    // Clear old info about local points
    for (Point p : m_local_points)
        m_point_value[p] &= 0xf000u;
    m_local_points.clear();

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
        LIBBOARDGAME_ASSERT(mv.is_regular());
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = bd.get_move_info_ext(mv);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
        {
            if (is_forbidden[*j] || (m_point_value[*j] & 0x0fff) == 0x0001u)
                continue;
            if ((m_point_value[*j] & 0x0fff) == 0)
                m_local_points.push_back(*j);
            // Opponent attach point
            m_point_value[*j] = (m_point_value[*j] & 0xf000) | 0x0001u;
            unsigned nu_adj = 0;
            geo.for_each_adj(*j, [&](Point k) {
                if (! is_forbidden[k])
                {
                    ++nu_adj;
                    if ((m_point_value[k] & 0x0fff) == 0)
                    {
                        m_local_points.push_back(k);
                        // Adjacent to opp. attach point
                        m_point_value[k] =
                                (m_point_value[k] & 0xf000) | 0x0010u;
                    }
                }
            });
            // If occupying the attach point is forbidden for us but there is
            // only 1 adj. point missing to make it a 1-point hole for the
            // opponent, then occupying this adj. point is (almost) as good as
            // occupying the attach point. (This is done only for 1-point holes
            // that are forbidden for to_play.)
            if (nu_adj == 1 && bd.is_forbidden(*j, to_play))
                for (Point k : geo.get_adj(*j))
                    if (! is_forbidden[k])
                    {
                        LIBBOARDGAME_ASSERT(m_local_points.contains(k));
                        m_point_value[k] =
                                (m_point_value[k] & 0xf000) | 0x0001u;
                        break;
                    }
        }
        while (++j != end);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H
