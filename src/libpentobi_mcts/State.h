//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_H
#define LIBPENTOBI_MCTS_STATE_H

#include "PlayoutFeatures.h"
#include "PriorKnowledge.h"
#include "SharedConst.h"
#include "StateUtil.h"
#include "libboardgame_mcts/LastGoodReply.h"
#include "libboardgame_mcts/PlayerMove.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Statistics.h"

namespace libpentobi_mcts {

using libboardgame_mcts::LastGoodReply;
using libboardgame_mcts::PlayerInt;
using libboardgame_mcts::PlayerMove;
using libboardgame_util::log;
using libboardgame_util::RandomGenerator;
using libboardgame_util::Statistics;
using libpentobi_base::BoardConst;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** A state of a simulation.
    This class contains modifiable data used in a simulation. In multi-threaded
    search (not yet implemented), each thread uses its own instance of this
    class.
    This class incrementally keeps track of the legal moves.
    The randomization in the playouts is done by assigning a heuristically
    tuned gamma value to each move. The gamma value determines the probabilty
    that a move is played in the playout phase. */
class State
{
public:
    typedef libboardgame_mcts::Node<Move, Float, SearchParamConst::multithread>
        Node;

    typedef libboardgame_mcts::Tree<Node> Tree;

    typedef libboardgame_mcts::LastGoodReply<Move,
                                             SearchParamConst::max_players,
                                             SearchParamConst::lgr_hash_table_size,
                                             SearchParamConst::multithread>
        LastGoodReply;

    /** Constructor.
        @param initial_variant Game variant to initialize the internal
        board with (may avoid unnecessary BoardConst creation for game variant
        that is never used)
        @param shared_const (@ref libboardgame_doc_storesref) */
    State(Variant initial_variant, const SharedConst& shared_const);

    State& operator=(const State&) = delete;

    /** Play a move in the in-tree phase of the search. */
    void play_in_tree(Move mv);

    /** Handle end of in-tree phase. */
    void finish_in_tree();

    /** Play a move right after expanding a node. */
    void play_expanded_child(Move mv);

    /** Finish in-tree phase without expanding a node. */
    void finish_in_tree_no_expansion();

    /** Get current player to play. */
    PlayerInt get_player() const;

    void start_search();

    void start_simulation(size_t n);

    void gen_children(Tree::NodeExpander& expander, Float init_val);

    void start_playout();

    /** Generate a playout move.
        @return @c false if end of game was reached, and no move was
        generated. */
    bool gen_playout_move(const LastGoodReply& lgr, Move last,
                          Move second_last, PlayerMove<Move>& move);

    void evaluate_playout(array<Float, 6>& result);

    void play_playout(Move mv);

    /** Do not update RAVE values for n'th move of the current simulation. */
    bool skip_rave(Move mv) const;

#if LIBBOARDGAME_DEBUG
    string dump() const;
#endif

    string get_info() const;

private:
    static const bool log_simulations = false;

    /** The cumulative gamma value of the moves in m_moves. */
    array<double, Move::range> m_cumulative_gamma;

    Color::IntType m_nu_passes;

    const SharedConst& m_shared_const;

    Board m_bd;

    const BoardConst* m_bc;

    Color::IntType m_nu_colors;

    const MoveInfo* m_move_info_array;

    const MoveInfoExt* m_move_info_ext_array;

    /** Incrementally updated lists of legal moves for both colors.
        Only the move list for the color to play van be used in any given
        position, the other color is not updated immediately after a move. */
    ColorMap<MoveList> m_moves;

    ColorMap<const PieceMap<bool>*> m_is_piece_considered;

    Board::PiecesLeftList m_pieces_considered;

    PriorKnowledge m_prior_knowledge;

    /** Gamma value for a piece. */
    PieceMap<double> m_gamma_piece;

    /** Gamma value for PlayoutFeatures::get_nu_local(). */
    array<double, PlayoutFeatures::max_local + 1> m_gamma_local;

    /** Number of moves played by a color since the last update of its move
        list. */
    ColorMap<unsigned> m_nu_new_moves;

    /** Board::get_attach_points().end() for a color at the last update of
        its move list. */
    ColorMap<PointList::const_iterator> m_last_attach_points_end;

    /** Last move played by a color since the last update of its move list. */
    ColorMap<Move> m_last_move;

    ColorMap<bool> m_is_move_list_initialized;

    ColorMap<bool> m_has_moves;

    /** Marks moves contained in m_moves. */
    ColorMap<MoveMarker> m_marker;

    ColorMap<PlayoutFeatures> m_playout_features;

    RandomGenerator m_random;

    /** Used in get_quality_bonus(). */
    ColorMap<Statistics<Float>> m_stat_score;

    /** Used in get_quality_bonus(). */
    Statistics<Float> m_stat_len;

    /** Used in get_quality_bonus(). */
    Statistics<Float> m_stat_attach;

    bool m_check_symmetric_draw;

    bool m_check_terminate_early;

    bool m_is_symmetry_broken;

    /** Enforce all pieces to be considered for the rest of the simulation. */
    bool m_force_consider_all_pieces;

    /** Minimum number of pieces on board to perform a symmetry check.
        3 in Duo/Junior or 5 in Trigon because this is the earliest move number
        to break the symmetry. The early playout termination that evaluates all
        symmetric positions as a draw should not be used earlier because it can
        cause bad move selection in very short searches if all moves are
        evaluated as draw and the search is not deep enough to find that the
        symmetry can be broken a few moves later. */
    unsigned m_symmetry_min_nu_pieces;

    /** Remember attach points that were already used for move generation.
        Allows the incremental update of the move lists to skip attach points
        of newly played pieces that were already attach points of previously
        played pieces. */
    ColorMap<Grid<bool>> m_moves_added_at;


    void add_moves(Point p, Color c, const Board::PiecesLeftList& pieces,
                   double& total_gamma, MoveList& moves, unsigned& nu_moves);

    void add_moves(Point p, Color c, Piece piece, unsigned adj_status,
                   double& total_gamma, MoveList& moves, unsigned& nu_moves);

    LIBBOARDGAME_NOINLINE
    void add_starting_moves(Color c, const Board::PiecesLeftList& pieces,
                            bool with_gamma, MoveList& moves);

    void evaluate_duo(array<Float, 6>& result);

    void evaluate_multicolor(array<Float, 6>& result);

    void evaluate_multiplayer(array<Float, 6>& result);

    Point find_best_starting_point(Color c) const;

    Float get_quality_bonus(Color c, Float result, Float score,
                            bool use_nu_attach);

    /** Equivalent to but faster than m_bd.get_move_info() */
    const MoveInfo& get_move_info(Move move) const;

    /** Equivalent to but faster than m_bd.get_move_info_ext() */
    const MoveInfoExt& get_move_info_ext(Move move) const;

    PrecompMoves::Range get_moves(Color c, Piece piece, Point p,
                                  unsigned adj_status) const;

    const PieceMap<bool>& get_is_piece_considered() const;

    const Board::PiecesLeftList& get_pieces_considered(Color c);

    void init_moves_with_gamma(Color c);

    void init_moves_without_gamma(Color c);

    bool check_forbidden(const Grid<bool>& is_forbidden, Move mv,
                         MoveList& moves, unsigned& nu_moves);

    bool check_move(Move mv, const MoveInfo& info, double gamma_piece,
                    MoveList& moves, unsigned& nu_moves,
                    const PlayoutFeatures& playout_features,
                    double& total_gamma);

    bool check_move(Move mv, const MoveInfo& info, MoveList& moves,
                    unsigned& nu_moves,
                    const PlayoutFeatures& playout_features,
                    double& total_gamma);

    bool gen_playout_move_full(PlayerMove<Move>& mv);

    void update_moves(Color c);

    void update_playout_features(Color c, Move mv);

    void update_symmetry_broken(Move mv);
};

inline void State::evaluate_playout(array<Float, 6>& result)
{
    auto nu_players = m_bd.get_nu_players();
    if (nu_players == 2)
    {
        if (m_nu_colors == 2)
            evaluate_duo(result);
        else
            evaluate_multicolor(result);
    }
    else
        evaluate_multiplayer(result);
}

inline void State::finish_in_tree()
{
    if (log_simulations)
        LIBBOARDGAME_LOG("Finish in-tree");
    if (m_check_symmetric_draw)
        m_is_symmetry_broken =
            check_symmetry_broken(m_bd, m_shared_const.symmetric_points);
}

inline void State::gen_children(Tree::NodeExpander& expander, Float init_val)
{
    if (m_nu_passes == m_nu_colors)
        return;
    Color to_play = m_bd.get_to_play();
    init_moves_without_gamma(to_play);
    m_prior_knowledge.gen_children(m_bd, m_moves[to_play],
                                   m_is_symmetry_broken, expander, init_val);
}

inline bool State::gen_playout_move(const LastGoodReply& lgr, Move last,
                                    Move second_last, PlayerMove<Move>& mv)
{
    if (m_nu_passes == m_nu_colors)
        return false;
    if (! m_is_symmetry_broken
            && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
    {
        // See also the comment in evaluate_playout()
        if (log_simulations)
            LIBBOARDGAME_LOG("Terminate playout. Symmetry not broken.");
        return false;
    }
    PlayerInt player = get_player();
    Move lgr2 = lgr.get_lgr2(player, last, second_last);
    if (! lgr2.is_null() && m_bd.is_legal(lgr2))
    {
        if (log_simulations)
            LIBBOARDGAME_LOG("Playing last good reply 2");
        mv = PlayerMove<Move>(player, lgr2);
        return true;
    }
    Move lgr1 = lgr.get_lgr1(player, last);
    if (! lgr1.is_null() && m_bd.is_legal(lgr1))
    {
        if (log_simulations)
            LIBBOARDGAME_LOG("Playing last good reply 1");
        mv = PlayerMove<Move>(player, lgr1);
        return true;
    }
    return gen_playout_move_full(mv);
}

inline const MoveInfo& State::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_nu_moves());
    return *(m_move_info_array + mv.to_int());
}

inline const MoveInfoExt& State::get_move_info_ext(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_nu_moves());
    return *(m_move_info_ext_array + mv.to_int());
}

inline PrecompMoves::Range State::get_moves(Color c, Piece piece, Point p,
                                            unsigned adj_status) const
{
    return m_shared_const.precomp_moves[c].get_moves(piece, p, adj_status);
}

inline PlayerInt State::get_player() const
{
    unsigned player = m_bd.get_to_play().to_int();
    if ( m_bd.get_variant() == Variant::classic_3 && player == 3)
        player += m_bd.get_alt_player();
    return static_cast<PlayerInt>(player);
}

inline void State::play_in_tree(Move mv)
{
    Color to_play = m_bd.get_to_play();
    if (! mv.is_null())
    {
        LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
        m_bd.play(to_play, mv);
        m_nu_passes = 0;
        update_playout_features(to_play, mv);
    }
    else
    {
        m_bd.set_to_play(to_play.get_next(m_nu_colors));
        ++m_nu_passes;
    }
    if (log_simulations)
        LIBBOARDGAME_LOG(m_bd);
}

inline void State::play_playout(Move mv)
{
    auto to_play = m_bd.get_to_play();
    LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
    m_bd.play(to_play, mv);
    ++m_nu_new_moves[to_play];
    m_last_move[to_play] = mv;
    m_nu_passes = 0;
    if (! m_is_symmetry_broken)
        update_symmetry_broken(mv);
    if (log_simulations)
        LIBBOARDGAME_LOG(m_bd);
    update_playout_features(to_play, mv);
}

inline void State::start_playout()
{
}

inline bool State::skip_rave(Move mv) const
{
    LIBBOARDGAME_UNUSED(mv);
    return false;
}

inline void State::update_playout_features(Color c, Move mv)
{
    auto& info = get_move_info(mv);
    for (Color i : Color::Range(m_nu_colors))
        m_playout_features[i].set_forbidden(info);
    m_playout_features[c].set_forbidden(get_move_info_ext(mv));
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
