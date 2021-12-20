//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PriorKnowledge.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PriorKnowledge.h"

#include <cmath>

namespace libpentobi_mcts {

using libpentobi_base::BoardType;
using libpentobi_base::GeometryType;
using libpentobi_base::PointState;
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
    auto geometry_type = bd.get_geometry_type();

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
        m_gamma_point_other = exp(0.394f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.399f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = 1; // unused
        m_gamma_adj_occupied_other = exp(0.359f / temperature);
        m_gamma_adj_forbidden_other = exp(0.404f / temperature);
        m_gamma_adj_own_attach = exp(-1.164f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.461f / temperature);
        m_gamma_attach_to_play = exp(-0.082f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.305f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.200f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.116f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.331f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.588f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.923f / temperature);
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(0.336f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.330f / temperature);
        gamma_piece_score_2 = exp(-0.402f / temperature);
        gamma_piece_score_3 = exp(-0.845f / temperature);
        gamma_piece_score_4 = exp(-0.245f / temperature);
        gamma_piece_score_5 = exp(1.149f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (variant == Variant::callisto_2)
    {
        Float temperature = 0.84f;
        m_gamma_point_other = exp(0.305f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(2.047f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = exp(-0.022f / temperature);
        m_gamma_adj_occupied_other = exp(-0.049f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.631f / temperature);
        m_gamma_adj_nonforbidden = exp(0.221f / temperature);
        m_gamma_attach_to_play = exp(-0.273f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.597f / temperature);
        m_gamma_attach_nonforbidden[0] = 1; // unused
        m_gamma_attach_nonforbidden[1] = exp(-0.070f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.140f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.075f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.199f / temperature);
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(0.203f / temperature);
        gamma_piece_score_0 = exp(0.942f / temperature);
        gamma_piece_score_1 = 1; // unused
        gamma_piece_score_2 = exp(-1.642f / temperature);
        gamma_piece_score_3 = exp(-0.800f / temperature);
        gamma_piece_score_4 = exp(0.436f / temperature);
        gamma_piece_score_5 = exp(1.082f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (variant == Variant::gembloq_2)
    {
        Float temperature = 0.84f;
        m_gamma_point_other = exp(0.120f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.315f / temperature);
        m_gamma_point_second_color_attach = 1; // unused
        m_gamma_adj_connect = 1; // unused
        m_gamma_adj_occupied_other = exp(0.350f / temperature);
        m_gamma_adj_forbidden_other = exp(0.511f / temperature);
        m_gamma_adj_own_attach = exp(0.285f / temperature);
        m_gamma_adj_nonforbidden = exp(0.095f / temperature);
        m_gamma_attach_to_play = exp(0.181f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.127f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.165f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.031f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.058f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(-0.057f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = 1; // unused
        m_gamma_local = exp(1.109f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(-0.468f / temperature);
        gamma_piece_score_2 = exp(-0.647f / temperature);
        gamma_piece_score_3 = exp(-0.386f / temperature);
        gamma_piece_score_4 = exp(0.250f / temperature);
        gamma_piece_score_5 = exp(1.283f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (piece_set == PieceSet::trigon)
    {
        Float temperature = 0.84f;
        // Tuned for trigon_2
        m_gamma_point_other = exp(0.182f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.828f / temperature);
        m_gamma_point_second_color_attach = exp(0.016f / temperature);
        m_gamma_adj_connect = exp(1.032f / temperature);
        m_gamma_adj_occupied_other = exp(1.024f / temperature);
        m_gamma_adj_forbidden_other = exp(0.671f / temperature);
        m_gamma_adj_own_attach = exp(0.193f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.155f / temperature);
        m_gamma_attach_to_play = exp(-0.153f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.382f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.220f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.263f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.155f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.059f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = exp(-0.051f / temperature);
        m_gamma_local = exp(0.536f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.453f / temperature);
        gamma_piece_score_2 = exp(0.083f / temperature);
        gamma_piece_score_3 = exp(-0.620f / temperature);
        gamma_piece_score_4 = exp(-0.687f / temperature);
        gamma_piece_score_5 = exp(-0.373f / temperature);
        gamma_piece_score_6 = exp(1.153f / temperature);
    }
    else if (piece_set == PieceSet::nexos)
    {
        Float temperature = 0.84f;
        // Tuned for nexos_2
        m_gamma_point_other = exp(0.601f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(1.525f / temperature);
        m_gamma_point_second_color_attach = exp(0.112f / temperature);
        m_gamma_adj_connect = exp(0.026f / temperature);
        m_gamma_adj_occupied_other = exp(0.002f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.251f / temperature);
        m_gamma_adj_nonforbidden = exp(0.036f / temperature);
        m_gamma_attach_to_play = exp(0.021f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.037f / temperature);
        m_gamma_attach_nonforbidden[0] = 1; // unused
        m_gamma_attach_nonforbidden[1] = exp(0.074f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.104f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(-0.067f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(-0.113f / temperature);
        m_gamma_attach_nonforbidden[5] = exp(-0.035f / temperature);
        m_gamma_attach_nonforbidden[6] = exp(0.127f / temperature);
        m_gamma_attach_second_color = exp(0.075f / temperature);
        m_gamma_local = exp(1.101f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(-0.167f / temperature);
        gamma_piece_score_2 = exp(-0.387f / temperature);
        gamma_piece_score_3 = exp(-0.306f / temperature);
        gamma_piece_score_4 = exp(0.852f / temperature);
        gamma_piece_score_5 = 1; // unused
        gamma_piece_score_6 = 1; // unused
    }
    else if (geometry_type == GeometryType::callisto)
    {
        Float temperature = 0.84f;
        // Tuned for callisto_2_4
        m_gamma_point_other = exp(0.310f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(2.043f / temperature);
        m_gamma_point_second_color_attach = exp(-0.017f / temperature);
        m_gamma_adj_connect = exp(0.189f / temperature);
        m_gamma_adj_occupied_other = exp(-0.033f / temperature);
        m_gamma_adj_forbidden_other = 1; // unused
        m_gamma_adj_own_attach = exp(-0.500f / temperature);
        m_gamma_adj_nonforbidden = exp(0.100f / temperature);
        m_gamma_attach_to_play = exp(-0.239f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.545f / temperature);
        m_gamma_attach_nonforbidden[0] = 1; // unused
        m_gamma_attach_nonforbidden[1] = exp(0.152f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.159f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.104f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.122f / temperature);
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = exp(-0.107f / temperature);
        m_gamma_local = exp(0.182f / temperature);
        gamma_piece_score_0 = exp(0.823f / temperature);
        gamma_piece_score_1 = 1; // unused
        gamma_piece_score_2 = exp(-1.507f / temperature);
        gamma_piece_score_3 = exp(-0.726f / temperature);
        gamma_piece_score_4 = exp(0.436f / temperature);
        gamma_piece_score_5 = exp(1.003f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else if (piece_set == PieceSet::gembloq)
    {
        Float temperature = 0.84f;
        // Tuned for gembloq_2_4
        m_gamma_point_other = exp(0.174f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.304f / temperature);
        m_gamma_point_second_color_attach = exp(0.098f / temperature);
        m_gamma_adj_connect = exp(0.296f / temperature);
        m_gamma_adj_occupied_other = exp(0.314f / temperature);
        m_gamma_adj_forbidden_other = exp(0.141f / temperature);
        m_gamma_adj_own_attach = exp(0.138f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.195f / temperature);
        m_gamma_attach_to_play = exp(0.191f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.129f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.123f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(0.104f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(-0.005f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.045f / temperature);
        m_gamma_attach_nonforbidden[4] = 1; // unused
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = exp(0.128f / temperature);
        m_gamma_local = exp(1.078f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(-0.094f / temperature);
        gamma_piece_score_2 = exp(-0.563f / temperature);
        gamma_piece_score_3 = exp(-0.661f / temperature);
        gamma_piece_score_4 = exp(-0.021f / temperature);
        gamma_piece_score_5 = exp(1.349f / temperature);
        gamma_piece_score_6 = 1; // unused
    }
    else
    {
        // Tuned for classic_2
        Float temperature = 0.84f;
        m_gamma_point_other = exp(0.137f / temperature);
        m_gamma_point_opp_attach_or_nb = exp(0.898f / temperature);
        m_gamma_point_second_color_attach = exp(-0.248f / temperature);
        m_gamma_adj_connect = exp(0.616f / temperature);
        m_gamma_adj_occupied_other = exp(0.568f / temperature);
        m_gamma_adj_forbidden_other = exp(0.544f / temperature);
        m_gamma_adj_own_attach = exp(-0.849f / temperature);
        m_gamma_adj_nonforbidden = exp(-0.115f / temperature);
        m_gamma_attach_to_play = exp(0.007f / temperature);
        m_gamma_attach_forbidden_other = exp(-0.439f / temperature);
        m_gamma_attach_nonforbidden[0] = exp(-0.177f / temperature);
        m_gamma_attach_nonforbidden[1] = exp(-0.002f / temperature);
        m_gamma_attach_nonforbidden[2] = exp(0.232f / temperature);
        m_gamma_attach_nonforbidden[3] = exp(0.342f / temperature);
        m_gamma_attach_nonforbidden[4] = exp(0.694f / temperature);
        m_gamma_attach_nonforbidden[5] = 1; // unused
        m_gamma_attach_nonforbidden[6] = 1; // unused
        m_gamma_attach_second_color = exp(-0.011f / temperature);
        m_gamma_local = exp(0.610f / temperature);
        gamma_piece_score_0 = 1; // unused
        gamma_piece_score_1 = exp(0.476f / temperature);
        gamma_piece_score_2 = exp(-0.316f / temperature);
        gamma_piece_score_3 = exp(-0.842f / temperature);
        gamma_piece_score_4 = exp(-0.301f / temperature);
        gamma_piece_score_5 = exp(0.969f / temperature);
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
