//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "State.h"

#include <cmath>
#include "libboardgame_util/FmtSaver.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/BoardUtil.h"
#include "libpentobi_base/Geometry.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::Tree;
using libboardgame_util::log;
using libboardgame_util::FmtSaver;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::GeometryIterator;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

const bool use_prior_knowledge = true;

/** Return the symmetric point state for symmetry detection.
    Only used for Variant::duo. Returns the other color or empty, if the
    given point state is empty. */
PointState get_symmetric_state(Color c)
{
    if (c == Color(0))
        return Color(1);
    else if (c == Color(1))
        return Color(0);
    else if (c == Color(2))
        return Color(3);
    else
    {
        LIBBOARDGAME_ASSERT(c == Color(3));
        return Color(2);
    }
}

} // namespace

//-----------------------------------------------------------------------------

SharedConst::SharedConst(const Color& to_play)
    : board(nullptr),
      to_play(to_play),
      detect_symmetry(true),
      avoid_symmetric_draw(true),
      score_modification(0.1f)
{
    // Game variant and position dependent variables are initialized in
    // libpentobi_mcts::Search::start_search()
}

//-----------------------------------------------------------------------------

State::State(Variant initial_variant, const SharedConst& shared_const)
  : m_shared_const(shared_const),
    m_bd(initial_variant),
    m_tmp_moves(new MoveList())
{
}

State::~State() throw()
{
}

inline void State::add_move(MoveList& moves, Move mv, double gamma)
{
    m_total_gamma += gamma;
    m_cumulative_gamma[moves.size()] = m_total_gamma;
    moves.push_back(mv);
}

inline void State::add_moves(Point p, Color c,
                             const Board::PiecesLeftList& pieces_considered)
{
    auto& moves = *m_moves[c];
    auto& marker = m_marker[c];
    auto adj_status = m_bd.get_adj_status(p, c);
    auto& is_forbidden = m_bd.is_forbidden(c);
    double gamma;
    for (Piece piece : pieces_considered)
    {
        auto move_candidates = get_moves(c, piece, p, adj_status);
        for (auto i = move_candidates.begin(); i != move_candidates.end(); ++i)
            if (! marker[*i]
                && check_move(is_forbidden, get_move_info(*i), gamma))
            {
                marker.set(*i);
                add_move(moves, *i, gamma);
            }
    }
    m_moves_added_at[c].set(p);
}

inline void State::add_moves(Point p, Color c, Piece piece,
                             unsigned adj_status)
{
    auto& moves = *m_moves[c];
    auto& marker = m_marker[c];
    auto move_candidates = get_moves(c, piece, p, adj_status);
    auto& is_forbidden = m_bd.is_forbidden(c);
    double gamma;
    for (auto i = move_candidates.begin(); i != move_candidates.end(); ++i)
        if (! marker[*i] && check_move(is_forbidden, get_move_info(*i), gamma))
        {
            marker.set(*i);
            add_move(moves, *i, gamma);
        }
}

void State::add_starting_moves(Color c,
                               const Board::PiecesLeftList& pieces_considered,
                               bool with_gamma)
{
    // Using only one starting point (if game variant has more than one) not
    // only reduces the branching factor but is also necessary because
    // update_moves() assumes that a move stays legal if the forbidden
    // status for all of its points does not change.
    Point p = find_best_starting_point(c);
    if (p.is_null())
        return;
    auto& moves = *m_moves[c];
    auto& marker = m_marker[c];
    auto& is_forbidden = m_bd.is_forbidden(c);
    // We use adj_status 0 and don't check if moves should be in the local
    // list, which is appropriate given the starting point locations in the
    // standard game variants and assuming normal game play with alternating
    // moves and adj_status is only an optimization and local moves only a
    // heuristic anyway. However, we check if all the moves are legal to avoid
    // the generation of illegal moves in non-standard board positions.
    for (Piece piece : pieces_considered)
    {
        auto piece_size = m_bd.get_piece_info(piece).get_size();
        for (Move mv : get_moves(c, piece, p, 0))
        {
            LIBBOARDGAME_ASSERT(! marker[mv]);
            if (check_move_without_gamma(is_forbidden, mv))
            {
                marker.set(mv);
                if (with_gamma)
                    add_move(moves, mv, m_gamma_piece_size[piece_size]);
                else
                    moves.push_back(mv);
            }
        }
    }
}

/** Check if move is not forbidden and compute/handle its local value in the
    same loop. */
bool State::check_move(const Grid<bool>& is_forbidden, const MoveInfo& info,
                       double& gamma)
{
    auto i = info.begin();
    if (is_forbidden[*i])
        return false;
    LocalValue::Compute local(*i, m_local_value);
    unsigned piece_size = info.size();
    auto end = i + piece_size;
    while (++i != end)
    {
        if (is_forbidden[*i])
            return false;
        local.add_move_point(*i, m_local_value);
    }
    gamma = m_gamma_piece_size[piece_size];
    if (local.has_local())
    {
        if (local.has_no_attach_or_adj())
            // Only 2nd-order adjacent to opponent attach point. Don't care how
            // many
            gamma *= 1e4;
        else
        {
            // Ignore 2nd-order adj. to opp. attach point if we have opp. attach
            // points or adj. to opp. attach point. Only care if we have any
            // adj. to opp. attach points, not how many.
            gamma *= m_gamma_nu_attach[local.get_nu_attach()];
            if (local.has_adj_attach())
                gamma *= 1e5;
        }
    }
    return true;
}

bool State::check_move_without_gamma(const Grid<bool>& is_forbidden, Move mv)
{
    auto& info = get_move_info(mv);
    auto i = info.begin();
    if (is_forbidden[*i])
        return false;
    auto end = info.end();
    while (++i != end)
        if (is_forbidden[*i])
            return false;
    return true;
}

void State::compute_features(bool check_dist_to_center, bool check_connect)
{
    auto to_play = m_bd.get_to_play();
    auto second_color = m_bd.get_second_color(to_play);
    const bool is_classic_2 = (m_bd.get_variant() == Variant::classic_2);
    auto& moves = *m_moves[to_play];
    auto& geo = m_bc->get_geometry();
    auto& is_forbidden = m_bd.is_forbidden(to_play);
    Grid<Float> point_value;
    Grid<Float> attach_point_value;
    Grid<Float> adj_point_value;
    for (GeometryIterator i(geo); i; ++i)
    {
        point_value[*i] = 1;
        auto s = m_bd.get_point_state(*i);
        if (is_forbidden[*i] && s != to_play)
            attach_point_value[*i] = -2.5;
        else
            attach_point_value[*i] = 0.5;
        if (! is_forbidden[*i])
        {
            if (m_bd.is_attach_point(*i, to_play))
                // Making own attach point forbidden is especially bad
                adj_point_value[*i] = -1;
            else
                // Creating new forbidden points is a bad thing
                adj_point_value[*i] = -0.1f;
        }
        else if (s == second_color)
            // Connecting 2 player colors in 2-colors-per-player game variants
            // is good (in other variants second_color is the same as to_play
            // but there it doesn't matter what adj_point_value[*i] is because
            // moves adjacent to to_play are not legal anyway).
            adj_point_value[*i] = 1;
        else
            adj_point_value[*i] = 0;
    }
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        if (*i == to_play || *i == second_color)
            continue;
        auto& is_forbidden = m_bd.is_forbidden(*i);
        for (Point p : m_bd.get_attach_points(*i))
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
    if (is_classic_2)
    {
        auto& is_forbidden_second_color = m_bd.is_forbidden(second_color);
        for (Point p : m_bd.get_attach_points(second_color))
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
    m_min_dist_to_center = numeric_limits<unsigned>::max();
    m_has_connect_move = false;
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        auto& info = get_move_info(moves[i]);
        auto& info_ext = get_move_info_ext(moves[i]);
        auto& features = m_features[i];
        Float heuristic = 0;
        if (! check_dist_to_center)
        {
            auto j = info.begin();
            auto end = info.end();
            do
                heuristic += point_value[*j];
            while (++j != end);
        }
        else
        {
            features.dist_to_center = numeric_limits<unsigned>::max();
            auto j = info.begin();
            auto end = info.end();
            do
            {
                heuristic += point_value[*j];
                features.dist_to_center =
                    min(features.dist_to_center, m_dist_to_center[*j]);
            }
            while (++j != end);
            m_min_dist_to_center =
                min(m_min_dist_to_center, features.dist_to_center);
        }
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
            heuristic += attach_point_value[*j];
        while (++j != end);
        j = info_ext.begin_adj();
        end = info_ext.end_adj();
        if (! check_connect)
        {
            do
                heuristic += adj_point_value[*j];
            while (++j != end);
        }
        else
        {
            features.connect = false;
            do
            {
                heuristic += adj_point_value[*j];
                if (m_bd.get_point_state(*j) == second_color)
                    features.connect = true;
            }
            while (++j != end);
            if (features.connect)
                m_has_connect_move = true;
        }
        if (heuristic > m_max_heuristic)
            m_max_heuristic = heuristic;
        features.heuristic = heuristic;
    }
}

bool State::check_symmetry_broken()
{
    Color to_play = m_bd.get_to_play();
    if (to_play == Color(0) || to_play == Color(2))
    {
        // First player to play: the symmetry is broken if the position is
        // not symmetric.
        for (BoardIterator i(m_bd); i; ++i)
        {
            PointState s1 = m_bd.get_point_state(*i);
            if (! s1.is_empty())
            {
                Point symm_p = m_shared_const.symmetric_points[*i];
                PointState s2 = m_bd.get_point_state(symm_p);
                if (s2 != get_symmetric_state(s1.to_color()))
                    return true;
            }
        }
    }
    else
    {
        // Second player to play: the symmetry is broken if the second player
        // cannot copy the first player's last move to make the position
        // symmetric again.
        unsigned nu_moves = m_bd.get_nu_moves();
        if (nu_moves == 0)
            // Don't try to handle the case if the second player has to play as
            // first move (e.g. in setup positions)
            return true;
        Color previous_color = m_bd.get_previous(to_play);
        ColorMove last_mv = m_bd.get_move(nu_moves - 1);
        if (last_mv.color != previous_color || last_mv.move.is_pass())
            // Don't try to handle non-alternating moves or pass moves in
            // board history
            return true;
        auto& info = get_move_info(last_mv.move);
        for (BoardIterator i(m_bd); i; ++i)
        {
            PointState s1 = m_bd.get_point_state(*i);
            if (! s1.is_empty())
            {
                Point symm_p = m_shared_const.symmetric_points[*i];
                PointState s2 = m_bd.get_point_state(symm_p);
                if (s2 != get_symmetric_state(s1.to_color()))
                    if (! (info.contains(*i) && s2.is_empty()))
                        return true;
            }
        }
    }
    return false;
}

void State::dump(ostream& out) const
{
    out << "pentobi_mcts::State:\n";
    libpentobi_base::boardutil::dump(m_bd, out);
}

array<Float,4> State::evaluate_playout()
{
    // Always evaluate symmetric positions as a draw in the playouts. This
    // will encourage the first player to break the symmetry and the second
    // player to preserve it.
    if (! m_is_symmetry_broken
        && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
    {
        if (log_simulations)
            log("Result: 0.5 (symmetry)");
        array<Float,4> result;
        for (ColorIterator i(m_nu_colors); i; ++i)
            result[(*i).to_int()] = 0.5;
        return result;
    }

    return evaluate_terminal();
}

/** Get the game result for each color.
    The result is 0,0.5,1 for loss/tie/win in 2-player variants. If there
    are n &gt; 2 players, this is generalized in the following way: The scores
    are sorted in ascending order. Each rank r_i (i in 0..n-1) is assigned
    a result value of r_i/(n-1). If multiple players have the same score,
    the result value is the average of all ranks with this score. So being
    the single winner still gives the result 1 and having the lowest score
    gives the result 0. Being the single winner is better than sharing the
    best place, which is better than getting the second place, etc.

    There is small modification applied to the result that encorages wins
    with larger scores. */
array<Float,4> State::evaluate_terminal()
{
    unsigned nu_players = m_bd.get_nu_players();
    ColorMap<Float> points;
    ColorMap<Float> score;
    for (ColorIterator i(m_nu_colors); i; ++i)
        points[*i] = Float(m_bd.get_points_with_bonus(*i));
    for (Color::IntType i = 0; i < nu_players; ++i)
        score[Color(i)] = Float(m_bd.get_score(Color(i)));
    if (m_nu_colors > nu_players)
    {
        LIBBOARDGAME_ASSERT(m_nu_colors == 4);
        score[Color(2)] = score[Color(0)];
        score[Color(3)] = score[Color(1)];
    }
    array<Float,Color::range> sorted_points;
    if (nu_players > 2)
    {
        for (ColorIterator i(m_nu_colors); i; ++i)
            sorted_points[(*i).to_int()] = points[*i];
        sort(sorted_points.begin(), sorted_points.begin() + m_nu_colors);
    }
    array<Float,4> result_array;
    for (Color::IntType i = 0; i < nu_players; ++i)
    {
        Color c(i);
        Float game_result;
        if (nu_players == 2)
        {
            if (score[c] > 0)
                game_result = 1;
            else if (score[c] < 0)
                game_result = 0;
            else
                game_result = 0.5;
        }
        else
        {
            game_result = 0;
            Float n = 0;
            for (Color::IntType j = 0; j < m_nu_colors; ++j)
                if (sorted_points[j] == points[c])
                {
                    game_result += Float(j) / Float(m_nu_colors - 1);
                    ++n;
                }
            game_result /= n;
        }

        Float score_modification = m_shared_const.score_modification;
        // Apply score modification. Example: If score modification is 0.1,
        // the game result is rescaled to [0..0.9] and the score modification
        // is added with 0.05 as the middle (corresponding to score 0).
        Float result =
            (1.f - score_modification) * game_result
            + 0.5f * (score_modification
                      + score[c] * m_score_modification_factor);
        result_array[i] = result;
        if (log_simulations)
            log() << "Result color " << c << ": score=" << score[c]
                  << " game_result=" << game_result << " result=" << result
                  << '\n';
    }
    if (m_nu_colors > nu_players)
    {
        LIBBOARDGAME_ASSERT(m_nu_colors == 4);
        result_array[2] = result_array[0];
        result_array[3] = result_array[1];
    }
    return result_array;
}

Point State::find_best_starting_point(Color c) const
{
    // We use the starting point that maximizes the distance to occupied
    // starting points, especially to the ones occupied by the player (their
    // distance is weighted with a factor of 2)
    Point best = Point::null();
    float max_distance = -1;
    bool is_trigon = (m_bd.get_board_type() == BoardType::trigon
                      || m_bd.get_board_type() == BoardType::trigon_3);
    float ratio = (is_trigon ? 1.732f : 1);
    for (Point p : m_bd.get_starting_points(c))
    {
        if (m_bd.is_forbidden(p, c))
            continue;
        float px = static_cast<float>(p.get_x());
        float py = static_cast<float>(p.get_y());
        float d = 0;
        for (ColorIterator i(m_nu_colors); i; ++i)
        {
            for (Point pp : m_bd.get_starting_points(*i))
            {
                PointState s = m_bd.get_point_state(pp);
                if (! s.is_empty())
                {
                    float ppx = static_cast<float>(pp.get_x());
                    float ppy = static_cast<float>(pp.get_y());
                    float dx = ppx - px;
                    float dy = ratio * (ppy - py);
                    float weight = 1;
                    if (s == c || s == m_bd.get_second_color(c))
                        weight = 2;
                    d += weight * sqrt(dx * dx + dy * dy);
                }
            }
        }
        if (d > max_distance)
        {
            best = p;
            max_distance = d;
        }
    }
    return best;
}

bool State::gen_playout_move(Move lgr1, Move lgr2, Move& mv)
{
    if (m_nu_passes == m_nu_colors)
        return false;

    if (! m_is_symmetry_broken
        && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
    {
        // See also the comment in evaluate_playout()
        if (log_simulations)
            log("Terminate playout. Symmetry not broken.");
        return false;
    }

    ++m_nu_playout_moves;
    if (lgr2.is_regular() && m_bd.is_legal(lgr2))
    {
        if (log_simulations)
            log("Playing last good reply 2");
        ++m_nu_last_good_reply_moves;
        mv = lgr2;
        return true;
    }
    if (lgr1.is_regular() && m_bd.is_legal(lgr1))
    {
        if (log_simulations)
            log("Playing last good reply 1");
        ++m_nu_last_good_reply_moves;
        mv = lgr1;
        return true;
    }

    Color to_play;
    while (true)
    {
        to_play = m_bd.get_to_play();
        if (! m_is_move_list_initialized[to_play])
            init_moves_with_gamma(to_play);
        else if (m_has_moves[to_play])
            update_moves(to_play);
        if ((m_has_moves[to_play] = ! m_moves[to_play]->empty()))
            break;
        if (m_nu_passes + 1 == m_nu_colors)
            return false;
        if (m_check_terminate_early && m_bd.get_score(to_play) < 0
            && ! m_has_moves[m_bd.get_second_color(to_play)])
        {
            if (log_simulations)
                log("Terminate early (no moves and negative score)");
            return false;
        }
        ++m_nu_passes;
        m_bd.set_to_play(m_bd.get_next(to_play));
        // Don't try to handle symmetry after pass moves
        m_is_symmetry_broken = true;
    }

    auto& moves = *m_moves[to_play];
    if (log_simulations)
        log() << "Moves: " << moves.size() << ", total gamma: "
              << m_total_gamma << "\n";
    auto begin = m_cumulative_gamma.begin();
    auto end = begin + moves.size();
    auto random = m_total_gamma * m_random.generate_double();
    auto pos = lower_bound(begin, end, random);
    LIBBOARDGAME_ASSERT(pos != end);
    mv = moves[pos - begin];
    return true;
}

bool State::gen_and_play_playout_move(Move lgr1, Move lgr2)
{
    Move mv;
    if (gen_playout_move(lgr1, lgr2, mv))
    {
        play_playout(mv);
        return true;
    }
    return false;
}

void State::gen_children(Tree::NodeExpander& expander, Float init_val)
{
    if (m_nu_passes == m_nu_colors)
        return;
    Color to_play = m_bd.get_to_play();

    init_moves_without_gamma(to_play);

    const auto& moves = *m_moves[to_play];
    if (moves.empty())
    {
        expander.add_child(Move::pass(), 0.5, 0);
        return;
    }
    if (! use_prior_knowledge)
    {
        for (Move mv : moves)
            expander.add_child(mv, 0.5, 0);
        return;
    }
    auto board_type = m_bd.get_board_type();
    auto nu_onboard_pieces = m_bd.get_nu_onboard_pieces();
    bool check_dist_to_center =
        ((board_type == BoardType::classic && nu_onboard_pieces < 13)
         || (board_type == BoardType::trigon && nu_onboard_pieces < 5)
         || (board_type == BoardType::trigon_3 && nu_onboard_pieces < 5));
    bool check_connect =
        (board_type == BoardType::classic && nu_onboard_pieces < 14);
    compute_features(check_dist_to_center, check_connect);
    if (! m_has_connect_move)
        check_connect = false;
    Move symmetric_mv = Move::null();
    bool has_symmetry_breaker = false;
    if (! m_is_symmetry_broken)
    {
        unsigned nu_moves = m_bd.get_nu_moves();
        if (to_play == Color(1) || to_play == Color(3))
        {
            if (nu_moves > 0)
            {
                ColorMove last = m_bd.get_move(nu_moves - 1);
                if (! last.is_pass())
                    symmetric_mv =
                        m_bd.get_move_info_ext_2(last.move).symmetric_move;
            }
        }
        else if (nu_moves > 0)
            for (Move mv : moves)
                if (m_bd.get_move_info_ext_2(mv).breaks_symmetry)
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
        // certain width. We could use exp(-c*x) here, but we use (a piecewise
        // linear approximation of) 0.1+0.9*exp(-c*x) instead to avoid that
        // the value is too close to 0, because then it might never get
        // explored in practice if the bias term constant is small.
        const Float exp_width = 0.6f;
        Float heuristic = m_max_heuristic - features.heuristic;
        if (heuristic > 10)
            heuristic = 0.100f;
        else if (heuristic > 2)
            heuristic = 0.252f - (0.015f * exp_width) * heuristic;
        else if (heuristic > 0.5)
            heuristic = 0.787f - (0.283f * exp_width) * heuristic;
        else
            heuristic = 1.000f - (0.708f * exp_width) * heuristic;

        // Initialize value from heuristic and init_val, each with a count
        // of 1.5
        Float value = 0.5f * heuristic + 0.5f * init_val;
        Float count = 3;

        // If a symmetric draw is still possible, encourage exploring a move
        // that keeps or breaks the symmetry by adding 5 wins or 5 losses
        // (use 0.1 for a loss to avoid values too close to 0). See also the
        // comment in evaluate_playout()
        if (! symmetric_mv.is_null())
        {
            if (mv == symmetric_mv)
                value = (3.f / 8) * value + (5.f / 8) * 1.0f;
            else
                value = (3.f / 8) * value + (5.f / 8) * 0.1f;
            count = 8;
        }
        else if (has_symmetry_breaker)
        {
            if (m_bd.get_move_info_ext_2(mv).breaks_symmetry)
                value = (3.f / 8) * value + (5.f / 8) * 1.0f;
            else
                value = (3.f / 8) * value + (5.f / 8) * 0.1f;
            count = 8;
        }

        expander.add_child(mv, value, count);
    }
}

inline const PieceMap<bool>& State::get_pieces_considered() const
{
    // Use number of on-board pieces for move number to handle the case where
    // there are more pieces on the board than moves (setup positions)
    unsigned nu_moves = m_bd.get_nu_onboard_pieces();
    if (nu_moves >= m_shared_const.min_move_all_considered
        || m_force_consider_all_pieces)
        return m_shared_const.is_piece_considered_all;
    else
        return *m_shared_const.is_piece_considered[nu_moves];
}

void State::init_moves_with_gamma(Color c)
{
    m_is_piece_considered[c] = &get_pieces_considered();
    m_local_value.init(m_bd);
    m_total_gamma = 0;
    auto& marker = m_marker[c];
    auto& moves = *m_moves[c];
    marker.clear_all_set_known(moves);
    moves.clear();
    Board::PiecesLeftList pieces_considered;
    for (Piece piece : m_bd.get_pieces_left(c))
        if ((*m_is_piece_considered[c])[piece])
            pieces_considered.push_back(piece);
    if (m_bd.is_first_piece(c))
        add_starting_moves(c, pieces_considered, true);
    else
        for (Point p : m_bd.get_attach_points(c))
            if (! m_bd.is_forbidden(p, c))
                add_moves(p, c, pieces_considered);
    m_is_move_list_initialized[c] = true;
    m_new_moves[c].clear();
    if (moves.empty() && ! m_force_consider_all_pieces)
    {
        m_force_consider_all_pieces = true;
        init_moves_with_gamma(c);
    }
}

void State::init_moves_without_gamma(Color c)
{
    m_is_piece_considered[c] = &get_pieces_considered();
    auto& marker = m_marker[c];
    auto& moves = *m_moves[c];
    marker.clear_all_set_known(moves);
    moves.clear();
    Board::PiecesLeftList pieces_considered;
    for (Piece piece : m_bd.get_pieces_left(c))
        if ((*m_is_piece_considered[c])[piece])
            pieces_considered.push_back(piece);
    auto& is_forbidden = m_bd.is_forbidden(c);
    if (m_bd.is_first_piece(c))
        add_starting_moves(c, pieces_considered, false);
    else
        for (Point p : m_bd.get_attach_points(c))
            if (! is_forbidden[p])
            {
                auto adj_status = m_bd.get_adj_status(p, c);
                for (Piece piece : pieces_considered)
                    for (Move mv : get_moves(c, piece, p, adj_status))
                        if (! marker[mv]
                            && check_move_without_gamma(is_forbidden, mv))
                        {
                            marker.set(mv);
                            moves.push_back(mv);
                        }
                m_moves_added_at[c].set(p);
            }
    m_is_move_list_initialized[c] = true;
    m_new_moves[c].clear();
    if (moves.empty() && ! m_force_consider_all_pieces)
    {
        m_force_consider_all_pieces = true;
        init_moves_without_gamma(c);
    }
}

void State::play_expanded_child(Move mv)
{
    if (log_simulations)
        log("Playing expanded child");
    if (! mv.is_pass())
        play_playout(mv);
    else
    {
        m_bd.play_pass();
        ++m_nu_passes;
        // Don't try to handle pass moves: a pass move either breaks symmetry
        // or both players have passed and it's the end of the game and we need
        // symmetry detection only as a heuristic ((playouts and move value
        // initialization)
        m_is_symmetry_broken = true;
        if (log_simulations)
            log() << m_bd;
    }
}

void State::play_playout(Move mv)
{
    LIBBOARDGAME_ASSERT(m_bd.is_legal(mv));
    m_new_moves[m_bd.get_to_play()].push_back(mv);
    m_bd.play_nonpass(mv);
    m_nu_passes = 0;
    if (! m_is_symmetry_broken)
        update_symmetry_broken(mv);
    if (log_simulations)
        log() << m_bd;
}

void State::start_search()
{
    auto& bd = *m_shared_const.board;
    m_bd.copy_from(bd);
    m_bd.set_to_play(m_shared_const.to_play);
    m_bd.take_snapshot();
    m_bc = &m_bd.get_board_const();
    m_nu_colors = bd.get_nu_colors();
    m_move_info_array = m_bc->get_move_info_array();
    m_move_info_ext_array = m_bc->get_move_info_ext_array();
    auto& geo = bd.get_geometry();
    m_nu_moves_initial = bd.get_nu_moves();
    m_check_terminate_early =
        (m_nu_moves_initial < 10u * m_nu_colors
         && m_bd.get_nu_players() == 2);
    Float total_piece_points = Float(m_bc->get_total_piece_points());
    m_score_modification_factor =
        m_shared_const.score_modification / total_piece_points;
    m_nu_simulations = 0;
    m_nu_playout_moves = 0;
    m_nu_last_good_reply_moves = 0;
    auto variant = bd.get_variant();
    m_check_symmetric_draw =
        ((variant == Variant::duo || variant == Variant::junior
          || variant == Variant::trigon_2)
         && m_shared_const.detect_symmetry
         && ! ((m_shared_const.to_play == Color(1)
                || m_shared_const.to_play == Color(3))
               && m_shared_const.avoid_symmetric_draw)
         && ! check_symmetry_broken());
    if (variant == Variant::trigon_2)
        m_symmetry_min_nu_pieces = 5;
    else
        m_symmetry_min_nu_pieces = 3; // Only used in Duo
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (! m_moves[*i])
            m_moves[*i].reset(new MoveList());

    // Init m_dist_to_center
    float width = static_cast<float>(geo.get_width());
    float height = static_cast<float>(geo.get_height());
    float center_x = 0.5f * width - 0.5f;
    float center_y = 0.5f * height - 0.5f;
    bool is_trigon = (bd.get_board_type() == BoardType::trigon
                      || bd.get_board_type() == BoardType::trigon_3);
    float ratio = (is_trigon ? 1.732f : 1);
    for (GeometryIterator i(geo); i; ++i)
    {
        float x = static_cast<float>(i->get_x());
        float y = static_cast<float>(i->get_y());
        float dx = x - center_x;
        float dy = ratio * (y - center_y);
        // Multiply Euklidian distance by 4, so that distances that differ
        // by max. 0.25 are treated as equal
        float d = round(4 * sqrt(dx * dx + dy * dy));
        if (bd.get_board_type() == BoardType::classic)
            // Don't make a distinction between moves close enough to the center
            // in game variant Classic/Classic2
            d = max(d, 10.f);
        m_dist_to_center[*i] = static_cast<unsigned>(d);
    }
    //log() << "Dist to center:\n" << m_dist_to_center;

    // Init precomputed gamma values
    double gamma = 1;
    for (unsigned i = 1; i < PieceInfo::max_size + 1; ++i, gamma *= 3)
        m_gamma_piece_size[i] = gamma;
    gamma = 1;
    for (unsigned i = 0; i < PieceInfo::max_size + 1; ++i, gamma *= 1e6)
        m_gamma_nu_attach[i] = gamma;
}

void State::start_simulation(size_t n)
{
    if (log_simulations)
        log() << "==========================================================\n"
              << "Simulation " << n << "\n"
              << "==========================================================\n";
    ++m_nu_simulations;
    m_bd.restore_snapshot();
    m_force_consider_all_pieces = false;
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_has_moves[*i] = true;
        m_is_move_list_initialized[*i] = false;
        m_new_moves[*i].clear();
        m_moves_added_at[*i].clear();
    }
    m_nu_passes = 0;
    // TODO: m_nu_passes should be initialized without assuming alternating
    // colors in the board's move history
    for (unsigned i = m_bd.get_nu_moves(); i > 0; --i)
    {
        if (! m_bd.get_move(i - 1).move.is_pass())
            break;
        ++m_nu_passes;
    }
}

void State::update_moves(Color c)
{
    m_local_value.init(m_bd);
    m_total_gamma = 0;
    auto& marker = m_marker[c];

    // Find old moves that are still legal
    PieceMap<bool> is_piece_left(false);
    for (Piece piece : m_bd.get_pieces_left(c))
        is_piece_left[piece] = true;
    auto& is_forbidden = m_bd.is_forbidden(c);
    auto& tmp_moves = *m_tmp_moves;
    tmp_moves.clear();
    double gamma;
    for (Move mv : *m_moves[c])
    {
        auto& info = get_move_info(mv);
        if (is_piece_left[info.get_piece()]
            && check_move(is_forbidden, info, gamma))
            add_move(tmp_moves, mv, gamma);
        else
            marker.clear(mv);
    }
    swap(m_tmp_moves, m_moves[c]);

    // Find new legal moves because of new pieces played by this color
    Board::PiecesLeftList pieces_considered;
    for (Piece piece : m_bd.get_pieces_left(c))
        if ((*m_is_piece_considered[c])[piece])
            pieces_considered.push_back(piece);
    for (Move mv : m_new_moves[c])
    {
        auto& info_ext = get_move_info_ext(mv);
        auto i = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
            if (! is_forbidden[*i] && ! m_moves_added_at[c][*i])
                add_moves(*i, c, pieces_considered);
        while (++i != end);
    }
    m_new_moves[c].clear();

    // Generate moves for pieces not considered in the last position
    auto& is_piece_considered = *m_is_piece_considered[c];
    if (&is_piece_considered != &m_shared_const.is_piece_considered_all)
    {
        if (m_moves[c]->empty())
            m_force_consider_all_pieces = true;
        auto& is_piece_considered_new = get_pieces_considered();
        if (&is_piece_considered !=  &is_piece_considered_new)
        {
            pieces_considered.clear();
            for (Piece piece : m_bd.get_pieces_left(c))
                if (! is_piece_considered[piece]
                    && is_piece_considered_new[piece])
                    pieces_considered.push_back(piece);
            for (Point p : m_bd.get_attach_points(c))
                if (! is_forbidden[p])
                {
                    auto adj_status = m_bd.get_adj_status(p, c);
                    for (Piece piece : pieces_considered)
                        add_moves(p, c, piece, adj_status);
                }
            m_is_piece_considered[c] = &is_piece_considered_new;
        }
    }
}

void State::update_symmetry_broken(Move mv)
{
    LIBBOARDGAME_ASSERT(! mv.is_pass());
    auto& info = get_move_info(mv);
    Color to_play = m_bd.get_to_play();
    Color second_color = m_bd.get_second_color(to_play);
    if (to_play == Color(0) || to_play == Color(2))
    {
        // First player to play: Check that all symmetric points of the last
        // move of the second player are occupied by the first player
        for (auto i = info.begin(); i != info.end(); ++i)
        {
            Point symm_p = m_shared_const.symmetric_points[*i];
            if (m_bd.get_point_state(symm_p) != second_color)
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
    }
    else
    {
        // Second player to play: Check that all symmetric points of the last
        // move of the first player are empty (i.e. the second player can play
        // there to preserve the symmetry)
        for (auto i = info.begin(); i != info.end(); ++i)
        {
            Point symm_p = m_shared_const.symmetric_points[*i];
            if (! m_bd.get_point_state(symm_p).is_empty())
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
    }
}

void State::write_info(ostream& out) const
{
    if (m_nu_playout_moves > 0)
    {
        FmtSaver saver(log());
        out << "LGR: " << fixed << setprecision(1)
            << (100.0 * static_cast<double>(m_nu_last_good_reply_moves)
                / static_cast<double>(m_nu_playout_moves))
            << "%";
    }
    out << '\n';
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
