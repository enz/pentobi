//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "State.h"

#include <cmath>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "libboardgame_util/Log.h"
#include "libboardgame_util/MathUtil.h"
#include "libboardgame_base/RectGeometry.h"
#include "libpentobi_base/BoardUtil.h"
#include "libpentobi_base/DiagIterator.h"
#include "libpentobi_base/Geometry.h"

namespace libpentobi_mcts {

using namespace std;
using boost::format;
using libboardgame_base::RectGeometry;
using libboardgame_base::Transform;
using libboardgame_mcts::Tree;
using libboardgame_util::log;
using libpentobi_base::board_type_classic;
using libpentobi_base::board_type_duo;
using libpentobi_base::board_type_trigon;
using libpentobi_base::board_type_trigon_3;
using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::DiagIterator;
using libpentobi_base::Geometry;
using libpentobi_base::GeometryIterator;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

const bool log_simulations = false;

const bool use_prior_knowledge = true;

const bool pure_random_playout = false;

Point find_best_starting_point(const Board& bd, Color c)
{
    // We use the starting point that maximizes the distance to occupied
    // starting points, especially to the ones occupied by the player (their
    // distance is weighted with a factor of 2)
    Point best = Point::null();
    float max_distance = -1;
    bool is_trigon = (bd.get_board_type() == board_type_trigon
                      || bd.get_board_type() == board_type_trigon_3);
    float ratio = (is_trigon ? 1.732f : 1);
    BOOST_FOREACH(Point p, bd.get_starting_points(c))
    {
        if (bd.is_forbidden(p, c))
            continue;
        float px = static_cast<float>(p.get_x());
        float py = static_cast<float>(p.get_y());
        float d = 0;
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        {
            BOOST_FOREACH(Point pp, bd.get_starting_points(*i))
            {
                PointState s = bd.get_point_state(pp);
                if (! s.is_empty())
                {
                    float ppx = static_cast<float>(pp.get_x());
                    float ppy = static_cast<float>(pp.get_y());
                    float dx = ppx - px;
                    float dy = ratio * (ppy - py);
                    float weight = 1;
                    if (s == c || s == bd.get_second_color(c))
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

/** Get game result from the view point of a color.
    The result is 0,0.5,1 for loss/tie/win in 2-player variants. If there are
    n &gt; 2 players, this is generalized in the following way: The scores are
    sorted in ascending order. Each rank r_i (i in 0..n-1) is assigned a result
    value of r_i/(n-1). If a multiple players have the same score, the result
    value is the average of all ranks with this score. So being the single
    winner still gives the result 1 and having the lowest score gives the
    result 0. Being the single winner is better than sharing the best place,
    which is better than getting the second place, etc. */
Float get_result(const Board& bd, Color c)
{
    Variant variant = bd.get_variant();
    if (variant == variant_duo || variant == variant_junior)
    {
        unsigned points0 = bd.get_points_with_bonus(Color(0));
        unsigned points1 = bd.get_points_with_bonus(Color(1));
        if (c == Color(0))
        {
            if (points0 > points1)
                return 1;
            else if (points0 < points1)
                return 0;
            else
                return 0.5;
        }
        else
        {
            if (points1 > points0)
                return 1;
            else if (points1 < points0)
                return 0;
            else
                return 0.5;
        }
    }
    else if (variant == variant_classic
             || variant == variant_trigon
             || variant == variant_trigon_3)
    {
        unsigned nu_colors = bd.get_nu_colors();
        array<unsigned,Color::range> points_array;
        for (unsigned i = 0; i < nu_colors; ++i)
            points_array[i] = bd.get_points_with_bonus(Color(i));
        unsigned points = points_array[c.to_int()];
        sort(points_array.begin(), points_array.begin() + nu_colors);
        Float result = 0;
        unsigned n = 0;
        for (unsigned i = 0; i < nu_colors; ++i)
            if (points_array[i] == points)
            {
                result += Float(i) / Float(nu_colors - 1);
                ++n;
            }
        result /= Float(n);
        return result;
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == variant_classic_2
                            || variant == variant_trigon_2);
        unsigned points0 =
            bd.get_points_with_bonus(Color(0))
            + bd.get_points_with_bonus(Color(2));
        unsigned points1 =
            bd.get_points_with_bonus(Color(1))
            + bd.get_points_with_bonus(Color(3));
        if (c == Color(0) || c == Color(2))
        {
            if (points0 > points1)
                return 1;
            else if (points0 < points1)
                return 0;
            else
                return 0.5;
        }
        else
        {
            if (points1 > points0)
                return 1;
            else if (points1 < points0)
                return 0;
            else
                return 0.5;
        }
    }
}

/** Return the symmetric point state for symmetry detection.
    Only used for variant_duo. Returns the other color or empty, if the
    given point state is empty. */
PointState get_symmetric_state(PointState s)
{
    if (s.is_empty())
        return s;
    Color c = s.to_color();
    LIBBOARDGAME_ASSERT(c.to_int() < 2); // variant_duo
    return PointState(Color(1 - c.to_int()));
}

bool is_only_move_diag(const Board& bd, Point p, Color c, Move mv)
{
    for (DiagIterator i(bd, p); i; ++i)
        if (bd.get_point_state(*i) == c && bd.get_played_move(*i) != mv)
            return false;
    return true;
}

} // namespace

//-----------------------------------------------------------------------------

SharedConst::SharedConst(const Color& to_play)
    : board(0),
      to_play(to_play),
      detect_symmetry(true),
      avoid_symmetric_draw(true),
      score_modification(Float(0.1))
{
    symmetric_points.init(*RectGeometry<Point>::get(14, 14));
}

//-----------------------------------------------------------------------------

State::State(Variant initial_variant, const SharedConst& shared_const)
  : m_shared_const(shared_const),
    m_bd(initial_variant)
{
}

State::State(const State& state)
  : m_shared_const(state.m_shared_const),
    m_bd(state.m_bd.get_variant())
{
}

State::~State() throw()
{
}

inline void State::add_moves(Point p, Color c)
{
    unsigned adj_status = m_bd.get_adj_status_index(p, c);
    BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
        if ((*m_is_piece_considered[c])[piece])
            add_moves(p, c, piece, adj_status);
}

inline void State::add_moves(Point p, Color c, Piece piece,
                             unsigned adj_status)
{
    MoveList& moves = m_moves[c];
    const Board::LocalMovesListRange& move_candidates =
        m_bd.get_moves(piece, p, adj_status);
    for (auto i = move_candidates.first; i != move_candidates.second; ++i)
        if (! m_shared_const.is_forbidden_at_root[c][*i])
        {
            unsigned local_value;
            const MoveInfo& info = get_move_info(*i);
            if (check_move(c, info.points, local_value)
                && ! m_marker.test_and_set(*i))
            {
                moves.push_back(*i);
                check_local(local_value, *i, info);
            }
        }
}

void State::check_local(unsigned local_value, Move mv, const MoveInfo& info)
{
    if (local_value < m_max_local_value)
        return;
    if (local_value > m_max_local_value)
    {
        m_max_local_value = local_value;
        m_local_moves.clear();
    }
    m_local_moves.push_back(mv);
    m_max_playable_piece_size_local =
        max(m_max_playable_piece_size_local, info.points.size());
}

bool State::check_move(Color c, const MovePoints& points,
                       unsigned& local_value)
{
    LocalValue::Compute compute_local(m_local_value);
    const Grid<bool>& is_forbidden = m_bd.is_forbidden(c);
    auto end = points.end();
    auto i = points.begin();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        if (is_forbidden[*i])
            return false;
        compute_local.add_move_point(*i);
        ++i;
    }
    while (i != end);
    local_value = compute_local.finish();
    m_max_playable_piece_size = max(m_max_playable_piece_size, points.size());
    return true;
}

void State::compute_features()
{
    Color to_play = m_bd.get_to_play();
    Color second_color = m_bd.get_second_color(to_play);
    BoardType board_type = m_bd.get_board_type();
    const MoveList& moves = m_moves[to_play];
    const Geometry& geometry = m_bd.get_geometry();
    Grid<Float> point_value(geometry, 1);
    for (ColorIterator i(m_bd.get_nu_colors()); i; ++i)
    {
        if (*i == to_play || *i == second_color)
            continue;
        BOOST_FOREACH(Point p, m_bd.get_attach_points(*i))
        {
            if (! m_bd.is_forbidden(p, *i))
            {
                point_value[p] = 5;
                for (AdjIterator j(m_bd, p); j; ++j)
                    if (! m_bd.is_forbidden(*j, *i))
                        point_value[*j] = max(point_value[*j], Float(4));
            }
        }
    }
    Grid<Float> attach_point_value(geometry);
    Grid<Float> adj_point_value(geometry);
    for (BoardIterator i(m_bd); i; ++i)
    {
        PointState s = m_bd.get_point_state(*i);
        if (m_bd.is_forbidden(*i, to_play) && s != to_play)
            attach_point_value[*i] = -5;
        else
            attach_point_value[*i] = 1;
        if (! m_bd.is_forbidden(*i, to_play))
        {
            if (m_bd.is_attach_point(*i, to_play))
                // Making own attach point forbidden is especially bad
                adj_point_value[*i] = -2;
            else
                // Creating new forbidden points is a bad thing
                adj_point_value[*i] = -0.2f;
        }
        else if (s == second_color)
            // Connecting 2 player colors in 2-colors-per-player game variants
            // is good (in other variants second_color is the same as to_play
            // but there it doesn't matter what adj_point_value[*i] is because
            // moves adjacent to to_play are not legal anyway).
            adj_point_value[*i] = 2;
        else
            adj_point_value[*i] = 0;
    }
    m_features.resize(moves.size());
    m_max_heuristic = -numeric_limits<Float>::max();
    m_min_dist_to_center = numeric_limits<unsigned>::max();
    m_has_connect_move = false;
    unsigned nu_onboard_pieces = m_bd.get_nu_onboard_pieces();
    bool compute_dist_to_center =
        ((board_type == board_type_classic && nu_onboard_pieces < 13)
         || (board_type == board_type_trigon && nu_onboard_pieces < 5)
         || (board_type == board_type_trigon_3 && nu_onboard_pieces < 5));
    bool check_connect =
        (board_type == board_type_classic && m_bd.get_nu_onboard_pieces() < 14);
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        const MoveInfo& info = get_move_info(moves[i]);
        const MoveInfoExt& info_ext = m_bd.get_move_info_ext(moves[i]);
        MoveFeatures& features = m_features[i];
        features.heuristic = 0;
        features.connect = false;
        features.dist_to_center = numeric_limits<unsigned>::max();
        auto j = info.points.begin();
        auto end = info.points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            features.heuristic += point_value[*j];
            ++j;
        }
        while (j != end);
        j = info_ext.attach_points.begin();
        end = info_ext.attach_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            features.heuristic += attach_point_value[*j];
            ++j;
        }
        while (j != end);
        j = info_ext.adj_points.begin();
        end = info_ext.adj_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        if (! check_connect)
        {
            do
            {
                features.heuristic += adj_point_value[*j];
                ++j;
            }
            while (j != end);
        }
        else
        {
            do
            {
                features.heuristic += adj_point_value[*j];
                if (m_bd.get_point_state(*j) == second_color)
                    features.connect = true;
                ++j;
            }
            while (j != end);
            if (features.connect)
                m_has_connect_move = true;
        }
        if (compute_dist_to_center)
        {
            for (auto j = info.points.begin(); j != info.points.end(); ++j)
                features.dist_to_center =
                    min(features.dist_to_center, m_dist_to_center[*j]);
            m_min_dist_to_center =
                min(m_min_dist_to_center, features.dist_to_center);
        }
        m_max_heuristic = max(m_max_heuristic, features.heuristic);
    }
}

void State::dump(ostream& out) const
{
    out << "pentobi_mcts::State:\n";
    libpentobi_base::boardutil::dump(m_bd, out);
}

array<Float,4> State::evaluate_playout()
{
    if (m_check_symmetric_draw && ! m_is_symmetry_broken
        && m_bd.get_nu_onboard_pieces() >= 3)
    {
        // Always evaluate symmetric positions as a draw in the playouts.
        // This will encourage the first player to break the symmetry and
        // the second player to preserve it. (Exception: don't do this if
        // the move number is less than 3 because the earliest time to break
        // the symmetry is move 3 and otherwise all moves are evaluated as
        // draw in very short searches.)
        if (log_simulations)
            log() << "Result: 0.5 (symmetry)\n";
        m_stat_score.add(0);
        array<Float,4> result;
        result[0] = result[1] = 0.5;
        return result;
    }
    return evaluate_terminal();
}

array<Float,4> State::evaluate_terminal()
{
    array<Float,4> result_array;
    for (ColorIterator i(m_bd.get_nu_colors()); i; ++i)
    {
        Float score = Float(m_bd.get_score(*i));
        Float game_result = get_result(m_bd, *i);
        Float score_modification = m_shared_const.score_modification;
        // Apply score modification. Example: If score modification is 0.1,
        // the game result is rescaled to [0..0.9] and the score modification
        // is added with 0.05 as the middle (corresponding to score 0).
        Float result =
            (1.f - score_modification) * game_result
            + 0.5f * (score_modification + score * m_score_modification_factor);
        if (*i == m_shared_const.to_play)
            m_stat_score.add(score);
        result_array[(*i).to_int()] = result;
        if (log_simulations)
            log() << "Result color " << (*i).to_int() << ": score=" << score
                  << " game_result=" << game_result << " result=" << result
                  << '\n';
    }
    return result_array;
}

bool State::gen_and_play_playout_move(Move last_good_reply)
{
    unsigned nu_colors = m_bd.get_nu_colors();
    if (m_nu_passes == nu_colors)
        return false;
    Color to_play = m_bd.get_to_play();
    Variant variant = m_bd.get_variant();
    m_has_moves[to_play] = ! m_moves[to_play].empty();

    if (! m_has_moves[to_play])
    {
        // Don't care about the exact score of a playout if we are still early
        // in the game and we know that the playout is a loss because the
        // player has no more moves and the score is already negative.
        if (m_nu_moves_initial < 10 * nu_colors
            && (variant == variant_duo || variant == variant_junior
                || ((variant == variant_classic_2
                     || variant == variant_trigon_2)
                    && ! m_has_moves[m_bd.get_second_color(to_play)])))
        {
            if (m_bd.get_score(to_play) < 0)
            {
                if (log_simulations)
                    log() << "Terminate early (no moves and negative score)\n";
                return false;
            }
        }
        play_pass();
        return true;
    }

    if (m_check_symmetric_draw)
        if (! m_is_symmetry_broken && m_bd.get_nu_onboard_pieces() >= 3)
        {
            // See also the comment in evaluate_playout()
            if (log_simulations)
                log() << "Terminate playout. Symmetry not broken.\n";
            return false;
        }
    ++m_nu_playout_moves;
    Move mv;
    if (last_good_reply.is_regular() && m_bd.is_legal(last_good_reply)
        && m_bd.get_pieces_left(to_play).contains(
                                     get_move_info(last_good_reply).piece))
    {
        ++m_nu_last_good_reply_moves;
        mv = last_good_reply;
        if (log_simulations)
            log() << "Playing last good reply\n";
    }
    else
    {
        const MoveList* moves;
        unsigned max_playable_piece_size;
        if (pure_random_playout || m_local_moves.empty())
        {
            moves = &m_moves[to_play];
            max_playable_piece_size = m_max_playable_piece_size;
            if (log_simulations)
                log() << "Moves: " << moves->size() << "\n";
        }
        else
        {
            moves = &m_local_moves;
            max_playable_piece_size = m_max_playable_piece_size_local;
            if (log_simulations)
                log() << "Moves: " << m_moves[to_play].size() << ", local: "
                      << m_local_moves.size() << ", max_local_value: "
                      << m_max_local_value << '\n';
        }
        // Choose a random move from the list of local moves (if not empty) or
        // the list of all moves. Try more than once if a bad move (e.g. not
        // maximum playable piece size) is chosen to reduce the probabilty
        // for such moves without becoming deterministic.
        const unsigned max_try = 2;
        unsigned nu_try = 0;
        do
        {
            ++nu_try;
            unsigned i = m_random.generate_small_uint(moves->size());
            mv = (*moves)[i];
        }
        while (get_move_info(mv).points.size() < max_playable_piece_size
               && nu_try < max_try);
    }
    play_nonpass(mv);
    return true;
}

void State::gen_children(Tree<Move>::NodeExpander& expander, Float init_val)
{
    unsigned nu_colors = m_bd.get_nu_colors();
    if (m_nu_passes == nu_colors)
        return;
    Color to_play = m_bd.get_to_play();
    init_move_list_without_local(to_play);
    init_symmetry_info();
    m_extended_update = true;
    const MoveList& moves = m_moves[to_play];
    if (moves.empty())
    {
        expander.add_child(Move::pass());
        return;
    }
    if (! use_prior_knowledge)
    {
        BOOST_FOREACH(Move mv, moves)
            expander.add_child(mv);
        return;
    }
    compute_features();
    Move symmetric_mv = Move::null();
    bool has_symmetry_breaker = false;
    if (m_check_symmetric_draw && ! m_is_symmetry_broken)
    {
        unsigned nu_moves = m_bd.get_nu_moves();
        if (to_play == Color(1))
        {
            if (nu_moves > 0)
            {
                ColorMove last = m_bd.get_move(nu_moves - 1);
                if (! last.is_pass())
                    symmetric_mv =
                        m_bd.get_move_info_ext(last.move).symmetric_move;
            }
        }
        else if (nu_moves > 0)
        {
            BOOST_FOREACH(Move mv, moves)
                if (m_bd.get_move_info_ext(mv).breaks_symmetry)
                {
                    has_symmetry_breaker = true;
                    break;
                }
        }
    }
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        Move mv = moves[i];
        const MoveFeatures& features = m_features[i];
        if (m_min_dist_to_center != numeric_limits<unsigned>::max()
            && features.dist_to_center != m_min_dist_to_center)
            // Prune early moves that don't minimize dist to center
            continue;
        if (m_bd.get_board_type() == board_type_classic
            && m_bd.get_nu_onboard_pieces() < 14 && m_has_connect_move
            && ! features.connect)
            // Prune moves that don't connect in the middle if connection is
            // possible
            continue;

        // Convert the heuristic, which is so far estimated in score points,
        // into a value in [0..1] by making it relative to the heuristic
        // of the best move and let it decrease exponentially with a certain
        // width.
        Float heuristic = 0.3f * (m_max_heuristic - features.heuristic);
        // Piecewise linear approximation of exp(-x) (make sure the
        // approximation is always greater than 0 and is always monotonically
        // decreasing)
        if (heuristic < 0.5)
            heuristic = 1.f - 0.9f * heuristic;
        else if (heuristic < 2)
            heuristic = 0.45f - 0.2f * (heuristic - 0.45f);
        else if (heuristic < 4)
            heuristic = 0.27f - 0.08f * (heuristic - 0.27f);
        else
            heuristic = 0.0248f / (heuristic - 3.0f);

        // Rescale to [0.1..1]. If the value is too close to 0, the move might
        // never get explored (in practice) if the bias term constant is small.
        Float value = 1 * (0.1f + 0.9f * heuristic);
        Float count = 1;

        value += init_val;
        count += 1;

        // Encourage to explore a move that keeps or breaks symmetry
        // See also the comment in evaluate_playout()
        if (m_check_symmetric_draw && ! m_is_symmetry_broken)
        {
            if (to_play == Color(1))
            {
                if (mv == symmetric_mv)
                    value += 5 * 1.0f;
                else
                    value += 5 * 0.1f;
                count += 5;
            }
            else if (has_symmetry_breaker)
            {
                if (m_bd.get_move_info_ext(mv).breaks_symmetry)
                    value += 5 * 1.0f;
                else
                    value += 5 * 0.1f;
                count += 5;
            }
        }
        value /= count;
        expander.add_child(mv, value, count, value, count);
    }
}

inline const PieceMap<bool>& State::get_pieces_considered() const
{
    if (m_consider_all_pieces)
        return m_shared_const.is_piece_considered_all;
    else
        return m_shared_const.is_piece_considered[m_bd.get_nu_onboard_pieces()];
}

void State::init_move_list_with_local(Color c)
{
    m_last_move[c] = Move::null();
    m_is_piece_considered[c] = &get_pieces_considered();
    m_local_value.init(m_bd);
    m_local_moves.clear();
    m_max_local_value = 1;
    m_max_playable_piece_size = 0;
    m_max_playable_piece_size_local = 0;
    MoveList& moves = m_moves[c];
    moves.clear();
    if (m_bd.is_first_piece(c))
    {
        // Using only one starting point (if game variant has more than one) not
        // only reduces the branching factor but is also necessary because
        // update_move_list() assumes that a move stays legal if the forbidden
        // status for all of its points does not change.
        Point p = find_best_starting_point(m_bd, c);
        if (! p.is_null())
        {
            BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
                if ((*m_is_piece_considered[c])[piece])
                {
                    unsigned adj_status = m_bd.get_adj_status_index(p, c);
                    BOOST_FOREACH(Move mv, m_bd.get_moves(piece, p, adj_status))
                    {
                        if (! m_bd.is_forbidden(c, mv)
                            && ! m_marker.test_and_set(mv))
                            moves.push_back(mv);
                    }
                }
        }
    }
    else
    {
        BOOST_FOREACH(Point p, m_bd.get_attach_points(c))
            if (! m_bd.is_forbidden(p, c))
                add_moves(p, c);
    }
    m_marker.clear_all_set_known(moves);
    m_is_move_list_initialized[c] = true;
    if (moves.empty() && ! m_consider_all_pieces)
    {
        m_consider_all_pieces = true;
        init_move_list_with_local(c);
    }
}

void State::init_move_list_without_local(Color c)
{
    m_last_move[c] = Move::null();
    m_is_piece_considered[c] = &get_pieces_considered();
    MoveList& moves = m_moves[c];
    moves.clear();
    if (m_bd.is_first_piece(c))
    {
        // Using only one starting point (if game variant has more than one) not
        // only reduces the branching factor but is also necessary because
        // update_move_list() assumes that a move stays legal if the forbidden
        // status for all of its points does not change.
        Point p = find_best_starting_point(m_bd, c);
        if (! p.is_null())
        {
            BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
                if ((*m_is_piece_considered[c])[piece])
                {
                    unsigned adj_status = m_bd.get_adj_status_index(p, c);
                    BOOST_FOREACH(Move mv, m_bd.get_moves(piece, p, adj_status))
                    {
                        if (! m_shared_const.is_forbidden_at_root[c][mv]
                            && ! m_bd.is_forbidden(c, mv)
                            && ! m_marker.test_and_set(mv))
                            moves.push_back(mv);
                    }
                }
        }
    }
    else
    {
        BOOST_FOREACH(Point p, m_bd.get_attach_points(c))
            if (! m_bd.is_forbidden(p, c))
            {
                unsigned adj_status = m_bd.get_adj_status_index(p, c);
                BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
                    if ((*m_is_piece_considered[c])[piece])
                    {
                        BOOST_FOREACH(Move mv,
                                      m_bd.get_moves(piece, p, adj_status))
                        {
                            if (! m_bd.is_forbidden(c, mv)
                                && ! m_marker.test_and_set(mv))
                                moves.push_back(mv);
                        }
                    }
            }
    }
    m_marker.clear_all_set_known(moves);
    m_is_move_list_initialized[c] = true;
    if (moves.empty() && ! m_consider_all_pieces)
    {
        m_consider_all_pieces = true;
        init_move_list_without_local(c);
    }
}

void State::init_symmetry_info()
{
    if (! m_check_symmetric_draw)
        return;
    m_is_symmetry_broken = false;
    if (m_bd.get_to_play() == Color(0))
    {
        // First player to play: We set m_is_symmetry_broken to true if the
        // position is symmetric.
        for (BoardIterator i(m_bd); i; ++i)
        {
            Point symm_p = m_shared_const.symmetric_points[*i];
            PointState s1 = m_bd.get_point_state(*i);
            PointState s2 = m_bd.get_point_state(symm_p);
            if (s1 != get_symmetric_state(s2))
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
    }
    else
    {
        // Second player to play: We set m_is_symmetry_broken to true if the
        // second player cannot copy the first player's last move to make the
        // position symmetric again.
        unsigned nu_moves = m_bd.get_nu_moves();
        if (nu_moves == 0)
        {
            // Don't try to handle white to play as first move
            m_is_symmetry_broken = true;
            return;
        }
        ColorMove last_mv = m_bd.get_move(nu_moves - 1);
        if (last_mv.color != Color(0))
        {
            // Don't try to handle non-alternating moves in board history
            m_is_symmetry_broken = true;
            return;
        }
        const MovePoints* points;
        if (last_mv.move.is_pass())
            points = 0;
        else
            points = &m_bd.get_move_info(last_mv.move).points;
        for (BoardIterator i(m_bd); i; ++i)
        {
            Point symm_p = m_shared_const.symmetric_points[*i];
            PointState s1 = m_bd.get_point_state(*i);
            PointState s2 = m_bd.get_point_state(symm_p);
            if (s1 != get_symmetric_state(s2))
            {
                if (points != 0)
                {
                    if ((points->contains(*i)
                         && s1 == Color(0) && s2.is_empty())
                        || (points->contains(symm_p)
                            && s1.is_empty() && s2 == Color(0)))
                        continue;
                }
                m_is_symmetry_broken = true;
                return;
            }
        }
    }
}

void State::play_pass()
{
    Color to_play = m_bd.get_to_play();
    m_last_move[to_play] = Move::pass();
    m_bd.play_pass(to_play);
    ++m_nu_passes;
    if (m_extended_update)
    {
        to_play = m_bd.get_to_play();
        if (m_is_move_list_initialized[to_play])
            update_move_list(to_play);
        else
            init_move_list_with_local(to_play);
        if (m_check_symmetric_draw && ! m_is_symmetry_broken)
        {
            // Don't try to handle pass moves: a pass move either breaks
            // symmetry or both players have passed and it's the end of the
            // game and we need symmetry detection only as a heuristic
            // (playouts and move value initialization)
            m_is_symmetry_broken = true;
        }
    }
    if (log_simulations)
        log() << m_bd;
}

void State::play_nonpass(Move mv)
{
    Color to_play = m_bd.get_to_play();
    m_last_move[to_play] = mv;
    LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
    m_bd.play(to_play, mv);
    m_nu_passes = 0;
    if (m_extended_update)
    {
        Color to_play = m_bd.get_to_play();
        if (m_is_move_list_initialized[to_play])
            update_move_list(to_play);
        else
            init_move_list_with_local(to_play);
        if (m_check_symmetric_draw && ! m_is_symmetry_broken)
            update_symmetry_info(mv);
    }
    if (log_simulations)
        log() << m_bd;
}

void State::play(Move mv)
{
    if (! mv.is_pass())
        play_nonpass(mv);
    else
        play_pass();
}

void State::start_playout()
{
    if (log_simulations)
        log() << "Start playout\n";
    if (! m_extended_update)
    {
        init_move_list_with_local(m_bd.get_to_play());
        init_symmetry_info();
        m_extended_update = true;
    }
}

void State::start_search()
{
    const Board& bd = *m_shared_const.board;
    m_move_info_array = bd.get_board_const().get_move_info_array();
    const Geometry& geometry = bd.get_geometry();
    m_local_value.init_geometry(geometry);
    m_nu_moves_initial = bd.get_nu_moves();
    Float total_piece_points =
        Float(bd.get_board_const().get_total_piece_points());
    m_score_modification_factor =
        m_shared_const.score_modification / total_piece_points;
    m_nu_simulations = 0;
    m_nu_playout_moves = 0;
    m_nu_last_good_reply_moves = 0;
    m_stat_score.clear();
    Variant variant = bd.get_variant();
    m_check_symmetric_draw =
        ((variant == variant_duo || variant == variant_junior)
         && m_shared_const.detect_symmetry
         && ! (m_shared_const.to_play == Color(1)
               && m_shared_const.avoid_symmetric_draw));

    // Init m_dist_to_center
    m_dist_to_center.init(geometry);
    float width = static_cast<float>(geometry.get_width());
    float height = static_cast<float>(geometry.get_height());
    float center_x = 0.5f * width - 0.5f;
    float center_y = 0.5f * height - 0.5f;
    bool is_trigon = (bd.get_board_type() == board_type_trigon
                      || bd.get_board_type() == board_type_trigon_3);
    float ratio = (is_trigon ? 1.732f : 1);
    for (GeometryIterator i(geometry); i; ++i)
    {
        float x = static_cast<float>(i->get_x());
        float y = static_cast<float>(i->get_y());
        float dx = x - center_x;
        float dy = ratio * (y - center_y);
        // Multiply Euklidian distance by 4, so that distances that differ
        // by max. 0.25 are treated as equal
        float d =
            libboardgame_util::math_util::round(4 * sqrt(dx * dx + dy * dy));
        if (bd.get_board_type() == board_type_classic)
            // Don't make a distinction between moves close enough to the center
            // in game variant Classic/Classic2
            d = max(d, 10.f);
        m_dist_to_center[*i] = static_cast<unsigned>(d);
    }
    //log() << "Dist to center:\n" << m_dist_to_center;
}

void State::start_simulation(size_t n)
{
    if (log_simulations)
        log() << "==========================================================\n"
              << "Simulation " << n << "\n"
              << "==========================================================\n";
    ++m_nu_simulations;
    m_bd.copy_from(*m_shared_const.board);
    m_bd.set_to_play(m_shared_const.to_play);
    m_extended_update = false;
    m_consider_all_pieces = false;
    for (ColorIterator i(m_bd.get_nu_colors()); i; ++i)
    {
        m_has_moves[*i] = true;
        m_is_move_list_initialized[*i] = false;
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

void State::update_move_list(Color c)
{
    m_local_value.init(m_bd);
    m_local_moves.clear();
    m_max_local_value = 1;
    m_max_playable_piece_size = 0;
    m_max_playable_piece_size_local = 0;
    Move last_mv = m_last_move[c];
    MoveList& moves = m_moves[c];

    // Find old moves that are still legal
    Piece last_piece = Piece::null();
    if (last_mv.is_regular())
    {
        Piece piece = get_move_info(last_mv).piece;
        if (m_bd.get_nu_left_piece(c, piece) == 0)
            last_piece = piece;
    }
    for (auto i = moves.begin(); i != moves.end(); ++i)
    {
        unsigned local_value;
        const MoveInfo& info = get_move_info(*i);
        if (info.piece != last_piece && check_move(c, info.points, local_value))
        {
            m_marker.set(*i);
            check_local(local_value, *i, info);
        }
        else
        {
            moves.remove_fast(i);
            --i;
        }
    }

    // Find new legal moves because of the last piece played by this color
    if (last_mv.is_regular())
    {
        BOOST_FOREACH(Point p, m_bd.get_move_info_ext(last_mv).attach_points)
            if (! m_bd.is_forbidden(p, c)
                && is_only_move_diag(m_bd, p, c, last_mv))
                add_moves(p, c);
    }

    // Generate moves for pieces that were not considered in the last position
    const PieceMap<bool>& is_piece_considered = get_pieces_considered();
    bool pieces_considered_changed = false;
    BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
    {
        LIBBOARDGAME_ASSERT(! ((*m_is_piece_considered[c])[piece]
                               && ! is_piece_considered[piece]));
        if (! (*m_is_piece_considered[c])[piece] && is_piece_considered[piece])
        {
            pieces_considered_changed = true;
            break;
        }
    }
    if (pieces_considered_changed)
    {
        BOOST_FOREACH(Point p, m_bd.get_attach_points(c))
            if (! m_bd.is_forbidden(p, c))
            {
                unsigned adj_status = m_bd.get_adj_status_index(p, c);
                BOOST_FOREACH(Piece piece, m_bd.get_pieces_left(c))
                {
                    if (! (*m_is_piece_considered[c])[piece]
                        && is_piece_considered[piece])
                        add_moves(p, c, piece, adj_status);
                }
            }
        m_is_piece_considered[c] = &is_piece_considered;
    }

    m_marker.clear_all_set_known(m_moves[c]);
    m_last_move[c] = Move::null();
    if (m_moves[c].empty() && ! m_consider_all_pieces)
    {
        m_consider_all_pieces = true;
        update_move_list(c);
    }
}

void State::update_symmetry_info(Move mv)
{
    LIBBOARDGAME_ASSERT(! mv.is_pass());
    const MovePoints& points = get_move_info(mv).points;
    if (m_bd.get_to_play() == Color(0))
    {
        // First player to play: Check that all symmetric points of the last
        // move of the second player are occupied by the first player
        for (auto i = points.begin(); i != points.end(); ++i)
        {
            Point symm_p = m_shared_const.symmetric_points[*i];
            if (m_bd.get_point_state(symm_p) != Color(0))
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
    }
    else
    {
        // Second player to play: Check that all symmetric points of the last
        // move of the first player are empty (i.e. the second can play there
        // to preserve the symmetry)
        for (auto i = points.begin(); i != points.end(); ++i)
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
    out << "Sco: ";
    m_stat_score.write(out, true, 1);
    if (m_nu_playout_moves > 0)
        out << (format(", LGR: %.1f%%")
                % (100.0 * static_cast<double>(m_nu_last_good_reply_moves)
                   / static_cast<double>(m_nu_playout_moves)));
    out << '\n';
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
