//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PriorKnowledge.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PriorKnowledge.h"

#include <cmath>

namespace libpentobi_mcts {

using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::PointState;
using libpentobi_base::PieceInfo;
using libpentobi_base::PieceSet;

//-----------------------------------------------------------------------------

PriorKnowledge::PriorKnowledge()
{
    m_is_local.fill_all(false);
}

void PriorKnowledge::start_search(const Board& bd)
{
    auto& geo = bd.get_geometry();
    auto board_type = bd.get_board_type();
    auto piece_set = bd.get_piece_set();

    // Init m_dist_to_center
    float width = static_cast<float>(geo.get_width());
    float height = static_cast<float>(geo.get_height());
    float center_x = 0.5f * width - 0.5f;
    float center_y = 0.5f * height - 0.5f;
    bool is_trigon = (piece_set == PieceSet::trigon);
    float ratio = (is_trigon ? 1.732f : 1);
    for (Point p : geo)
    {
        float x = static_cast<float>(geo.get_x(p));
        float y = static_cast<float>(geo.get_y(p));
        float dx = x - center_x;
        float dy = ratio * (y - center_y);
        float d = sqrt(dx * dx + dy * dy);
        if (board_type == BoardType::classic)
            // Don't make a distinction between moves close enough to the
            // center in game variant Classic/Classic2
            d = max(d, 2.f);
        m_dist_to_center[p] = d;
    }
    m_dist_to_center[Point::null()] = numeric_limits<float>::max();

    // Init m_check_dist_to_center
    switch(bd.get_variant())
    {
    case Variant::classic:
    case Variant::classic_2:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 12;
        m_max_dist_diff = 0.3f;
        break;
    case Variant::classic_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 10;
        m_max_dist_diff = 0.3f;
        break;
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 3;
        m_max_dist_diff = 0.5f;
        break;
    case Variant::duo:
    case Variant::junior:
        m_check_dist_to_center.fill(false);
        break;
    case Variant::callisto:
    case Variant::callisto_2_4:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 8;
        m_max_dist_diff = 4;
        break;
    case Variant::callisto_2:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 4;
        m_max_dist_diff = 0;
        break;
    case Variant::callisto_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 6;
        m_max_dist_diff = 3;
        break;
    case Variant::nexos:
    case Variant::nexos_2:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 7;
        m_max_dist_diff = 0.3f;
        break;
    case Variant::gembloq:
    case Variant::gembloq_2_4:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 12;
        m_max_dist_diff = 0.5f;
        break;
    case Variant::gembloq_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 9;
        m_max_dist_diff = 0.5f;
        break;
    case Variant::gembloq_2:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 4;
        m_max_dist_diff = 0.5f;
        break;
    }

    if (piece_set != PieceSet::callisto)
        // Don't check dist to center if the position was setup in a way that
        // placed pieces but did not cover the starting point(s), otherwise the
        // search might not generate any moves (if no moves meet the
        // dist-to-center condition). Even if such positions cannot occur in
        // legal games, we still don't want the move generation to fail.
        for (Color c : bd.get_colors())
        {
            if (bd.get_nu_onboard_pieces(c) == 0)
                continue;
            bool is_starting_point_covered = false;
            for (Point p : bd.get_starting_points(c))
                if (bd.get_point_state(p) == PointState(c))
                {
                    is_starting_point_covered = true;
                    break;
                }
            if (! is_starting_point_covered)
                m_check_dist_to_center[c] = false;
        }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
