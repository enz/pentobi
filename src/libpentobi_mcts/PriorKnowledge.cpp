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

PriorKnowledge::PriorKnowledge(const Board& bd)
{
    init_variant(bd);
}

void PriorKnowledge::init_variant(const Board& bd)
{
    auto variant = bd.get_variant();
    m_variant = variant;
    auto& geo = bd.get_geometry();
    auto board_type = bd.get_board_type();
    auto piece_set = bd.get_piece_set();

    // Init m_dist_to_center
    auto width = static_cast<float>(geo.get_width());
    auto height = static_cast<float>(geo.get_height());
    float center_x = 0.5f * width - 0.5f;
    float center_y = 0.5f * height - 0.5f;
    bool is_trigon = (piece_set == PieceSet::trigon);
    float ratio = (is_trigon ? 1.732f : 1);
    for (Point p : geo)
    {
        auto x = static_cast<float>(geo.get_x(p));
        auto y = static_cast<float>(geo.get_y(p));
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
    switch (variant)
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

    // Init gammas. The values are learned using pentobi/src/learn_tool.
    Float gamma_piece_score_0;
    Float gamma_piece_score_1;
    Float gamma_piece_score_2;
    Float gamma_piece_score_3;
    Float gamma_piece_score_4;
    Float gamma_piece_score_5;
    Float gamma_piece_score_6;
    if (variant == Variant::duo || variant == Variant::junior)
    {
        Float temperature = 0.84f;
        // Tuned for duo
        m_gamma_point_other = exp(0.249f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.277f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = 1; // unused
        m_gamma_adj_occupied_other = exp(0.326f / temperature);
        m_gamma_adj_forbidden_other = exp(0.317f / temperature);
        m_gamma_adj_own_attach = exp(-0.977f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.398f / temperature);
        m_gamma_attach_to_play = exp(0.112f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.310f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.046f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.004f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.330f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.406f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.695f / temperature);
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(0.135f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.246f / temperature);
        gamma_piece_score_2 = exp(-0.268f / temperature);
        gamma_piece_score_3 = exp(-0.720f / temperature);
        gamma_piece_score_4 = exp(-0.284f / temperature);
        gamma_piece_score_5 = exp(1.038f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (variant == Variant::callisto_2)
    {
        Float temperature = 0.84f;
        m_gamma_point_other = exp(0.264f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.988f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = exp(-0.095f / temperature);
        m_gamma_adj_occupied_other = exp(-0.053f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.593f / temperature);
        m_gamma_adj_nonforbidden = exp(0.205f / temperature);
        m_gamma_attach_to_play = exp(-0.113f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.530f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(0.129f / temperature);
        m_gamma_attach_nonforbidden[1] = 1; // unused
        m_gamma_attach_nonforbidden[2] = 1; // unused
        m_gamma_attach_nonforbidden[3] = 1; // unused
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(0.182f / temperature);
        gamma_piece_score_0 = exp(0.614f / temperature);
        gamma_piece_score_1 = 1; // unused
        gamma_piece_score_2 = exp(-1.282f / temperature);
        gamma_piece_score_3 = exp(-0.634f / temperature);
        gamma_piece_score_4 = exp(0.353f / temperature);
        gamma_piece_score_5 = exp(0.950f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (variant == Variant::gembloq_2)
    {
        Float temperature = 0.84f;
        m_gamma_point_other = exp(0.047f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.210f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = 1; // unused
        m_gamma_adj_occupied_other = exp(0.312f / temperature);
        m_gamma_adj_forbidden_other = exp(0.343f / temperature);
        m_gamma_adj_own_attach = exp(0.304f / temperature);
        m_gamma_adj_nonforbidden = exp(0.024f / temperature);
        m_gamma_attach_to_play = exp(0.207f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.130f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.042f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(0.129f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.008f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(-0.068f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(1.165f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(-0.747f / temperature);
        gamma_piece_score_2 = exp(-0.766f / temperature);
        gamma_piece_score_3 = exp(-0.451f / temperature);
        gamma_piece_score_4 = exp(0.409f / temperature);
        gamma_piece_score_5 = exp(1.538f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (piece_set == PieceSet::trigon)
    {
        Float temperature = 0.84f;
        // Tuned for trigon_2
        m_gamma_point_other = exp(0.116f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.740f / temperature);
        m_gamma_point_second_color_attach = exp(0.064f / temperature);
        m_gamma_adj_connect = exp(0.891f / temperature);
        m_gamma_adj_occupied_other = exp(0.943f / temperature);
        m_gamma_adj_forbidden_other = exp(0.636f / temperature);
        m_gamma_adj_own_attach = exp(0.191f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.102f / temperature);
        m_gamma_attach_to_play = exp(-0.147f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.372f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.199f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.235f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.160f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(-0.000f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = exp(0.015f / temperature);
        m_gamma_local = exp(0.492f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.399f / temperature);
        gamma_piece_score_2 = exp(0.066f / temperature);
        gamma_piece_score_3 = exp(-0.526f / temperature);
        gamma_piece_score_4 = exp(-0.648f / temperature);
        gamma_piece_score_5 = exp(-0.330f / temperature);
        gamma_piece_score_6 = exp(1.021f / temperature);
    }
    else if (piece_set == PieceSet::nexos)
    {
        Float temperature = 0.84f;
        // Tuned for nexos_2
        m_gamma_point_other = exp(0.676f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.676f / temperature);
        m_gamma_point_second_color_attach = exp(0.021f / temperature);
        m_gamma_adj_connect = exp(0.100f / temperature);
        m_gamma_adj_occupied_other = exp(0.015f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.278f / temperature);
        m_gamma_adj_nonforbidden = exp(0.144f / temperature);
        m_gamma_attach_to_play = exp(0.026f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.071f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(0.015f / temperature);
        m_gamma_attach_nonforbidden[1] = 1; // unused
        m_gamma_attach_nonforbidden[2] = 1; // unused
        m_gamma_attach_nonforbidden[3] = 1; // unused
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = exp(0.064f / temperature);
        m_gamma_local = exp(0.839f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.319f / temperature);
        gamma_piece_score_2 = exp(-0.312f / temperature);
        gamma_piece_score_3 = exp(-0.463f / temperature);
        gamma_piece_score_4 = exp(0.456f / temperature);
        gamma_piece_score_5 = 1; // unused
        gamma_piece_score_6 = 1; // unused
    }
    else if (piece_set == PieceSet::callisto)
    {
        Float temperature = 0.84f;
        // Tuned for callisto_2_4
        m_gamma_point_other = exp(0.212f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.936f / temperature);
        m_gamma_point_second_color_attach = exp(0.057f / temperature);
        m_gamma_adj_connect = exp(0.097f / temperature);
        m_gamma_adj_occupied_other = exp(-0.087f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.474f / temperature);
        m_gamma_adj_nonforbidden = exp(0.044f / temperature);
        m_gamma_attach_to_play = exp(-0.130f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.439f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(0.145f / temperature);
        m_gamma_attach_nonforbidden[1] = 1; // unused
        m_gamma_attach_nonforbidden[2] = 1; // unused
        m_gamma_attach_nonforbidden[3] = 1; // unused
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = exp(-0.027f / temperature);
        m_gamma_local = exp(0.262f / temperature);
        gamma_piece_score_0 = exp(0.639f / temperature);
        gamma_piece_score_1 = 1; // unused
        gamma_piece_score_2 = exp(-1.209f / temperature);
        gamma_piece_score_3 = exp(-0.662f / temperature);
        gamma_piece_score_4 = exp(0.349f / temperature);
        gamma_piece_score_5 = exp(0.894f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (piece_set == PieceSet::gembloq)
    {
        Float temperature = 0.84f;
        // Tuned for gembloq_2_4
        m_gamma_point_other = exp(0.041f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.171f / temperature);
        m_gamma_point_second_color_attach = exp(0.057f / temperature);
        m_gamma_adj_connect = exp(0.381f / temperature);
        m_gamma_adj_occupied_other = exp(0.384f / temperature);
        m_gamma_adj_forbidden_other = exp(0.237f / temperature);
        m_gamma_adj_own_attach = exp(0.225f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.096f / temperature);
        m_gamma_attach_to_play = exp(0.176f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.140f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.223f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(0.088f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.002f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.050f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_second_color = exp(0.057f / temperature);
        m_gamma_local = exp(0.960f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(-0.511f / temperature);
        gamma_piece_score_2 = exp(-0.825f / temperature);
        gamma_piece_score_3 = exp(-0.682f / temperature);
        gamma_piece_score_4 = exp(0.157f / temperature);
        gamma_piece_score_5 = exp(1.828f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else
    {
        Float temperature = 0.84f;
        // Tuned for classic_2
        m_gamma_point_other = exp(0.111f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.807f / temperature);
        m_gamma_point_second_color_attach = exp(-0.296f / temperature);
        m_gamma_adj_connect = exp(0.583f / temperature);
        m_gamma_adj_occupied_other = exp(0.505f / temperature);
        m_gamma_adj_forbidden_other = exp(0.473f / temperature);
        m_gamma_adj_own_attach = exp(-0.743f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.060f / temperature);
        m_gamma_attach_to_play = exp(0.018f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.404f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.130f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(0.002f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.207f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.308f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.544f / temperature);
        m_gamma_attach_second_color = exp(-0.099f / temperature);
        m_gamma_local = exp(0.590f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.345f / temperature);
        gamma_piece_score_2 = exp(-0.188f / temperature);
        gamma_piece_score_3 = exp(-0.663f / temperature);
        gamma_piece_score_4 = exp(-0.284f / temperature);
        gamma_piece_score_5 = exp(0.817f / temperature);
        gamma_piece_score_6 = 1; // unused
    }

    for (Piece::IntType i = 0; i < bd.get_nu_uniq_pieces(); ++i)
        switch (static_cast<unsigned>(
                    bd.get_piece_info(Piece(i)).get_score_points()))
        {
        case 0: m_gamma_piece_score[Piece(i)] = gamma_piece_score_0; break;
        case 1: m_gamma_piece_score[Piece(i)] = gamma_piece_score_1; break;
        case 2: m_gamma_piece_score[Piece(i)] = gamma_piece_score_2; break;
        case 3: m_gamma_piece_score[Piece(i)] = gamma_piece_score_3; break;
        case 4: m_gamma_piece_score[Piece(i)] = gamma_piece_score_4; break;
        case 5: m_gamma_piece_score[Piece(i)] = gamma_piece_score_5; break;
        default: m_gamma_piece_score[Piece(i)] = gamma_piece_score_6; break;
        }
}

void PriorKnowledge::start_search(const Board& bd)
{
    if (bd.get_variant() != m_variant)
        init_variant(bd);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
