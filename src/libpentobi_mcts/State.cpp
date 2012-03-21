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
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMove;
using libpentobi_base::DiagIterator;
using libpentobi_base::Geometry;
using libpentobi_base::GeometryIterator;
using libpentobi_base::MoveInfo;
using libpentobi_base::Piece;
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

/** Return the symmetric point state for symmetry detection.
    Only used for game_variant_duo. Returns the other color or empty, if the
    given point state is empty. */
PointState get_symmetric_state(PointState s)
{
    if (s.is_empty())
        return s;
    Color c = s.to_color();
    LIBBOARDGAME_ASSERT(c.to_int() < 2); // game_variant_duo
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
      avoid_symmetric_draw(false),
      score_modification(ValueType(0.1))
{
    symmetric_points.init(*RectGeometry<Point>::get(14, 14));
}

//-----------------------------------------------------------------------------

State::State(GameVariant initial_game_variant, const SharedConst& shared_const)
  : m_shared_const(shared_const),
    m_bd(initial_game_variant)
{
}

State::State(const State& state)
  : m_shared_const(state.m_shared_const),
    m_bd(state.m_bd.get_game_variant())
{
}

State::~State() throw()
{
}

inline void State::add_moves(Point p, Color c)
{
    unsigned int adj_status = m_bd.get_adj_status_index(p, c);
    BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
        if ((*m_is_piece_considered[c])[i])
            add_moves(p, c, i, adj_status);
}

inline void State::add_moves(Point p, Color c, unsigned int piece,
                             unsigned int adj_status)
{
    ArrayList<Move, Move::range>& moves = m_moves[c];
    const Board::LocalMovesListRange& move_candidates =
        m_bd.get_moves(piece, p, adj_status);
    for (auto i = move_candidates.first; i != move_candidates.second; ++i)
        if (! m_shared_const.is_forbidden_at_root[c][*i])
        {
            unsigned int local_value;
            const MoveInfo& info = get_move_info(*i);
            if (check_move(c, info.points, local_value) && ! m_marker[*i])
            {
                moves.push_back(*i);
                m_marker.set(*i);
                check_local(local_value, *i, info);
            }
        }
}

void State::check_local(unsigned int local_value, Move mv, const MoveInfo& info)
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
                       unsigned int& local_value)
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
    const ArrayList<Move, Move::range>& moves = m_moves[to_play];
    const Geometry& geometry = m_bd.get_geometry();
    Grid<ValueType> point_value(geometry, 1);
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
                        point_value[*j] = max(point_value[*j], ValueType(4));
            }
        }
    }
    Grid<ValueType> attach_point_value(geometry);
    Grid<ValueType> adj_point_value(geometry);
    for (BoardIterator i(m_bd); i; ++i)
    {
        PointState s = m_bd.get_point_state(*i);
        if (m_bd.is_forbidden(*i, to_play) && s != to_play)
            attach_point_value[*i] = -5;
        else
            attach_point_value[*i] = 1;
        if (! m_bd.is_forbidden(*i, to_play))
            // Creating new forbidden points is a bad thing
            adj_point_value[*i] = -0.2f;
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
    m_max_heuristic = -numeric_limits<ValueType>::max();
    m_min_dist_to_center = numeric_limits<unsigned int>::max();
    m_has_connect_move = false;
    unsigned int nu_onboard_pieces = m_bd.get_nu_onboard_pieces();
    bool compute_dist_to_center =
        ((board_type == board_type_classic && nu_onboard_pieces < 13)
         || (board_type == board_type_trigon && nu_onboard_pieces < 5)
         || (board_type == board_type_trigon_3 && nu_onboard_pieces < 5));
    for (unsigned int i = 0; i < moves.size(); ++i)
    {
        const MoveInfo& info = get_move_info(moves[i]);
        MoveFeatures& features = m_features[i];
        features.heuristic = 0;
        features.connect = false;
        features.dist_to_center = numeric_limits<unsigned int>::max();
        auto j = info.points.begin();
        auto end = info.points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            features.heuristic += point_value[*j];
            ++j;
        }
        while (j != end);
        j = info.attach_points.begin();
        end = info.attach_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            features.heuristic += attach_point_value[*j];
            ++j;
        }
        while (j != end);
        j = info.adj_points.begin();
        end = info.adj_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            features.heuristic += adj_point_value[*j];
            if (m_bd.get_point_state(*j) == second_color)
            {
                features.connect = true;
                m_has_connect_move = true;
            }
            ++j;
        }
        while (j != end);
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

array<ValueType, 4> State::evaluate_playout()
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
        array<ValueType, 4> result;
        result[0] = result[1] = 0.5;
        return result;
    }
    return evaluate_terminal();
}

array<ValueType, 4> State::evaluate_terminal()
{
    array<ValueType, 4> result_array;
    for (ColorIterator i(m_bd.get_nu_colors()); i; ++i)
    {
        double game_result;
        ValueType score = ValueType(m_bd.get_score(*i, game_result));
        ValueType score_modification = m_shared_const.score_modification;
        ValueType result;
        if (game_result == 1)
            result =
                1 - score_modification + score * m_score_modification_factor;
        else if (game_result == 0)
            result = score_modification + score * m_score_modification_factor;
        else
            result = 0.5;
        if (*i == m_shared_const.to_play)
            m_stat_score.add(score);
        result_array[(*i).to_int()] = result;
        if (log_simulations)
            log() << "Result color " << (*i).to_int() << ": score=" << score
                  << " result=" << result << '\n';
    }
    return result_array;
}

bool State::gen_and_play_playout_move(Move last_good_reply)
{
    unsigned int nu_colors = m_bd.get_nu_colors();
    if (m_nu_passes == nu_colors)
        return false;
    Color to_play = m_bd.get_to_play();
    GameVariant variant = m_bd.get_game_variant();
    m_has_moves[to_play] = ! m_moves[to_play].empty();

    if (! m_has_moves[to_play])
    {
        // Don't care about the exact score of a playout if we are still early
        // in the game and we know that the playout is a loss because the
        // player has no more moves and the score is already negative.
        if (m_nu_moves_initial < 10 * nu_colors
            && (variant == game_variant_duo
                || ((variant == game_variant_classic_2
                     || variant == game_variant_trigon_2)
                    && ! m_has_moves[m_bd.get_second_color(to_play)])))
        {
            double game_result;
            if (m_bd.get_score(to_play, game_result) < 0)
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
    if (! last_good_reply.is_null() && ! last_good_reply.is_pass()
        && m_bd.is_legal(last_good_reply)
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
        const ArrayList<Move, Move::range>* moves;
        unsigned int max_playable_piece_size;
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
        const unsigned int max_try = 2;
        unsigned int nu_try = 0;
        do
        {
            ++nu_try;
            int i =
                m_random.generate_small_int(static_cast<int>(moves->size()));
            mv = (*moves)[i];
        }
        while (get_move_info(mv).points.size() < max_playable_piece_size
               && nu_try < max_try);
    }
    play_nonpass(mv);
    return true;
}

void State::gen_children(Tree<Move>::NodeExpander& expander)
{
    unsigned int nu_colors = m_bd.get_nu_colors();
    if (m_nu_passes == nu_colors)
        return;
    Color to_play = m_bd.get_to_play();
    init_move_list_without_local(to_play);
    init_symmetry_info();
    m_extended_update = true;
    const ArrayList<Move, Move::range>& moves = m_moves[to_play];
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
        if (to_play == Color(1))
        {
            ColorMove last = m_bd.get_move(m_bd.get_nu_moves() - 1);
            symmetric_mv = m_bd.get_move_info_ext(last.move).symmetric_move;
        }
        else if (m_bd.get_nu_moves() > 0)
        {
            BOOST_FOREACH(Move mv, moves)
                if (m_bd.get_move_info_ext(mv).breaks_symmetry)
                {
                    has_symmetry_breaker = true;
                    break;
                }
        }
    }
    for (unsigned int i = 0; i < moves.size(); ++i)
    {
        Move mv = moves[i];
        const MoveFeatures& features = m_features[i];
        if (m_min_dist_to_center != numeric_limits<unsigned int>::max()
            && features.dist_to_center != m_min_dist_to_center)
            // Prune early moves that don't minimize dist to center
            continue;
        if (m_bd.get_board_type() == board_type_classic
            && m_bd.get_nu_onboard_pieces() < 14 && m_has_connect_move
            && ! features.connect)
            // Prune moves that don't connect in the middle if connection is
            // possible
            continue;
        // Make heuristic relative to best move and scale it to [0..1]
        ValueType heuristic = 0.3f * (m_max_heuristic - features.heuristic);

        // Quickly squash to [0..1] roughly as in exp(-x)  (and make sure it
        // stays greater than 0 and is monotonically decreasing)
        if (heuristic < 0.5)
            heuristic = 1.f - 0.9f * heuristic;
        else if (heuristic < 2)
            heuristic = 0.45f - 0.2f * (heuristic - 0.45f);
        else if (heuristic < 4)
            heuristic = 0.27f - 0.08f * (heuristic - 0.27f);
        else
            heuristic = 0.0248f / (heuristic - 3.0f);

        ValueType value = 1 * (0.1f + 0.9f * heuristic);
        ValueType count = 1;
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
        if (count > 0)
            value /= count;
        expander.add_child(mv, value, count, value, count);
    }
}

void State::init_move_list_with_local(Color c)
{
    m_last_move[c] = Move::null();
    m_is_piece_considered[c] =
        &m_shared_const.is_piece_considered[m_bd.get_nu_onboard_pieces()];
    m_local_value.init(m_bd);
    m_local_moves.clear();
    m_max_local_value = 1;
    m_max_playable_piece_size = 0;
    m_max_playable_piece_size_local = 0;
    ArrayList<Move, Move::range>& moves = m_moves[c];
    moves.clear();
    bool is_first_move =
        (m_bd.get_pieces_left(c).size() == m_bd.get_nu_pieces());
    if (is_first_move)
    {
        // Using only one starting point (if game variant has more than one) not
        // only reduces the branching factor but is also necessary because
        // update_move_list() assumes that a move stays legal if the forbidden
        // status for all of its points does not change.
        Point p = find_best_starting_point(m_bd, c);
        if (! p.is_null())
        {
            BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
                if ((*m_is_piece_considered[c])[i])
                {
                    unsigned int adj_status = m_bd.get_adj_status_index(p, c);
                    BOOST_FOREACH(Move mv, m_bd.get_moves(i, p, adj_status))
                    {
                        if (m_marker[mv])
                            continue;
                        if (! m_bd.is_forbidden(c, mv))
                        {
                            moves.push_back(mv);
                            m_marker.set(mv);
                        }
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
    m_marker.clear(moves);
    m_is_move_list_initialized[c] = true;
}

void State::init_move_list_without_local(Color c)
{
    m_last_move[c] = Move::null();
    m_is_piece_considered[c] =
        &m_shared_const.is_piece_considered[m_bd.get_nu_onboard_pieces()];
    ArrayList<Move, Move::range>& moves = m_moves[c];
    moves.clear();
    bool is_first_move =
        (m_bd.get_pieces_left(c).size() == m_bd.get_nu_pieces());
    if (is_first_move)
    {
        // Using only one starting point (if game variant has more than one) not
        // only reduces the branching factor but is also necessary because
        // update_move_list() assumes that a move stays legal if the forbidden
        // status for all of its points does not change.
        Point p = find_best_starting_point(m_bd, c);
        if (! p.is_null())
        {
            BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
                if ((*m_is_piece_considered[c])[i])
                {
                    unsigned int adj_status = m_bd.get_adj_status_index(p, c);
                    BOOST_FOREACH(Move mv, m_bd.get_moves(i, p, adj_status))
                    {
                        if (m_shared_const.is_forbidden_at_root[c][mv]
                            || m_marker[mv])
                            continue;
                        if (! m_bd.is_forbidden(c, mv))
                        {
                            moves.push_back(mv);
                            m_marker.set(mv);
                        }
                    }
                }
        }
    }
    else
    {
        BOOST_FOREACH(Point p, m_bd.get_attach_points(c))
            if (! m_bd.is_forbidden(p, c))
            {
                unsigned int adj_status = m_bd.get_adj_status_index(p, c);
                BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
                    if ((*m_is_piece_considered[c])[i])
                    {
                        BOOST_FOREACH(Move mv, m_bd.get_moves(i, p, adj_status))
                        {
                            if (m_marker[mv])
                                continue;
                            if (! m_bd.is_forbidden(c, mv))
                            {
                                moves.push_back(mv);
                                m_marker.set(mv);
                            }
                        }
                    }
            }
    }
    m_marker.clear(moves);
    m_is_move_list_initialized[c] = true;
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
        unsigned int nu_moves = m_bd.get_nu_moves();
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
    if (m_extended_update
        && m_bd.get_nu_moves() < m_bd.get_max_game_moves())
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
    ValueType total_piece_points =
        ValueType(bd.get_board_const().get_total_piece_points());
    m_score_modification_factor =
        m_shared_const.score_modification / total_piece_points;
    m_nu_simulations = 0;
    m_nu_playout_moves = 0;
    m_nu_last_good_reply_moves = 0;
    m_stat_score.clear();
    m_check_symmetric_draw =
        (bd.get_game_variant() == game_variant_duo
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
        m_dist_to_center[*i] = static_cast<unsigned int>(d);
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
    for (ColorIterator i(m_bd.get_nu_colors()); i; ++i)
    {
        m_has_moves[*i] = true;
        m_is_move_list_initialized[*i] = false;
    }
    m_nu_passes = 0;
    // TODO: m_nu_passes should be initialized without asuming alternating
    // colors in the board's move history
    for (unsigned int i = m_bd.get_nu_moves(); i > 0; --i)
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
    ArrayList<Move, Move::range>& moves = m_moves[c];

    // Find old moves that are still legal
    unsigned int last_piece = numeric_limits<unsigned int>::max();
    if (! last_mv.is_null() && ! last_mv.is_pass())
        last_piece = get_move_info(last_mv).piece;
    for (auto i = moves.begin(); i != moves.end(); ++i)
    {
        unsigned int local_value;
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
    if (! last_mv.is_null() && ! last_mv.is_pass())
    {
        BOOST_FOREACH(Point p, get_move_info(last_mv).attach_points)
            if (! m_bd.is_forbidden(p, c)
                && is_only_move_diag(m_bd, p, c, last_mv))
                add_moves(p, c);
    }

    // Generate moves for pieces that were not considered in the last position
    const array<bool,Board::max_pieces>& is_piece_considered =
        m_shared_const.is_piece_considered[m_bd.get_nu_onboard_pieces()];
    bool pieces_considered_changed = false;
    BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
    {
        LIBBOARDGAME_ASSERT(! ((*m_is_piece_considered[c])[i]
                               && ! is_piece_considered[i]));
        if (! (*m_is_piece_considered[c])[i] && is_piece_considered[i])
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
                unsigned int adj_status = m_bd.get_adj_status_index(p, c);
                BOOST_FOREACH(unsigned int i, m_bd.get_pieces_left(c))
                {
                    if (! (*m_is_piece_considered[c])[i]
                        && is_piece_considered[i])
                        add_moves(p, c, i, adj_status);
                }
            }
        m_is_piece_considered[c] = &is_piece_considered;
    }

    m_marker.clear(m_moves[c]);
    m_last_move[c] = Move::null();
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
