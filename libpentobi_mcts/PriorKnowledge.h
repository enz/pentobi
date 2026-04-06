//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PriorKnowledge.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
#define LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H

#include "Float.h"
#include "LocalPoints.h"
#include "SearchParamConst.h"
#include "libboardgame_mcts/Tree.h"
#include "libpentobi_base/Board.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::Piece;
using libpentobi_base::PieceMap;

//-----------------------------------------------------------------------------

/** Initializes newly created nodes with move prior, count and value.
    Computes move priors of the form exp(phi*x) with a weight vector phi and a
    feature vector x. These weights can be learned with softmax training from
    existing games (see pentobi/src/learn_tool).

    The move generation also prunes certain moves in some game variants (e.g.
    opening moves that don't go towards the center). */
class PriorKnowledge
{
public:
    using Node =
        libboardgame_mcts::Node<Move, Float, SearchParamConst::multithread>;

    using Tree = libboardgame_mcts::Tree<Node>;


    explicit PriorKnowledge(const Board& bd);

    void start_search(const Board& bd);

    /** Generate children nodes initialized with prior knowledge.
        @return false If the tree has not enough capacity for the children. */
    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
    bool gen_children(const Board& bd, const MoveList& moves,
                      bool is_symmetry_broken, Tree::NodeExpander& expander,
                      Float root_val);

private:
    struct MoveFeatures
    {
        /** Heuristic unnormalized probability of the move. */
        Float gamma;

        /** Does the move touch a piece of the same player? */
        bool connect;

        /** Only used on Classic and Trigon boards. */
        float dist_to_center;
    };


    array<MoveFeatures, Move::range> m_features;

    /** @name Gammas for move scores. */
    /** @{ */

    Float m_gamma_point_other;

    /** Point is opponent attach point or adjacent to it. */
    Float m_gamma_point_opp_attach_or_nb;

    /** Point is attach point of second color. */
    Float m_gamma_point_second_color_attach;

    /** Adjacent point connects two own colors. */
    Float m_gamma_adj_connect;

    /** Adjacent point is occupied by opponent. */
    Float m_gamma_adj_occupied_other;

    Float m_gamma_adj_forbidden_other;

    /** Adjacent point is own attach point. */
    Float m_gamma_adj_own_attach;

    /** Adjacent point is not already forbidden. */
    Float m_gamma_adj_nonforbidden;

    Float m_gamma_attach_to_play;

    Float m_gamma_attach_forbidden_other;

    /** Attach point is attach point of another own color. */
    Float m_gamma_attach_second_color;

    /** Move occupies an attach point of a recent opponent move. */
    Float m_gamma_local;

    PieceMap<Float> m_gamma_piece_score;

    /** Attach point is nonforbidden and has n non-forbidden neighbors.
        Nexos/Callisto use "diagonal" neighbors instead of "adjacent", so the
        index is [0..6] */
    array<Float, 7> m_gamma_attach_nonforbidden;

    /** @} */ // @name

    /** Maximum of Features::gamma for all moves. */
    Float m_max_gamma;

    /** Sum of Features::gamma for all moves. */
    Float m_sum_gamma;

    bool m_has_connect_move;

    ColorMap<bool> m_check_dist_to_center;

    Variant m_variant;

    unsigned m_dist_to_center_max_pieces;

    float m_min_dist_to_center;

    float m_max_dist_diff;

    LocalPoints m_local_points;

    /** Distance to center heuristic. */
    GridExt<float> m_dist_to_center;


    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
    void compute_features(const Board& bd, const MoveList& moves,
                          bool check_dist_to_center, bool check_connect);

    void init_variant(const Board& bd);
};


template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
void PriorKnowledge::compute_features(const Board& bd, const MoveList& moves,
                                      bool check_dist_to_center,
                                      bool check_connect)
{
    auto to_play = bd.get_to_play();
    auto variant = bd.get_variant();
    Color second_color;
    // connect_color is the 2nd color of the player in game variants with 2
    // colors per player (connecting to_play and connect_color is good) and
    // to_play in other game variants (which disables the feature without
    // needing an extra check below because adj_value is not used for
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
    GridExt<Float> gamma_point;
    gamma_point[Point::null()] = 1;
    Grid<Float> gamma_attach;
    Grid<Float> gamma_adj;
    for (Point p : geo)
    {
        auto s = bd.get_point_state(p);
        if (is_forbidden[p])
        {
            // No need to initialize gamma_point[p] for forbidden points
            if (s != to_play)
                gamma_attach[p] = m_gamma_attach_forbidden_other;
            else
                gamma_attach[p] = m_gamma_attach_to_play;
            if (s == connect_color)
                gamma_adj[p] = m_gamma_adj_connect;
            else if (! s.is_empty())
                // Occupied by opponent (no need to check if s == to_play,
                // such moves are illegal)
                gamma_adj[p] = m_gamma_adj_occupied_other;
            else
                gamma_adj[p] = m_gamma_adj_forbidden_other;
        }
        else
        {
            gamma_point[p] = m_gamma_point_other;
            if (bd.is_attach_point(p, to_play))
                gamma_adj[p] = m_gamma_adj_own_attach;
            else
                gamma_adj[p] = m_gamma_adj_nonforbidden;
            unsigned n = 0;
            if (MAX_SIZE == 7 || IS_CALLISTO)
            {
                // Nexos and Callisto don't use "adjacent" points, use
                // "diagonal" instead
                LIBBOARDGAME_ASSERT(geo.get_adj(p).empty());
                for (auto pa : geo.get_diag(p))
                    n += 1u - static_cast<unsigned>(is_forbidden[pa]);
            }
            else
                for (auto pa : geo.get_adj(p))
                    n += 1u - static_cast<unsigned>(is_forbidden[pa]);
            LIBBOARDGAME_ASSERT(n < m_gamma_attach_nonforbidden.size());
            gamma_attach[p] = m_gamma_attach_nonforbidden[n];
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
                gamma_point[p] = m_gamma_point_opp_attach_or_nb;
                if (MAX_SIZE == 7 || IS_CALLISTO)
                    // Nexos or Callisto
                    LIBBOARDGAME_ASSERT(geo.get_adj(p).empty());
                else
                    for (Point j : geo.get_adj(p))
                        if (! is_forbidden[j])
                            gamma_point[j] = m_gamma_point_opp_attach_or_nb;
            }
    }
    if (second_color != to_play)
    {
        auto& is_forbidden_second_color = bd.is_forbidden(second_color);
        for (Point p : bd.get_attach_points(second_color))
            if (! is_forbidden_second_color[p])
            {
                gamma_point[p] *= m_gamma_point_second_color_attach;
                if (! is_forbidden[p])
                    gamma_attach[p] *= m_gamma_attach_second_color;
            }
    }
    m_max_gamma = -numeric_limits<Float>::max();
    m_sum_gamma = 0;
    m_min_dist_to_center = numeric_limits<unsigned short>::max();
    m_has_connect_move = false;
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        auto mv = moves[i];
        auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                    mv, move_info_ext_array);
        auto& features = m_features[i];
        auto& info = BoardConst::get_move_info<MAX_SIZE>(mv, move_info_array);
        auto j = info.begin();
        Float gamma = gamma_point[*j];
        bool local = m_local_points.contains(*j);
        if (! check_dist_to_center)
            for (unsigned k = 1; k < MAX_SIZE; ++k)
            {
                ++j;
                gamma *= gamma_point[*j];
                local |= m_local_points.contains(*j);
            }
        else
        {
            features.dist_to_center = m_dist_to_center[*j];
            for (unsigned k = 1; k < MAX_SIZE; ++k)
            {
                ++j;
                gamma *= gamma_point[*j];
                local |= m_local_points.contains(*j);
                features.dist_to_center =
                    min(features.dist_to_center, m_dist_to_center[*j]);
            }
            m_min_dist_to_center =
                min(m_min_dist_to_center, features.dist_to_center);
        }
        if (local)
            gamma *= m_gamma_local;
        j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        gamma *= gamma_attach[*j];
        while (++j != end)
            gamma *= gamma_attach[*j];
        if (MAX_SIZE == 7 || IS_CALLISTO)
        {
            // Nexos and Callisto don't use "adjacent" points, only "diagonal"
            // Use the features of gamma_adj also for the attach points
            LIBBOARDGAME_ASSERT(info_ext.size_adj_points == 0);
            LIBBOARDGAME_ASSERT(! check_connect);
            j = info_ext.begin_attach();
            end = info_ext.end_attach();
            for ( ; j != end; ++j)
            {
                gamma *= gamma_attach[*j];
                gamma *= gamma_adj[*j];
            }
        }
        else
        {
            j = info_ext.begin_adj();
            end = info_ext.end_adj();
            if (! check_connect)
            {
                for ( ; j != end; ++j)
                    gamma *= gamma_adj[*j];
            }
            else
            {
                features.connect = (bd.get_point_state(*j) == second_color);
                for ( ; j != end; ++j)
                {
                    gamma *= gamma_adj[*j];
                    if (bd.get_point_state(*j) == second_color)
                        features.connect = true;
                }
                if (features.connect)
                    m_has_connect_move = true;
            }
        }
        gamma *= m_gamma_piece_score[info.get_piece()];
        m_sum_gamma += gamma;
        if (gamma > m_max_gamma)
            m_max_gamma = gamma;
        features.gamma = gamma;
    }
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
bool PriorKnowledge::gen_children(const Board& bd, const MoveList& moves,
                                  bool is_symmetry_broken,
                                  Tree::NodeExpander& expander, Float root_val)
{
    if (moves.empty())
    {
        // Add a pass move. The in-tree phase of the search assumes alternating
        // moves, because the color of a move is not stored in the nodes and
        // it wouldn't know who is to play otherwise without generating moves.
        if (! expander.check_capacity(1))
            return false;
        expander.add_child(Move::null(), root_val,
                           SearchParamConst::child_min_count, 1);
        return true;
    }
    m_local_points.init<MAX_SIZE, MAX_ADJ_ATTACH>(bd);
    auto to_play = bd.get_to_play();
    auto nu_onboard_pieces = bd.get_nu_onboard_pieces();
    bool check_dist_to_center =
            (m_check_dist_to_center[to_play]
             && nu_onboard_pieces <= m_dist_to_center_max_pieces);
    bool check_connect =
        (bd.get_variant() == Variant::classic_2 && nu_onboard_pieces < 14);
    compute_features<MAX_SIZE, MAX_ADJ_ATTACH, IS_CALLISTO>(
                bd, moves, check_dist_to_center, check_connect);
    if (! m_has_connect_move)
        check_connect = false;
    bool has_symmetry_breaker = false;
    if (! is_symmetry_broken)
    {
        unsigned nu_moves = bd.get_nu_moves();
        if (to_play == Color(1) || to_play == Color(3))
        {
            if (nu_moves > 0)
            {
                // If a symmetric draw is still possible, encourage exploring
                // the move that keeps the symmetry
                ColorMove last = bd.get_move(nu_moves - 1);
                Move symmetric_mv =
                        bd.get_move_info_ext_2(last.move).symmetric_move;
                for (unsigned i = 0; i < moves.size(); ++i)
                    if (moves[i] == symmetric_mv)
                    {
                        m_sum_gamma -= m_features[i].gamma;
                        m_features[i].gamma *= 100.f;
                        m_sum_gamma += m_features[i].gamma;
                        if (m_features[i].gamma > m_max_gamma)
                            m_max_gamma = m_features[i].gamma;
                        break;
                    }
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
    auto inv_max_gamma = 1.f / m_max_gamma;
    auto inv_sum_gamma = 1.f / m_sum_gamma;

    for (unsigned i = 0; i < moves.size(); ++i)
    {
        const auto& features = m_features[i];
        // Depending on the game variant, prune early moves that don't minimize
        // dist to center and moves that don't connect in the middle
        if ((check_dist_to_center
             && features.dist_to_center > m_min_dist_to_center)
                || (check_connect && ! features.connect))
            continue;
        auto mv = moves[i];
        // If a symmetric draw is still possible, consider only moves that
        // break the symmetry
        if (has_symmetry_breaker
                && ! bd.get_move_info_ext_2(mv).breaks_symmetry)
            continue;
        Float move_prior = features.gamma * inv_sum_gamma;
        // Empirical good formula for value initialization
        Float value = root_val * sqrt(features.gamma * inv_max_gamma);
        LIBBOARDGAME_ASSERT(bd.is_legal(to_play, mv));
        expander.add_child(mv, value, SearchParamConst::child_min_count,
                           move_prior);
    }
    return true;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PRIOR_KNOWLEDGE_H
