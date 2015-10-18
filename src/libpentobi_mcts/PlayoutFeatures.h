//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PlayoutFeatures.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H
#define LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H

#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::Board;
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
    This class encodes features that correspond to points on the board in bit
    ranges of an integer, such that the sum of the features values for all
    points of a move can be quickly computed in the playout move generation.
    Currently, there are only two features: the forbidden status and whether
    the point is a local point. Local points are attach points of recent
    opponent moves or points that are adjacent to them. Local points that
    are attach points of the color to play count double.
    During a simulation, some of the features are updated incrementally
    (forbidden status) and some non-incrementally (local points). */
class PlayoutFeatures
{
public:
    /** The maximum number of local points for a move.
        The number can be higher than PieceInfo::max_size (see class
        description). */
    static const unsigned max_local = 2 * PieceInfo::max_size;

    /** Compute the sum of the feature values for a move. */
    class Compute
    {
    public:
        /** Constructor.
            @param p The first point of the move
            @param playout_features */
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

        /** Get the number of local points for this move.
            @pre ! is_forbidden()
            @return The number of local points in [0..max_local] */
        unsigned get_nu_local() const
        {
            LIBBOARDGAME_ASSERT(! is_forbidden());
            return m_value;
        }

    private:
        unsigned m_value;
    };

    friend class Compute;

    PlayoutFeatures();

    /** Initialize snapshot with forbidden state. */
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

    /** Points with non-zero local value. */
    PointList m_local_points;
};

inline void PlayoutFeatures::init_snapshot(const Board& bd, Color c)
{
    auto& is_forbidden = bd.is_forbidden(c);
    for (Point p : bd)
        m_snapshot[p] = (is_forbidden[p] ? 0x1000u : 0);
}


inline void PlayoutFeatures::restore_snapshot(const Board& bd)
{
    m_point_value.copy_from(m_snapshot, bd.get_geometry());
}

inline void PlayoutFeatures::set_forbidden(const MoveInfo& info)
{
    auto p = info.begin();
    auto end = info.end();
    m_point_value[*p] = 0x1000u;
    // Loop over fixed size to help compiler unroll
    for (unsigned i = 1; i < PieceInfo::max_size; ++i)
    {
        if (++p == end)
            return;
        m_point_value[*p] = 0x1000u;
    }
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
    unsigned nu_local = 0;

    Color to_play = bd.get_to_play();
    Color second_color;
    if (bd.get_variant() == Variant::classic_3 && to_play.to_int() == 3)
        second_color = Color(bd.get_alt_player());
    else
        second_color = bd.get_second_color(to_play);
    auto& geo = bd.get_geometry();
    auto& moves = bd.get_moves();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-color variants)
    auto end = moves.end();
    auto begin = (end - moves.begin() < 3 ? moves.begin() : end - 3);
    for (auto i = begin; i != end; ++i)
    {
        Color c = i->color;
        if (c == to_play || c == second_color)
            continue;
        Move mv = i->move;
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = bd.get_move_info_ext(mv);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
        {
            if (is_forbidden[*j])
                continue;
            if ((m_point_value[*j] & 0x0fff) == 0)
            {
                m_local_points.get_unchecked(nu_local++) = *j;
                m_point_value[*j] |=
                        bd.is_attach_point(*j, to_play) ? 0x0002u : 0x0001u;
            }
            geo.for_each_adj(*j, [&](Point k) {
                if (! is_forbidden[k] && (m_point_value[k] & 0x0fff) == 0)
                {
                    m_local_points.get_unchecked(nu_local++) = k;
                    m_point_value[k] |=
                            bd.is_attach_point(k, to_play) ? 0x0002u : 0x0001u;
                }
            });
        }
        while (++j != end);
    }
    m_local_points.resize(nu_local);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PLAYOUT_FEATURES_H
