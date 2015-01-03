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
#include "libboardgame_util/MathUtil.h"
#include "libpentobi_base/AdjIterator.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_util::fast_exp;
using libpentobi_base::AdjIterator;
using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::GeometryIterator;
using libpentobi_base::Point;
using libpentobi_base::PointState;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

PriorKnowledge::PriorKnowledge()
{
    m_local_value.fill_all(0);
}

void PriorKnowledge::compute_features(const Board& bd, const MoveList& moves,
                                      bool check_dist_to_center,
                                      bool check_connect)
{
    auto to_play = bd.get_to_play();
    auto variant = bd.get_variant();
    Color second_color;
    // connect_color is the 2nd color of the player in game variants with 2
    // colors per player (connecting to_play and connect_color is good) and
    // to_play in other game variants (which disables the bonus without
    // needing an extra check below because adj_point_value is not used for
    // pieces of to_play because it is illegal for to_play to play there).
    Color connect_color;
    if (variant == Variant::classic_3 && to_play.to_int() == 3)
    {
        second_color = Color(bd.get_alt_player());
        connect_color = to_play;
    }
    else
    {
        second_color = bd.get_second_color(to_play);
        connect_color = second_color;
    }
    auto& bc = bd.get_board_const();
    auto& geo = bc.get_geometry();
    auto move_info_array = bc.get_move_info_array();
    auto move_info_ext_array = bc.get_move_info_ext_array();
    auto& is_forbidden = bd.is_forbidden(to_play);
    Grid<Float> point_value;
    Grid<Float> attach_point_value;
    Grid<Float> adj_point_value;
    for (GeometryIterator i(geo); i; ++i)
    {
        point_value[*i] = 1;
        auto s = bd.get_point_state(*i);
        if (is_forbidden[*i] && s != to_play)
            attach_point_value[*i] = -2.5;
        else
            attach_point_value[*i] = 0.5;
        if (! is_forbidden[*i])
        {
            if (bd.is_attach_point(*i, to_play))
                // Making own attach point forbidden is especially bad
                adj_point_value[*i] = -1;
            else
                // Creating new forbidden points is a bad thing
                adj_point_value[*i] = -0.1f;
        }
        else if (s == connect_color)
            // Connecting own colors is good
            adj_point_value[*i] = 1;
        else if (! s.is_empty())
            // Touching opponent is better than playing elsewhere (no need to
            // check if s == to_play, such moves are illegal)
            adj_point_value[*i] = 0.4f;
        else
            adj_point_value[*i] = 0;
    }
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
    {
        if (*i == to_play || *i == second_color)
            continue;
        auto& is_forbidden = bd.is_forbidden(*i);
        for (Point p : bd.get_attach_points(*i))
            if (! is_forbidden[p])
            {
                // Occupying opponent attach points is very good
                point_value[p] = 3.2f;
                for (AdjIterator j(geo, p); j; ++j)
                    if (! is_forbidden[*j])
                        // Occupying points adjacent to opponent attach points
                        // is good
                        point_value[*j] = max(point_value[*j], Float(2.5));
            }
    }
    if (variant == Variant::classic_2
            || (variant == Variant::classic_3 && second_color != to_play))
    {
        auto& is_forbidden_second_color = bd.is_forbidden(second_color);
        for (Point p : bd.get_attach_points(second_color))
            if (! is_forbidden_second_color[p])
            {
                // Occupying attach points of second color is bad
                point_value[p] -= 3.f;
                if (! is_forbidden[p])
                    // Sharing an attach point with second color is bad
                    attach_point_value[p] -= 1.f;
            }
    }
    m_max_heuristic = -numeric_limits<Float>::max();
    m_min_dist_to_center = numeric_limits<unsigned short>::max();
    m_has_connect_move = false;
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        auto mv_int = moves[i].to_int();
        auto& info = *(move_info_array + mv_int);
        auto& info_ext = *(move_info_ext_array + mv_int);
        auto& features = m_features[i];
        auto j = info.begin();
        auto end = info.end();
        Float heuristic = point_value[*j];
        unsigned local = m_local_value[*j];
        if (! check_dist_to_center)
            while (++j != end)
            {
                heuristic += point_value[*j];
                local += m_local_value[*j];
            }
        else
        {
            features.dist_to_center = m_dist_to_center[*j];
            while (++j != end)
            {
                heuristic += point_value[*j];
                local += m_local_value[*j];
                features.dist_to_center =
                    min(features.dist_to_center, m_dist_to_center[*j]);
            }
            m_min_dist_to_center =
                min(m_min_dist_to_center, features.dist_to_center);
        }
        j = info_ext.begin_attach();
        end = info_ext.end_attach();
        heuristic += attach_point_value[*j];
        while (++j != end)
            heuristic += attach_point_value[*j];
        j = info_ext.begin_adj();
        end = info_ext.end_adj();
        heuristic += adj_point_value[*j];
        if (! check_connect)
            while (++j != end)
                heuristic += adj_point_value[*j];
        else
        {
            features.connect = (bd.get_point_state(*j) == second_color);
            while (++j != end)
            {
                heuristic += adj_point_value[*j];
                if (bd.get_point_state(*j) == second_color)
                    features.connect = true;
            }
            if (features.connect)
                m_has_connect_move = true;
        }
        if (heuristic > m_max_heuristic)
            m_max_heuristic = heuristic;
        features.heuristic = heuristic;
        features.is_local = (local != 0);
    }
}

void PriorKnowledge::gen_children(const Board& bd, const MoveList& moves,
                                  bool is_symmetry_broken,
                                  Tree::NodeExpander& expander, Float init_val)
{
    if (moves.empty())
    {
        // The initialization value does not matter for a single child, but we
        // need to use SearchParamConst::child_min_count for the count to
        // avoid an assertion.
        expander.add_child(Move::pass(), init_val, 3);
        return;
    }
    init_local(bd);
    auto to_play = bd.get_to_play();
    auto nu_onboard_pieces = bd.get_nu_onboard_pieces();
    bool check_dist_to_center =
            (m_check_dist_to_center[to_play]
             && nu_onboard_pieces <= m_dist_to_center_max_pieces);
    bool check_connect =
        (bd.get_variant() == Variant::classic_2 && nu_onboard_pieces < 14);
    compute_features(bd, moves, check_dist_to_center, check_connect);
    if (! m_has_connect_move)
        check_connect = false;
    Move symmetric_mv = Move::null();
    bool has_symmetry_breaker = false;
    if (! is_symmetry_broken)
    {
        unsigned nu_moves = bd.get_nu_moves();
        if (to_play == Color(1) || to_play == Color(3))
        {
            if (nu_moves > 0)
            {
                ColorMove last = bd.get_move(nu_moves - 1);
                if (! last.is_pass())
                    symmetric_mv =
                        bd.get_move_info_ext_2(last.move).symmetric_move;
            }
        }
        else if (nu_moves > 0)
            for (Move mv : moves)
                if (bd.get_move_info_ext_2(mv).breaks_symmetry)
                {
                    has_symmetry_breaker = true;
                    break;
                }
    }
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        const auto& features = m_features[i];

        // In variant Classic, prune early moves that don't minimize dist to
        // center and moves that don't connect in the middle if connection is
        // possible
        if ((check_dist_to_center
             && features.dist_to_center != m_min_dist_to_center)
            || (check_connect && ! features.connect))
            continue;

        auto mv = moves[i];

        // Convert the heuristic, which is so far estimated in score points,
        // into a win/loss value in [0..1] by making it relative to the
        // heuristic of the best move and let it decrease exponentially with a
        // certain width. We could use exp(-c*x) here, but we use
        // 0.1+0.9*exp(-c*x) instead to avoid that the value is too close to
        // 0, because then it might never get explored in practice if the bias
        // term constant is small.
        Float heuristic = m_max_heuristic - features.heuristic;
        heuristic = 0.1f + 0.9f * fast_exp(-0.6f * heuristic);

        // Initialize value from heuristic and init_val, each with a count
        // of 1.5. If this is changed, SearchParamConst::child_min_count
        // should be updated.
        Float value = 1.5f * heuristic + 1.5f * init_val;
        Float count = 3;

        // If a symmetric draw is still possible, encourage exploring a move
        // that keeps or breaks the symmetry by adding 5 wins or 5 losses
        // (use 0.1 for a loss to avoid values too close to 0). See also the
        // comment in evaluate_playout()
        if (! symmetric_mv.is_null())
        {
            if (mv == symmetric_mv)
                value += 5;
            count += 5;
        }
        else if (has_symmetry_breaker
                 && ! bd.get_move_info_ext_2(mv).breaks_symmetry)
            continue;

        // Add 1 win for moves that are local responses to recent opponent
        // moves
        if (features.is_local)
        {
            value += 1;
            count += 1;
        }

        expander.add_child(mv, value / count, count);
    }
}

void PriorKnowledge::init_local(const Board& bd)
{
    for (Point p: m_local_points)
        m_local_value[p] = 0;
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
            if (m_local_value[*j] == 0)
                m_local_points.push_back(*j);
            m_local_value[*j] = 1;
            for (AdjIterator k(geo, *j); k; ++k)
                if (! is_forbidden[*k])
                {
                    if (m_local_value[*k] == 0)
                        m_local_points.push_back(*k);
                    if (m_local_value[*k] == 0 || m_local_value[*k] == 3)
                    {
                        m_local_value[*k] = 2;
                        for (AdjIterator l(geo, *k); l; ++l)
                            if (! is_forbidden[*l] && m_local_value[*l] == 0)
                            {
                                m_local_points.push_back(*l);
                                m_local_value[*l] = 3;
                            }
                    }
                }
        }
        while (++j != end);
    }
}

void PriorKnowledge::start_search(const Board& bd)
{
    auto& geo = bd.get_geometry();
    auto board_type = bd.get_board_type();

    // Init m_dist_to_center
    float width = static_cast<float>(geo.get_width());
    float height = static_cast<float>(geo.get_height());
    float center_x = 0.5f * width - 0.5f;
    float center_y = 0.5f * height - 0.5f;
    bool is_trigon = (board_type == BoardType::trigon
                      || board_type == BoardType::trigon_3);
    float ratio = (is_trigon ? 1.732f : 1);
    for (GeometryIterator i(geo); i; ++i)
    {
        float x = static_cast<float>(i->get_x(geo.get_width()));
        float y = static_cast<float>(i->get_y(geo.get_width()));
        float dx = x - center_x;
        float dy = ratio * (y - center_y);
        // Multiply Euklidian distance by 4, so that distances that differ
        // by max. 0.25 are treated as equal
        float d = round(4 * sqrt(dx * dx + dy * dy));
        if (board_type == BoardType::classic)
            // Don't make a distinction between moves close enough to the
            // center in game variant Classic/Classic2
            d = max(d, 10.f);
        m_dist_to_center[*i] = static_cast<unsigned short>(d);
    }

    // Init m_check_dist_to_center
    switch(bd.get_variant())
    {
    case Variant::classic:
    case Variant::classic_2:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 12;
        break;
    case Variant::classic_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 10;
        break;
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        m_check_dist_to_center.fill(true);
        m_dist_to_center_max_pieces = 4;
        break;
    default:
        m_check_dist_to_center.fill(false);
    }
    // Don't check dist to center if the position was setup in a way that
    // placed pieces but did not cover the starting point(s), otherwise the
    // search might not generate any moves (if no moves meet the dist-to-center
    // condition). Even if such positions cannot occur in legal games, we still
    // don't want the move generation to fail.
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
    {
        if (bd.get_nu_onboard_pieces(*i) == 0)
            continue;
        bool is_starting_point_covered = false;
        for (Point p : bd.get_starting_points(*i))
            if (bd.get_point_state(p) == PointState(*i))
            {
                is_starting_point_covered = true;
                break;
            }
        if (! is_starting_point_covered)
            m_check_dist_to_center[*i] = false;
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
