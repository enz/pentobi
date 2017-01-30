//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PriorKnowledge.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
#define LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H

#include "Float.h"
#include "SearchParamConst.h"
#include "libboardgame_mcts/Tree.h"
#include "libboardgame_util/MathUtil.h"
#include "libpentobi_base/Board.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_util::fast_exp;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::Grid;
using libpentobi_base::GridExt;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Initializes newly created nodes with heuristic prior count and value. */
class PriorKnowledge
{
public:
    typedef libboardgame_mcts::Node<Move, Float, SearchParamConst::multithread>
    Node;

    typedef libboardgame_mcts::Tree<Node> Tree;

    PriorKnowledge();

    void start_search(const Board& bd);

    /** Generate children nodes initialized with prior knowledge.
        @return false If the tree has not enough capacity for the children. */
    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    bool gen_children(const Board& bd, const MoveList& moves,
                      bool is_symmetry_broken, Tree::NodeExpander& expander,
                      Float root_val);

private:
    struct MoveFeatures
    {
        /** Heuristic value of the move expressed in score points. */
        Float heuristic;

        bool is_local;

        /** Does the move touch a piece of the same player? */
        bool connect;

        /** Only used on Classic and Trigon boards. */
        float dist_to_center;
    };


    array<MoveFeatures, Move::range> m_features;

    /** Maximum of Features::heuristic for all moves. */
    Float m_max_heuristic;

    bool m_has_connect_move;

    ColorMap<bool> m_check_dist_to_center;

    unsigned m_dist_to_center_max_pieces;

    float m_min_dist_to_center;

    float m_max_dist_diff;

    /** Marker for attach points of recent opponent moves. */
    GridExt<bool> m_is_local;

    /** Points in m_is_local with value greater zero. */
    PointList m_local_points;

    /** Distance to center heuristic. */
    GridExt<float> m_dist_to_center;


    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void compute_features(const Board& bd, const MoveList& moves,
                          bool check_dist_to_center, bool check_connect);

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void init_local(const Board& bd);
};


template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
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
    GridExt<Float> point_value;
    point_value[Point::null()] = 0;
    Grid<Float> attach_point_value;
    Grid<Float> adj_point_value;
    for (Point p : geo)
    {
        auto s = bd.get_point_state(p);
        if (is_forbidden[p])
        {
            if (s != to_play)
                attach_point_value[p] = -2.5;
            else
                attach_point_value[p] = 0.5;
            if (s == connect_color)
                // Connecting own colors is good
                adj_point_value[p] = 1;
            else if (! s.is_empty())
                // Touching opponent is better than playing elsewhere (no need to
                // check if s == to_play, such moves are illegal)
                adj_point_value[p] = 0.4f;
            else
                adj_point_value[p] = 0;
        }
        else
        {
            point_value[p] = 1;
            attach_point_value[p] = 0.5;
            if (bd.is_attach_point(p, to_play))
                // Making own attach point forbidden is especially bad
                adj_point_value[p] = -1;
            else
                // Creating new forbidden points is a bad thing
                adj_point_value[p] = -0.1f;
        }
    }
    for (Color c : bd.get_colors())
    {
        if (c == to_play || c == second_color)
            continue;
        auto& is_forbidden = bd.is_forbidden(c);
        for (Point p : bd.get_attach_points(c))
            if (! is_forbidden[p])
            {
                // Occupying opponent attach points or points adjacent to them
                // is very good
                point_value[p] = 3.f;
                for (Point j : geo.get_adj(p))
                    if (! is_forbidden[j])
                        point_value[j] = 3.f;
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
        auto mv = moves[i];
        auto info = BoardConst::get_move_info<MAX_SIZE>(mv, move_info_array);
        auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                    mv, move_info_ext_array);
        auto& features = m_features[i];
        auto j = info.begin();
        Float heuristic = point_value[*j];
        bool local = m_is_local[*j];
        if (! check_dist_to_center)
            for (unsigned k = 1; k < MAX_SIZE; ++k)
            {
                ++j;
                heuristic += point_value[*j];
                // Logically, we mean: local = local || m_is_local[*j]
                // But this generates branches, which are bad for performance
                // in this tight loop (unrolled by the compiler). So we use a
                // bitwise OR, which works because C++ guarantees that
                // true/false converts to 1/0.
                local |= m_is_local[*j];
            }
        else
        {
            features.dist_to_center = m_dist_to_center[*j];
            for (unsigned k = 1; k < MAX_SIZE; ++k)
            {
                ++j;
                heuristic += point_value[*j];
                // See comment above about bitwise OR on bool
                local |= m_is_local[*j];
                features.dist_to_center =
                    min(features.dist_to_center, m_dist_to_center[*j]);
            }
            m_min_dist_to_center =
                min(m_min_dist_to_center, features.dist_to_center);
        }
        j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        heuristic += attach_point_value[*j];
        while (++j != end)
            heuristic += attach_point_value[*j];
        if (MAX_SIZE == 7) // Nexos
        {
            LIBBOARDGAME_ASSERT(info_ext.size_adj_points == 0);
            LIBBOARDGAME_ASSERT(! check_connect);
        }
        else
        {
            j = info_ext.begin_adj();
            end = info_ext.end_adj();
            if (! check_connect)
            {
                for ( ; j != end; ++j)
                    heuristic += adj_point_value[*j];
            }
            else
            {
                features.connect = (bd.get_point_state(*j) == second_color);
                for ( ; j != end; ++j)
                {
                    heuristic += adj_point_value[*j];
                    if (bd.get_point_state(*j) == second_color)
                        features.connect = true;
                }
                if (features.connect)
                    m_has_connect_move = true;
            }
        }
        if (heuristic > m_max_heuristic)
            m_max_heuristic = heuristic;
        features.heuristic = heuristic;
        features.is_local = local;
    }
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
bool PriorKnowledge::gen_children(const Board& bd, const MoveList& moves,
                                  bool is_symmetry_broken,
                                  Tree::NodeExpander& expander, Float root_val)
{
    if (moves.empty())
    {
        // Add a pass move. The initialization value does not matter for a
        // single child, but we need to use SearchParamConst::child_min_count
        // for the count to avoid an assertion.
        if (! expander.check_capacity(1))
            return false;
        expander.add_child(Move::null(), root_val, 3);
        return true;
    }
    init_local<MAX_SIZE, MAX_ADJ_ATTACH>(bd);
    auto to_play = bd.get_to_play();
    auto nu_onboard_pieces = bd.get_nu_onboard_pieces();
    bool check_dist_to_center =
            (m_check_dist_to_center[to_play]
             && nu_onboard_pieces <= m_dist_to_center_max_pieces);
    bool check_connect =
        (bd.get_variant() == Variant::classic_2 && nu_onboard_pieces < 14);
    compute_features<MAX_SIZE, MAX_ADJ_ATTACH>(bd, moves, check_dist_to_center,
                                               check_connect);
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
    m_min_dist_to_center += m_max_dist_diff;
    if (! expander.check_capacity(static_cast<unsigned short>(moves.size())))
        return false;
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        const auto& features = m_features[i];

        // Depending on the game variant, prune early moves that don't minimize
        // dist to center and moves that don't connect in the middle if
        // connection is possible
        if ((check_dist_to_center
             && features.dist_to_center > m_min_dist_to_center)
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

        // Initialize value from heuristic and root_val, each with a count
        // of 1.5. If this is changed, SearchParamConst::child_min_count
        // should be updated.
        Float value = 1.5f * (heuristic + root_val);
        Float count = 3;

        // If a symmetric draw is still possible, encourage exploring a move
        // that keeps or breaks the symmetry by adding 5 wins or 5 losses
        // See also the comment in evaluate_playout()
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

        LIBBOARDGAME_ASSERT(bd.is_legal(to_play, mv));
        expander.add_child(mv, value / count, count);
    }
    return true;
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void PriorKnowledge::init_local(const Board& bd)
{
    for (Point p : m_local_points)
        m_is_local[p] = false;
    unsigned nu_local = 0;
    Color to_play = bd.get_to_play();
    Color second_color;
    if (bd.get_variant() == Variant::classic_3 && to_play.to_int() == 3)
        second_color = Color(bd.get_alt_player());
    else
        second_color = bd.get_second_color(to_play);
    auto& moves = bd.get_moves();
    auto move_info_ext_array = bd.get_board_const().get_move_info_ext_array();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-color variants)
    auto end = moves.end();
    auto begin = (end - moves.begin() < 3 ? moves.begin() : end - 3);
    for (auto i = begin; i != end; ++i)
    {
        Color c = i->color;
        if (c == to_play || c == second_color)
            continue;
        auto mv = i->move;
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                    mv, move_info_ext_array);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
            if (! is_forbidden[*j] && ! m_is_local[*j])
            {
                m_local_points.get_unchecked(nu_local++) = *j;
                m_is_local[*j] = true;
            }
        while (++j != end);
    }
    m_local_points.resize(nu_local);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
