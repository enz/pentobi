//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "State.h"

#include "libboardgame_util/MathUtil.h"
#include "libpentobi_base/ScoreUtil.h"
#ifdef LIBBOARDGAME_DEBUG
#include "libpentobi_base/BoardUtil.h"
#endif

namespace libpentobi_mcts {

using libboardgame_util::fast_exp;
using libpentobi_base::get_multiplayer_result;
using libpentobi_base::BoardType;
using libpentobi_base::PointState;
using libpentobi_base::ScoreType;

//-----------------------------------------------------------------------------

namespace {

inline Float sigmoid(Float steepness, Float x)
{
    return -1.f + 2.f / (1.f + fast_exp(-steepness * x));
}

} // namespace

//-----------------------------------------------------------------------------

State::State(Variant initial_variant, const SharedConst& shared_const)
  : m_shared_const(shared_const),
    m_bd(initial_variant),
    m_prior_knowledge(m_bd)
{
}

template<unsigned MAX_SIZE>
inline void State::add_moves(Point p, Color c,
                             const Board::PiecesLeftList& pieces,
                             float& total_gamma, MoveList& moves,
                             unsigned& nu_moves)
{
    auto& marker = m_marker[c];
    auto& playout_features = m_playout_features[c];
    auto adj_status = m_bd.get_adj_status(p, c);
    for (Piece piece : pieces)
    {
        if (! has_moves(c, piece, p, adj_status))
            continue;
        auto gamma_piece = m_gamma_piece[piece];
        for (Move mv : get_moves(c, piece, p, adj_status))
            if (! marker[mv]
                    && check_move<MAX_SIZE>(
                           mv, get_move_info<MAX_SIZE>(mv), gamma_piece, moves,
                           nu_moves, playout_features, total_gamma))
                marker.set(mv);
    }
}

void State::add_callisto_one_piece_moves(Color c, bool with_gamma,
                                         float& total_gamma, MoveList& moves,
                                         unsigned& nu_moves)
{
    Piece one_piece = m_bd.get_one_piece();
    auto nu_left = m_bd.get_nu_left_piece(c, one_piece);
    if (nu_left == 0)
        return;
    for (unsigned i = 0; i < m_shared_const.one_piece_points_callisto.size();
         ++i)
    {
        Point p = m_shared_const.one_piece_points_callisto[i];
        if (m_bd.is_forbidden(p, c))
            continue;
        Move mv = m_shared_const.one_piece_moves_callisto[i];
        LIBBOARDGAME_ASSERT(nu_moves < MoveList::max_size);
        moves.get_unchecked(nu_moves) = mv;
        ++nu_moves;
        LIBBOARDGAME_ASSERT(! m_marker[c][mv]);
        m_marker[c].set(mv);
        if (with_gamma)
        {
            total_gamma += m_gamma_piece[one_piece];
            m_cumulative_gamma[nu_moves - 1] = total_gamma;
        }
    }
}

template<unsigned MAX_SIZE>
void State::add_starting_moves(Color c, const Board::PiecesLeftList& pieces,
                               bool with_gamma, MoveList& moves)
{
    // Using only one starting point (if game variant has more than one) not
    // only reduces the branching factor but is also necessary because
    // update_moves() assumes that a move stays legal if the forbidden
    // status for all of its points does not change.
    Point p = find_best_starting_point(c);
    if (p.is_null())
        return;
    unsigned nu_moves = 0;
    auto& marker = m_marker[c];
    auto& is_forbidden = m_bd.is_forbidden(c);
    float total_gamma = 0;
    bool is_gembloq = (m_bd.get_piece_set() == PieceSet::gembloq);
    for (Piece piece : pieces)
        for (Move mv : get_moves(c, piece, p, 0))
        {
            // In GembloQ, not all moves covering one starting point
            // (=quarter-square tringle) are legal.
            if (is_gembloq && ! m_bd.is_legal(c, mv))
                continue;
            if (check_forbidden<MAX_SIZE>(is_forbidden, mv, moves, nu_moves))
            {
                LIBBOARDGAME_ASSERT(! marker[mv]);
                marker.set(mv);
                if (with_gamma)
                {
                    total_gamma += m_gamma_piece[piece];
                    m_cumulative_gamma[nu_moves - 1] = total_gamma;
                }
            }
        }
    moves.resize(nu_moves);
}

template<unsigned MAX_SIZE>
bool State::check_forbidden(const GridExt<bool>& is_forbidden, Move mv,
                            MoveList& moves, unsigned& nu_moves)
{
    auto p = get_move_info<MAX_SIZE>(mv).begin();
    unsigned forbidden = is_forbidden[*p];
    for (unsigned i = 1; i < MAX_SIZE; ++i)
        // Logically, forbidden is a bool and the next line should be
        //   forbidden = forbidden || is_forbidden[*(++p)]
        // But this generates branches, which are bad for performance in this
        // tight loop (unrolled by the compiler). So we use a bitwise OR, which
        // works because C++ guarantees that true/false converts to 1/0.
        forbidden |= static_cast<unsigned>(is_forbidden[*(++p)]);
    if (forbidden != 0)
        return false;
    LIBBOARDGAME_ASSERT(nu_moves < MoveList::max_size);
    moves.get_unchecked(nu_moves) = mv;
    ++nu_moves;
    return true;
}

template<unsigned MAX_SIZE>
bool State::check_move(Move mv, const MoveInfo<MAX_SIZE>& info,
                       float gamma_piece, MoveList& moves, unsigned& nu_moves,
                       const PlayoutFeatures& playout_features,
                       float& total_gamma)
{
    auto p = info.begin();
    PlayoutFeatures::Compute features(*p, playout_features);
    for (unsigned i = 1; i < MAX_SIZE; ++i)
        features.add(*(++p), playout_features);
    if (features.is_forbidden())
        return false;
    auto gamma = gamma_piece;
    gamma *= m_gamma_local[features.get_nu_local()];
    total_gamma += gamma;
    m_cumulative_gamma[nu_moves] = total_gamma;
    LIBBOARDGAME_ASSERT(nu_moves < MoveList::max_size);
    moves.get_unchecked(nu_moves) = mv;
    ++nu_moves;
    return true;
}

template<unsigned MAX_SIZE>
inline bool State::check_move(Move mv, const MoveInfo<MAX_SIZE>& info,
                              MoveList& moves, unsigned& nu_moves,
                              const PlayoutFeatures& playout_features,
                              float& total_gamma)
{
    return check_move<MAX_SIZE>(
                mv, info, m_gamma_piece[info.get_piece()], moves, nu_moves,
                playout_features, total_gamma);
}

#ifdef LIBBOARDGAME_DEBUG
string State::dump() const
{
    ostringstream s;
    s << "pentobi_mcts::State:\n" << libpentobi_base::dump(m_bd);
    return s.str();
}
#endif

/** Evaluation function for game variants with 2 players and 2 colors per
    player. */
void State::evaluate_multicolor(array<Float, 6>& result)
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_players() == 2);
    LIBBOARDGAME_ASSERT(m_bd.get_nu_colors() == 4);
    // Always evaluate symmetric positions in trigon_2 as a draw in the
    // playouts. See comment in evaluate_playout_duo.
    // m_is_symmetry_broken is always true in classic_2, no need to check for
    // game variant.
    if (! m_is_symmetry_broken
            && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
    {
        if (log_simulations)
            LIBBOARDGAME_LOG("Result: 0.5 (symmetry)");
        result[0] = result[1] = result[2] = result[3] = 0.5;
        return;
    }

    auto s = m_bd.get_score_multicolor(Color(0));
    Float res;
    if (s > 0)
        res = 1;
    else if (s < 0)
        res = 0;
    else
        res = 0.5;
    if (log_simulations)
        LIBBOARDGAME_LOG("Result color 0: sco=", s, " game_res=", res);
    res += get_quality_bonus(Color(0), res, s)
            + get_quality_bonus_attach_multicolor();
    if (log_simulations)
        LIBBOARDGAME_LOG("res=", res);
    result[0] = result[2] = res;
    result[1] = result[3] = 1.f - res;
}

/** Evaluation function for game variants with more than 2 players.
    The result is 0,0.5,1 for loss/tie/win in 2-player variants. For n \> 2
    players, this is generalized in the following way: The scores are sorted in
    ascending order. Each rank r_i (i in 0..n-1) is assigned a result value of
    r_i/(n-1). If multiple players have the same score, the result value is the
    average of all ranks with this score. So being the single winner still
    gives the result 1 and having the lowest score gives the result 0. Being
    the single winner is better than sharing the best place, which is better
    than getting the second place, etc. */
void State::evaluate_multiplayer(array<Float, 6>& result)
{
    auto nu_players = m_bd.get_nu_players();
    LIBBOARDGAME_ASSERT(nu_players > 2);
    array<ScoreType, Color::range> points;
    for (Color::IntType i = 0; i < nu_players; ++i)
        points[i] = m_bd.get_points(Color(i));
    array<Float, Color::range> game_result;
    get_multiplayer_result(nu_players, points, game_result, m_is_callisto);
    for (Color::IntType i = 0; i < nu_players; ++i)
    {
        Color c(i);
        auto s = m_bd.get_score_multiplayer(c);
        result[i] = game_result[i] + get_quality_bonus(c, game_result[i], s);
        if (log_simulations)
            LIBBOARDGAME_LOG("Result sco=", s, " game_res=", game_result[i],
                             " res=", result[i]);
    }
    if (m_bd.get_variant() == Variant::classic_3)
    {
        result[3] = result[0];
        result[4] = result[1];
        result[5] = result[2];
    }
}

/** Evaluation function for game variants with 2 colors. */
void State::evaluate_twocolor(array<Float, 6>& result)
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_players() == 2);
    LIBBOARDGAME_ASSERT(m_bd.get_nu_colors() == 2);
    ScoreType s;
    if (! m_is_symmetry_broken
            && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
    {
        if (log_simulations)
            LIBBOARDGAME_LOG("Symmetry not broken");
        s = 0;
    }
    else
        s = m_bd.get_score_twocolor(Color(0));
    Float res;
    if (s > 0)
        res = 1;
    else if (s < 0 || (m_is_callisto && s == 0))
        res = 0;
    else
        res = 0.5;
    if (log_simulations)
        LIBBOARDGAME_LOG("Result sco=", s, " game_res=", res);
    res += get_quality_bonus(Color(0), res, s);
    if (m_is_callisto)
        res += get_quality_bonus_attach_twocolor();
    if (log_simulations)
        LIBBOARDGAME_LOG("res=", res);
    result[0] = res;
    result[1] = 1.f - res;
}

Point State::find_best_starting_point(Color c) const
{
    // We use the starting point that maximizes the distance to occupied
    // starting points, especially to the ones occupied by the player (their
    // distance is weighted with a factor of 2).
    Point best = Point::null();
    float max_distance = -1;
    auto board_type = m_bd.get_board_type();
    bool is_trigon = (board_type == BoardType::trigon
                      || board_type == BoardType::trigon_3);
    bool is_nexos = board_type == BoardType::nexos;
    float ratio = (is_trigon ? 1.732f : 1);
    auto& geo = m_bd.get_geometry();
    for (Point p : m_bd.get_starting_points(c))
    {
        if (m_bd.is_forbidden(p, c))
            continue;
        if (is_nexos)
        {
            // Don't use the starting segments towards the edge of the board
            auto x = geo.get_x(p);
            if (x <= 3 || x >= geo.get_width() - 3 - 1)
                continue;
            auto y = geo.get_y(p);
            if (y <= 3 || y >= geo.get_height() - 3 - 1)
                continue;
        }
        auto px = static_cast<float>(geo.get_x(p));
        auto py = static_cast<float>(geo.get_y(p));
        float d = 0;
        for (Color i : Color::Range(m_nu_colors))
            for (Point pp : m_bd.get_starting_points(i))
            {
                PointState s = m_bd.get_point_state(pp);
                if (! s.is_empty())
                {
                    auto ppx = static_cast<float>(geo.get_x(pp));
                    auto ppy = static_cast<float>(geo.get_y(pp));
                    float dx = ppx - px;
                    float dy = ratio * (ppy - py);
                    float weight = 1;
                    if (s == c || s == m_bd.get_second_color(c))
                        weight = 2;
                    d += weight * sqrt(dx * dx + dy * dy);
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

bool State::gen_children(Tree::NodeExpander& expander, Float root_val)
{
    if (m_nu_passes == m_nu_colors)
        return true;
    Color to_play = m_bd.get_to_play();
    if (m_max_piece_size == 5)
    {
        if (! m_is_callisto)
        {
            init_moves_without_gamma<5, false>(to_play);
            return m_prior_knowledge.gen_children<5, 16, false>(
                        m_bd, m_moves[to_play], m_is_symmetry_broken,
                        expander, root_val);
        }
        init_moves_without_gamma<5, true>(to_play);
        return m_prior_knowledge.gen_children<5, 16, true>(
                    m_bd, m_moves[to_play], m_is_symmetry_broken,
                    expander, root_val);
    }
    if (m_max_piece_size == 6)
    {
        init_moves_without_gamma<6, false>(to_play);
        return m_prior_knowledge.gen_children<6, 22, false>(
                    m_bd, m_moves[to_play], m_is_symmetry_broken, expander,
                    root_val);
    }
    if (m_max_piece_size == 7)
    {
        init_moves_without_gamma<7, false>(to_play);
        return m_prior_knowledge.gen_children<7, 12, false>(
                    m_bd, m_moves[to_play], m_is_symmetry_broken, expander,
                    root_val);
    }
    LIBBOARDGAME_ASSERT(m_max_piece_size == 22);
    init_moves_without_gamma<22, false>(to_play);
    return m_prior_knowledge.gen_children<22, 44, false>(
                m_bd, m_moves[to_play], m_is_symmetry_broken, expander,
                root_val);
}

bool State::gen_playout_move_full(PlayerMove<Move>& mv)
{
    Color to_play = m_bd.get_to_play();
    while (true)
    {
        if (! m_is_move_list_initialized[to_play])
        {
            if (m_max_piece_size == 5)
            {
                if (m_is_callisto)
                    init_moves_with_gamma<5, 16, true>(to_play);
                else
                    init_moves_with_gamma<5, 16, false>(to_play);
            }
            else if (m_max_piece_size == 6)
                init_moves_with_gamma<6, 22, false>(to_play);
            else if (m_max_piece_size == 7)
                init_moves_with_gamma<7, 12, false>(to_play);
            else
                init_moves_with_gamma<22, 44, false>(to_play);
        }
        else if (m_has_moves[to_play])
        {
            if (m_max_piece_size == 5)
            {
                if (m_is_callisto)
                    update_moves<5, 16, true>(to_play);
                else
                    update_moves<5, 16, false>(to_play);
            }
            else if (m_max_piece_size == 6)
                update_moves<6, 22, false>(to_play);
            else if (m_max_piece_size == 7)
                update_moves<7, 12, false>(to_play);
            else
                update_moves<22, 44, false>(to_play);
        }
        if ((m_has_moves[to_play] = ! m_moves[to_play].empty()))
            break;
        if (++m_nu_passes == m_nu_colors)
            return false;
        if (m_check_terminate_early && m_bd.get_score_twoplayer(to_play) < 0
            && ! m_has_moves[m_bd.get_second_color(to_play)])
        {
            if (log_simulations)
                LIBBOARDGAME_LOG("Terminate early (no moves and neg. score)");
            return false;
        }
        to_play = to_play.get_next(m_nu_colors);
        m_bd.set_to_play(to_play);
        // Don't try to handle symmetry after pass moves
        m_is_symmetry_broken = true;
    }

    auto& moves = m_moves[to_play];
    LIBBOARDGAME_ASSERT(! moves.empty());
    auto total_gamma = m_cumulative_gamma[moves.size() - 1];
    if (log_simulations)
        LIBBOARDGAME_LOG("Moves: ", moves.size(), ", total_gamma: ",
                         total_gamma);
    auto begin = m_cumulative_gamma.begin();
    auto end = begin + moves.size();
    auto random = m_random.generate_float(0, total_gamma);
    auto pos = lower_bound(begin, end, random);
    LIBBOARDGAME_ASSERT(pos != end);
    mv = PlayerMove<Move>(get_player(),
                          moves[static_cast<unsigned>(pos - begin)]);
    return true;
}

string State::get_info() const
{
    ostringstream s;
    if (m_bd.get_nu_players() == 2)
    {
        s << "Sco: ";
        m_stat_score[Color(0)].write(s, true, 1);
    }
    s << '\n';
    return s.str();
}

inline const PieceMap<bool>& State::get_is_piece_considered(Color c) const
{
    if (m_is_callisto
            && m_bd.get_nu_left_piece(c, m_bd.get_one_piece()) > 1)
        return m_shared_const.is_piece_considered_none;
    // Use number of on-board pieces for move number to handle the case where
    // there are more pieces on the board than moves (setup positions)
    unsigned nu_moves = m_bd.get_nu_onboard_pieces();
    if (nu_moves >= m_shared_const.min_move_all_considered
            || m_force_consider_all_pieces)
        return m_shared_const.is_piece_considered_all;
    return *m_shared_const.is_piece_considered[nu_moves];
}

/** Initializes and returns m_pieces_considered if not all pieces are
    considered, otherwise m_bd.get_pieces_left(c) is returned. */
template<bool IS_CALLISTO>
inline const Board::PiecesLeftList& State::get_pieces_considered(Color c)
{
    auto is_piece_considered = m_is_piece_considered[c];
    auto& pieces_left = m_bd.get_pieces_left(c);
    if (is_piece_considered == &m_shared_const.is_piece_considered_all
            && ! IS_CALLISTO)
        return pieces_left;
    unsigned n = 0;
    for (Piece piece : pieces_left)
        if ((*is_piece_considered)[piece])
            m_pieces_considered.get_unchecked(n++) = piece;
    m_pieces_considered.resize(n);
    return m_pieces_considered;
}

/** Basic bonus added to the result for quality-based rewards.
    See also: Pepels et al.: Quality-based Rewards for Monte-Carlo Tree Search
    Simulations. ECAI 2014. */
inline Float State::get_quality_bonus(Color c, Float result, Float score)
{
    Float bonus = 0;

    // Game length
    auto l = static_cast<Float>(m_bd.get_nu_moves());
    m_stat_len.add(l);
    Float var = m_stat_len.get_variance();
    if (var > 0)
        bonus += -0.12f * (result - 0.5f)
                * sigmoid(2.f, (l - m_stat_len.get_mean()) / sqrt(var));

    // Game score
    auto& stat = m_stat_score[c];
    stat.add(score);
    var = stat.get_variance();
    if (var > 0)
        bonus += 0.3f * sigmoid(2.f, (score - stat.get_mean()) / sqrt(var));
    return bonus;
}

/** Additional quality-based rewards based on number of attach points.
    The number of non-forbidden attach points is another feature of a superior
    final position. Only used in some two-player variants, mainly helps in
    Trigon. */
inline Float State::get_quality_bonus_attach_twocolor()
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_players() == 2);
    int n = m_bd.get_attach_points(Color(0)).size()
            - m_bd.get_attach_points(Color(1)).size();
    for (Point p : m_bd.get_attach_points(Color(0)))
        n -= static_cast<int>(m_bd.is_forbidden(p, Color(0)));
    for (Point p : m_bd.get_attach_points(Color(1)))
        n += static_cast<int>(m_bd.is_forbidden(p, Color(1)));
    auto attach = static_cast<Float>(n);
    m_stat_attach.add(attach);
    auto var = m_stat_attach.get_variance();
    if (var > 0)
        return 0.1f * sigmoid(2.f,
                              (attach - m_stat_attach.get_mean()) / sqrt(var));
    return 0;
}

/** Like get_quality_bonus_attach_twocolor() but for 2 colors per player. */
inline Float State::get_quality_bonus_attach_multicolor()
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_players() == 2);
    LIBBOARDGAME_ASSERT(m_bd.get_nu_colors() == 4);
    int n = m_bd.get_attach_points(Color(0)).size()
            + m_bd.get_attach_points(Color(2)).size()
            - m_bd.get_attach_points(Color(1)).size()
            - m_bd.get_attach_points(Color(3)).size();
    for (Point p : m_bd.get_attach_points(Color(0)))
        n -= static_cast<int>(m_bd.is_forbidden(p, Color(0)));
    for (Point p : m_bd.get_attach_points(Color(2)))
        n -= static_cast<int>(m_bd.is_forbidden(p, Color(2)));
    for (Point p : m_bd.get_attach_points(Color(1)))
        n += static_cast<int>(m_bd.is_forbidden(p, Color(1)));
    for (Point p : m_bd.get_attach_points(Color(3)))
        n += static_cast<int>(m_bd.is_forbidden(p, Color(3)));
    auto attach = static_cast<Float>(n);
    m_stat_attach.add(attach);
    auto var = m_stat_attach.get_variance();
    if (var > 0)
        return 0.1f * sigmoid(2.f,
                              (attach - m_stat_attach.get_mean()) / sqrt(var));
    return 0;
}

void State::init_gamma()
{
    auto& bd = *m_shared_const.board;
    const auto piece_set = bd.get_piece_set();
    if (piece_set == PieceSet::gembloq)
    {
        static_assert(PlayoutFeatures::max_local + 1 >= 20, "");
        m_gamma_local[0] = 1;
        m_gamma_local[1] = 1e6f;
        m_gamma_local[2] = 1e6f;
        m_gamma_local[3] = 1e6f;
        m_gamma_local[4] = 1e6f;
        m_gamma_local[5] = 1e6f;
        m_gamma_local[6] = 1e6f;
        m_gamma_local[7] = 1e6f;
        m_gamma_local[8] = 1e12f;
        m_gamma_local[9] = 1e12f;
        m_gamma_local[10] = 1e12f;
        m_gamma_local[11] = 1e12f;
        m_gamma_local[12] = 1e18f;
        m_gamma_local[13] = 1e18f;
        m_gamma_local[14] = 1e18f;
        m_gamma_local[15] = 1e18f;
        m_gamma_local[16] = 1e24f;
        m_gamma_local[17] = 1e24f;
        m_gamma_local[18] = 1e24f;
        m_gamma_local[19] = 1e24f;
        for (unsigned i = 20; i < PlayoutFeatures::max_local + 1; ++i)
            m_gamma_local[i] = 1e25f;
    }
    else if (piece_set == PieceSet::trigon)
    {
        static_assert(PlayoutFeatures::max_local + 1 >= 5, "");
        m_gamma_local[0] = 1;
        m_gamma_local[1] = 1e6f;
        m_gamma_local[2] = 1e12f;
        m_gamma_local[3] = 1e18f;
        m_gamma_local[4] = 1e24f;
        for (unsigned i = 5; i < PlayoutFeatures::max_local + 1; ++i)
            m_gamma_local[i] = 1e30f;
    }
    else if (piece_set == PieceSet::nexos)
    {
        static_assert(PlayoutFeatures::max_local + 1 >= 4, "");
        m_gamma_local[0] = 1;
        m_gamma_local[1] = 1e6f;
        m_gamma_local[2] = 1e12f;
        m_gamma_local[3] = 1e18f;
        for (unsigned i = 4; i < PlayoutFeatures::max_local + 1; ++i)
            m_gamma_local[i] = 1e24f;
    }
    else
    {
        static_assert(PlayoutFeatures::max_local + 1 >= 5, "");
        m_gamma_local[0] = 1;
        m_gamma_local[1] = 1e6f;
        m_gamma_local[2] = 1e12f;
        m_gamma_local[3] = 1e18f;
        m_gamma_local[4] = 1e24f;
        for (unsigned i = 5; i < PlayoutFeatures::max_local + 1; ++i)
            m_gamma_local[i] = 1e25f;
    }
    float gamma_size_factor = 1;
    float gamma_nu_attach_factor = 1;
    switch (bd.get_board_type())
    {
    case BoardType::classic:
        gamma_size_factor = 5;
        break;
    case BoardType::duo:
        gamma_size_factor = 3;
        gamma_nu_attach_factor = 1.8f;
        break;
    case BoardType::trigon:
    case BoardType::trigon_3: // Not tuned
        gamma_size_factor = 5;
        break;
    case BoardType::nexos: // Not tuned
        gamma_size_factor = 5;
        gamma_nu_attach_factor = 1.8f;
        break;
    case BoardType::callisto_2:
    case BoardType::callisto: // Not tuned
    case BoardType::callisto_3: // Not tuned
        gamma_size_factor = 12;
        gamma_nu_attach_factor = 1.8f;
        break;
    case BoardType::gembloq_2:
    case BoardType::gembloq: // Not tuned
    case BoardType::gembloq_3: // Not tuned
        gamma_size_factor = 1.5f;
        break;
    }
    for (Piece::IntType i = 0; i < m_bc->get_nu_pieces(); ++i)
    {
        Piece piece(i);
        auto score_points = m_bc->get_piece_info(piece).get_score_points();
        auto piece_nu_attach =
                static_cast<float>(m_bc->get_nu_attach_points(piece));
        LIBBOARDGAME_ASSERT(score_points >= 0);
        LIBBOARDGAME_ASSERT(piece_nu_attach > 0);
        m_gamma_piece[piece] =
                pow(gamma_size_factor, score_points)
                * pow(gamma_nu_attach_factor, piece_nu_attach - 1);
    }
    if (m_is_callisto)
        // Playing 1-piece in Callisto early in playouts is bad, make sure it
        // gets a low gamma even if it is on a local point.
        m_gamma_piece[m_bd.get_one_piece()] = 1e-13f;
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
void State::init_moves_with_gamma(Color c)
{
    m_is_piece_considered[c] = &get_is_piece_considered(c);
    m_playout_features[c]
            .set_local<MAX_SIZE, MAX_ADJ_ATTACH, IS_CALLISTO>(m_bd);
    auto& marker = m_marker[c];
    auto& moves = m_moves[c];
    marker.clear(moves);
    auto& pieces = get_pieces_considered<IS_CALLISTO>(c);
    if (m_bd.is_first_piece(c) && ! IS_CALLISTO)
        add_starting_moves<MAX_SIZE>(c, pieces, true, moves);
    else
    {
        unsigned nu_moves = 0;
        float total_gamma = 0;
        if (IS_CALLISTO)
            add_callisto_one_piece_moves(c, true, total_gamma, moves, nu_moves);
        if (m_is_piece_considered[c]
                != &m_shared_const.is_piece_considered_none)
            for (Point p : m_bd.get_attach_points(c))
            {
                if (m_bd.is_forbidden(p, c))
                    continue;
                add_moves<MAX_SIZE>(p, c, pieces, total_gamma, moves,
                                    nu_moves);
                m_moves_added_at[c][p] = true;
            }
        moves.resize(nu_moves);
    }
    m_is_move_list_initialized[c] = true;
    m_nu_new_moves[c] = 0;
    m_last_attach_points_end[c] = m_bd.get_attach_points(c).end();
    if (moves.empty() &&
            m_is_piece_considered[c]
            != &m_shared_const.is_piece_considered_all)
    {
        m_force_consider_all_pieces = true;
        init_moves_with_gamma<MAX_SIZE, MAX_ADJ_ATTACH, IS_CALLISTO>(c);
    }
}

template<unsigned MAX_SIZE, bool IS_CALLISTO>
void State::init_moves_without_gamma(Color c)
{
    m_is_piece_considered[c] = &get_is_piece_considered(c);
    auto& marker = m_marker[c];
    auto& moves = m_moves[c];
    marker.clear(moves);
    auto& pieces = get_pieces_considered<IS_CALLISTO>(c);
    auto& is_forbidden = m_bd.is_forbidden(c);
    if (m_bd.is_first_piece(c) && ! IS_CALLISTO)
        add_starting_moves<MAX_SIZE>(c, pieces, false, moves);
    else
    {
        unsigned nu_moves = 0;
        if (IS_CALLISTO)
        {
            float total_gamma_dummy;
            add_callisto_one_piece_moves(c, false, total_gamma_dummy, moves,
                                         nu_moves);
        }
        if (m_is_piece_considered[c]
                != &m_shared_const.is_piece_considered_none)
            for (Point p : m_bd.get_attach_points(c))
            {
                if (is_forbidden[p])
                    continue;
                auto adj_status = m_bd.get_adj_status(p, c);
                for (Piece piece : pieces)
                {
                    if (! has_moves(c, piece, p, adj_status))
                        continue;
                    for (Move mv : get_moves(c, piece, p, adj_status))
                        if (! marker[mv]
                                && check_forbidden<MAX_SIZE>(
                                    is_forbidden, mv, moves, nu_moves))
                            marker.set(mv);
                }
                m_moves_added_at[c][p] = true;
            }
        moves.resize(nu_moves);
    }
    m_is_move_list_initialized[c] = true;
    m_nu_new_moves[c] = 0;
    m_last_attach_points_end[c] = m_bd.get_attach_points(c).end();
    if (moves.empty() &&
            m_is_piece_considered[c]
            != &m_shared_const.is_piece_considered_all)
    {
        m_force_consider_all_pieces = true;
        init_moves_without_gamma<MAX_SIZE, IS_CALLISTO>(c);
    }
}

void State::play_expanded_child(Move mv)
{
    if (log_simulations)
        LIBBOARDGAME_LOG("Playing expanded child");
    if (! mv.is_null())
        play_playout(mv);
    else
    {
        ++m_nu_passes;
        m_bd.set_to_play(m_bd.get_to_play().get_next(m_nu_colors));
        // Don't try to handle pass moves: a pass move either breaks symmetry
        // or both players have passed and it's the end of the game and we need
        // symmetry detection only as a heuristic (playouts and move value
        // initialization)
        m_is_symmetry_broken = true;
        if (log_simulations)
            LIBBOARDGAME_LOG(m_bd);
    }
}

void State::start_search()
{
    auto& bd = *m_shared_const.board;
    m_bd.copy_from(bd);
    m_bd.set_to_play(m_shared_const.to_play);
    m_bd.take_snapshot();
    m_nu_colors = bd.get_nu_colors();
    m_is_callisto = bd.is_callisto();
    for (Color c : Color::Range(m_nu_colors))
        m_playout_features[c].init_snapshot(m_bd, c);
    m_bc = &m_bd.get_board_const();
    m_max_piece_size = m_bc->get_max_piece_size();
    m_move_info_array = m_bc->get_move_info_array();
    m_move_info_ext_array = m_bc->get_move_info_ext_array();
    m_check_terminate_early =
            (bd.get_nu_moves() < 10u * m_nu_colors
             && m_bd.get_nu_players() == 2);
    auto variant = bd.get_variant();
    m_check_symmetric_draw =
            (has_central_symmetry(variant)
             && ! ((m_shared_const.to_play == Color(1)
                    || m_shared_const.to_play == Color(3))
                   && m_shared_const.avoid_symmetric_draw)
             && ! check_symmetry_broken(bd));
    if (! m_check_symmetric_draw)
        // Pretending that the symmetry is always broken is equivalent to
        // ignoring symmetric draws
        m_is_symmetry_broken = true;
    if (variant == Variant::trigon_2 || variant == Variant::callisto_2)
        m_symmetry_min_nu_pieces = 5;
    else
    {
        LIBBOARDGAME_ASSERT(! m_check_symmetric_draw || variant == Variant::duo
                            || variant == Variant::junior
                            || variant == Variant::gembloq_2);
        m_symmetry_min_nu_pieces = 3;
    }

    m_prior_knowledge.start_search(bd);
    m_stat_len.clear();
    m_stat_attach.clear();
    for (Color c : Color::Range(m_nu_colors))
        m_stat_score[c].clear();

    init_gamma();
}

void State::start_simulation(size_t n)
{
#ifdef LIBBOARDGAME_DISABLE_LOG
    LIBBOARDGAME_UNUSED(n);
#endif
    if (log_simulations)
        LIBBOARDGAME_LOG("=================================================\n",
                         "Simulation ", n, "\n",
                         "=================================================");
    m_bd.restore_snapshot();
    m_force_consider_all_pieces = false;
    auto& geo = m_bd.get_geometry();
    for (Color c : Color::Range(m_nu_colors))
    {
        m_has_moves[c] = true;
        m_is_move_list_initialized[c] = false;
        m_playout_features[c].restore_snapshot(m_bd);
        m_moves_added_at[c].fill(false, geo);
    }
    m_nu_passes = 0;
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
void State::update_moves(Color c)
{
    auto& playout_features = m_playout_features[c];
    playout_features.set_local<MAX_SIZE, MAX_ADJ_ATTACH, IS_CALLISTO>(m_bd);

    auto& marker = m_marker[c];

    // Find old moves that are still legal
    auto& is_forbidden = m_bd.is_forbidden(c);
    auto& moves = m_moves[c];
    unsigned nu_moves = 0;
    float total_gamma = 0;
    Piece piece;
    if (m_nu_new_moves[c] == 1 &&
            ! m_bd.is_piece_left(
                c, (piece =
                    get_move_info<MAX_SIZE>(m_last_move[c]).get_piece())))
        for (Move mv : moves)
        {
            auto& info = get_move_info<MAX_SIZE>(mv);
            if (info.get_piece() == piece
                    || ! check_move<MAX_SIZE>(
                             mv, info, moves, nu_moves, playout_features,
                             total_gamma))
                marker.clear(mv);
        }
    else
        for (Move mv : moves)
        {
            auto& info = get_move_info<MAX_SIZE>(mv);
            if (! m_bd.is_piece_left(c, info.get_piece())
                    || ! check_move<MAX_SIZE>(
                             mv, info, moves, nu_moves, playout_features,
                             total_gamma))
                marker.clear(mv);
        }

    // Find new legal moves because of new pieces played by this color
    auto& pieces = get_pieces_considered<IS_CALLISTO>(c);
    auto& attach_points = m_bd.get_attach_points(c);
    auto begin = m_last_attach_points_end[c];
    auto end = attach_points.end();
    for (auto i = begin; i != end; ++i)
        if (! is_forbidden[*i] && ! m_moves_added_at[c][*i])
        {
            m_moves_added_at[c][*i] = true;
            add_moves<MAX_SIZE>(*i, c, pieces, total_gamma, moves, nu_moves);
        }
    m_nu_new_moves[c] = 0;
    m_last_attach_points_end[c] = end;

    // Generate moves for pieces not considered in the last position
    if (m_is_piece_considered[c] != &m_shared_const.is_piece_considered_all)
    {
        auto& is_piece_considered = *m_is_piece_considered[c];
        if (nu_moves == 0)
            m_force_consider_all_pieces = true;
        auto& is_piece_considered_new = get_is_piece_considered(c);
        if (&is_piece_considered != &is_piece_considered_new)
        {
            Board::PiecesLeftList new_pieces;
            unsigned n = 0;
            for (Piece piece : m_bd.get_pieces_left(c))
                if (! is_piece_considered[piece]
                        && is_piece_considered_new[piece])
                    new_pieces.get_unchecked(n++) = piece;
            new_pieces.resize(n);
            for (Point p : attach_points)
                if (! is_forbidden[p])
                    add_moves<MAX_SIZE>(
                        p, c, new_pieces, total_gamma, moves, nu_moves);
            m_is_piece_considered[c] = &is_piece_considered_new;
        }
    }
    moves.resize(nu_moves);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
