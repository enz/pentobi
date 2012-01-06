//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LastAttachPoints.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LastAttachPoints.h"

namespace libpentobi_mcts {

using libboardgame_util::ArrayList;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

void LastAttachPoints::init(const Board& bd)
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
        for (auto j = attach_points.begin(); j != attach_points.end(); ++j)
            if (! bd.is_forbidden(*j, c) && m_marker[*j] == 0)
            {
                m_points.push_back(*j);
                m_marker[*j] = 1;
            }
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
