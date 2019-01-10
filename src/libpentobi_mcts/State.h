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
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Statistics.h"

namespace libpentobi_mcts {

using libboardgame_mcts::LastGoodReply;
using libboardgame_mcts::PlayerInt;
using libboardgame_mcts::PlayerMove;
using libboardgame_util::RandomGenerator;
using libboardgame_util::Statistics;
using libpentobi_base::BoardConst;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::PieceSet;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** A state of a simulation.
    This class contains modifiable data used in a simulation. In multi-threaded
    search (not yet implemented), each thread uses its own instance of this
    class.
    This class incrementally keeps track of the legal moves.
    The randomization in the playouts is done by assigning a heuristically
    tuned gamma value to each move. The gamma value determines the probability
    that a move is played in the playout phase. */
class State
{
public:
    using Node =
        libboardgame_mcts::Node<Move, Float, SearchParamConst::multithread>;

    using Tree = libboardgame_mcts::Tree<Node>;

    using LastGoodReply =
        libboardgame_mcts::LastGoodReply<Move,
                                         SearchParamConst::max_players,
                                         SearchParamConst::lgr_hash_table_size,
                                         SearchParamConst::multithread>;


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

    /** Get current player to play. */
    PlayerInt get_player() const;

    void start_search();

    void start_simulation(size_t n);

    bool gen_children(Tree::NodeExpander& expander, Float root_val);

    void start_playout() { }

    /** Generate a playout move.
        @return @c false if end of game was reached, and no move was
        generated. */
    bool gen_playout_move(const LastGoodReply& lgr, Move last,
                          Move second_last, PlayerMove<Move>& mv);

    void evaluate_playout(array<Float, 6>& result);

    void play_playout(Move mv);

    /** Check if RAVE value for this move should not be updated. */
    bool skip_rave(Move mv) const;

#ifdef LIBBOARDGAME_DEBUG
    string dump() const;
#endif

    string get_info() const;

private:
    /** The cumulative gamma value of the moves in m_moves. */
    array<float, MoveList::max_size> m_cumulative_gamma;

    Color::IntType m_nu_passes;

    const SharedConst& m_shared_const;

    Board m_bd;

    const BoardConst* m_bc;

    Color::IntType m_nu_colors;

    BoardConst::MoveInfoArray m_move_info_array;

    BoardConst::MoveInfoExtArray m_move_info_ext_array;

    /** Incrementally updated lists of legal moves for both colors.
        Only the move list for the color to play van be used in any given
        position, the other color is not updated immediately after a move. */
    ColorMap<MoveList> m_moves;

    ColorMap<const PieceMap<bool>*> m_is_piece_considered;

    /** The list of pieces considered in the current move if not all pieces
        are considered. */
    Board::PiecesLeftList m_pieces_considered;

    PriorKnowledge m_prior_knowledge;

    /** Gamma value for PlayoutFeatures::get_nu_local(). */
    array<float, PlayoutFeatures::max_local + 1> m_gamma_local;

    /** Gamma value for a piece. */
    PieceMap<float> m_gamma_piece;

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

    /** Enforce all pieces to be considered for the rest of the simulation.
        This applies to all colors, because it is only used if no moves were
        generated because not all pieces were considered and this case is so
        rare that it is not worth the cost of setting such a flag for each
        color individually. */
    bool m_force_consider_all_pieces;

    bool m_is_callisto;

    /** Minimum number of pieces on board to perform a symmetry check.
        3 in Duo/Junior or 5 in Trigon because this is the earliest move number
        to break the symmetry. The early playout termination that evaluates all
        symmetric positions as a draw should not be used earlier because it can
        cause bad move selection in very short searches if all moves are
        evaluated as draw and the search is not deep enough to find that the
        symmetry can be broken a few moves later. */
    unsigned m_symmetry_min_nu_pieces;

    /** Cache of m_bc->get_max_piece_size() */
    unsigned m_max_piece_size;

    /** Remember attach points that were already used for move generation.
        Allows the incremental update of the move lists to skip attach points
        of newly played pieces that were already attach points of previously
        played pieces. */
    ColorMap<Grid<bool>> m_moves_added_at;


    template<unsigned MAX_SIZE>
    void add_moves(Point p, Color c, const Board::PiecesLeftList& pieces,
                   float& total_gamma, MoveList& moves, unsigned& nu_moves);

    template<unsigned MAX_SIZE>
    LIBBOARDGAME_NOINLINE
    void add_starting_moves(Color c, const Board::PiecesLeftList& pieces,
                            bool with_gamma, MoveList& moves);

    LIBBOARDGAME_NOINLINE
    void add_callisto_one_piece_moves(Color c, bool with_gamma,
                                      float& total_gamma, MoveList& moves,
                                      unsigned& nu_moves);

    void evaluate_multicolor(array<Float, 6>& result);

    void evaluate_multiplayer(array<Float, 6>& result);

    void evaluate_twocolor(array<Float, 6>& result);

    Point find_best_starting_point(Color c) const;

    Float get_quality_bonus(Color c, Float result, Float score);

    Float get_quality_bonus_attach_twocolor();

    Float get_quality_bonus_attach_multicolor();

    template<unsigned MAX_SIZE>
    const MoveInfo<MAX_SIZE>& get_move_info(Move mv) const;

    template<unsigned MAX_ADJ_ATTACH>
    const MoveInfoExt<MAX_ADJ_ATTACH>& get_move_info_ext(Move mv) const;

    PrecompMoves::Range get_moves(Color c, Piece piece, Point p,
                                  unsigned adj_status) const;

    bool has_moves(Color c, Piece piece, Point p, unsigned adj_status) const;

    const PieceMap<bool>& get_is_piece_considered(Color c) const;

    template<bool IS_CALLISTO>
    const Board::PiecesLeftList& get_pieces_considered(Color c);

    void init_gamma();

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
    void init_moves_with_gamma(Color c);

    template<unsigned MAX_SIZE, bool IS_CALLISTO>
    void init_moves_without_gamma(Color c);

    template<unsigned MAX_SIZE>
    bool check_forbidden(const GridExt<bool>& is_forbidden, Move mv,
                         MoveList& moves, unsigned& nu_moves);

    bool check_lgr(Move mv) const;

    template<unsigned MAX_SIZE>
    bool check_move(Move mv, const MoveInfo<MAX_SIZE>& info, float gamma_piece,
                    MoveList& moves, unsigned& nu_moves,
                    const PlayoutFeatures& playout_features,
                    float& total_gamma);

    template<unsigned MAX_SIZE>
    bool check_move(Move mv, const MoveInfo<MAX_SIZE>& info, MoveList& moves,
                    unsigned& nu_moves,
                    const PlayoutFeatures& playout_features,
                    float& total_gamma);

    bool gen_playout_move_full(PlayerMove<Move>& mv);

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
    void update_moves(Color c);

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void update_playout_features(Color c, Move mv);

    template<unsigned MAX_SIZE>
    LIBBOARDGAME_NOINLINE void update_symmetry_broken(Move mv);
};

/** Check if last-good-reply move is applicable.
    To be faster, it doesn't check for starting moves because such moves rarely
    occur in the playout phase and doesn't check if a 1-piece move is in the
    center in Callisto because such moves are not generated in the search. */
inline bool State::check_lgr(Move mv) const
{
    if (mv.is_null())
        return false;
    Color c = m_bd.get_to_play();
    auto piece = m_bd.get_move_piece(mv);
    if (! m_bd.is_piece_left(c, piece))
        return false;
    auto points = m_bd.get_move_points(mv);
    auto i = points.begin();
    auto end = points.end();
    int has_attach_point = 0;
    do
    {
        if (m_bd.is_forbidden(*i, c))
            return false;
        has_attach_point |= static_cast<int>(m_bd.is_attach_point(*i, c));
    }
    while (++i != end);
    if (m_is_callisto)
    {
        Piece one_piece = m_bd.get_one_piece();
        if (piece == one_piece)
            return true;
        if (m_bd.get_nu_left_piece(c, one_piece) > 1 && piece != one_piece)
            return false;
    }
    return has_attach_point != 0;
}

inline void State::evaluate_playout(array<Float, 6>& result)
{
    auto nu_players = m_bd.get_nu_players();
    if (nu_players == 2)
    {
        if (m_nu_colors == 2)
            evaluate_twocolor(result);
        else
            evaluate_multicolor(result);
    }
    else
        evaluate_multiplayer(result);
}

inline void State::finish_in_tree()
{
    if (m_check_symmetric_draw)
        m_is_symmetry_broken = check_symmetry_broken(m_bd);
}

inline bool State::gen_playout_move(const LastGoodReply& lgr, Move last,
                                    Move second_last, PlayerMove<Move>& mv)
{
    if (m_nu_passes == m_nu_colors)
        return false;
    if (! m_is_symmetry_broken
            && m_bd.get_nu_onboard_pieces() >= m_symmetry_min_nu_pieces)
        // See also the comment in evaluate_playout()
        return false;
    PlayerInt player = get_player();
    Move lgr2 = lgr.get_lgr2(player, last, second_last);
    if (check_lgr(lgr2))
    {
        mv = PlayerMove(player, lgr2);
        return true;
    }
    Move lgr1 = lgr.get_lgr1(player, last);
    if (check_lgr(lgr1))
    {
        mv = PlayerMove(player, lgr1);
        return true;
    }
    return gen_playout_move_full(mv);
}

template<unsigned MAX_SIZE>
inline const MoveInfo<MAX_SIZE>& State::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_range());
    return BoardConst::get_move_info<MAX_SIZE>(mv, m_move_info_array);
}

template<unsigned MAX_ADJ_ATTACH>
inline const MoveInfoExt<MAX_ADJ_ATTACH>& State::get_move_info_ext(
        Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_range());
    return BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                mv, m_move_info_ext_array);
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

inline bool State::has_moves(Color c, Piece piece, Point p,
                             unsigned adj_status) const
{
    return m_shared_const.precomp_moves[c].has_moves(piece, p, adj_status);
}

inline void State::play_in_tree(Move mv)
{
    Color to_play = m_bd.get_to_play();
    if (! mv.is_null())
    {
        LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
        m_nu_passes = 0;
        if (m_max_piece_size == 5)
        {
            m_bd.play<5, 16>(to_play, mv);
            update_playout_features<5, 16>(to_play, mv);
        }
        else if (m_max_piece_size == 6)
        {
            m_bd.play<6, 22>(to_play, mv);
            update_playout_features<6, 22>(to_play, mv);
        }
        else if (m_max_piece_size == 7)
        {
            m_bd.play<7, 12>(to_play, mv);
            update_playout_features<7, 12>(to_play, mv);
        }
        else
        {
            m_bd.play<22, 44>(to_play, mv);
            update_playout_features<22, 44>(to_play, mv);
        }
    }
    else
    {
        ++m_nu_passes;
        m_bd.set_to_play(to_play.get_next(m_nu_colors));
    }
}

inline void State::play_playout(Move mv)
{
    auto to_play = m_bd.get_to_play();
    LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
    if (m_max_piece_size == 5)
    {
        m_bd.play<5, 16>(to_play, mv);
        update_playout_features<5, 16>(to_play, mv);
        if (! m_is_symmetry_broken)
            update_symmetry_broken<5>(mv);
    }
    else if (m_max_piece_size == 6)
    {
        m_bd.play<6, 22>(to_play, mv);
        update_playout_features<6, 22>(to_play, mv);
        if (! m_is_symmetry_broken)
            update_symmetry_broken<6>(mv);
    }
    else if (m_max_piece_size == 7)
    {
        m_bd.play<7, 12>(to_play, mv);
        update_playout_features<7, 12>(to_play, mv);
        // No game variant with piece size 7 uses m_is_symmetry_broken
        LIBBOARDGAME_ASSERT(m_is_symmetry_broken);
    }
    else
    {
        m_bd.play<22, 44>(to_play, mv);
        update_playout_features<22, 44>(to_play, mv);
        if (! m_is_symmetry_broken)
            update_symmetry_broken<22>(mv);
    }
    ++m_nu_new_moves[to_play];
    m_last_move[to_play] = mv;
    m_nu_passes = 0;
}

inline bool State::skip_rave([[maybe_unused]] Move mv) const
{
    return false;
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void State::update_playout_features(Color c, Move mv)
{
    auto& info = get_move_info<MAX_SIZE>(mv);
    for (Color i : Color::Range(m_nu_colors))
        m_playout_features[i].set_forbidden(info);
    if (MAX_SIZE == 7) // Nexos
        LIBBOARDGAME_ASSERT(get_move_info_ext<MAX_ADJ_ATTACH>(mv).size_adj_points == 0);
    else
        m_playout_features[c].set_forbidden<MAX_ADJ_ATTACH>(
                    get_move_info_ext<MAX_ADJ_ATTACH>(mv));
}

template<unsigned MAX_SIZE>
void State::update_symmetry_broken(Move mv)
{
    Color to_play = m_bd.get_to_play();
    Color second_color = m_bd.get_second_color(to_play);
    auto& symmetric_points = m_bc->get_symmetrc_points();
    auto& info = get_move_info<MAX_SIZE>(mv);
    auto i = info.begin();
    auto end = info.end();
    if (to_play == Color(0) || to_play == Color(2))
    {
        // First player to play: Check that all symmetric points of the last
        // move of the second player are occupied by the first player
        do
        {
            Point symm_p = symmetric_points[*i];
            if (m_bd.get_point_state(symm_p) != second_color)
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
        while (++i != end);
    }
    else
    {
        // Second player to play: Check that all symmetric points of the last
        // move of the first player are empty (i.e. the second player can play
        // there to preserve the symmetry)
        do
        {
            Point symm_p = symmetric_points[*i];
            if (! m_bd.get_point_state(symm_p).is_empty())
            {
                m_is_symmetry_broken = true;
                return;
            }
        }
        while (++i != end);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
