//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_H
#define LIBPENTOBI_MCTS_STATE_H

#include "PlayoutFeatures.h"
#include "PriorKnowledge.h"
#include "StateUtil.h"
#include "libboardgame_mcts/PlayerMove.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Statistics.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::PlayerInt;
using libboardgame_mcts::PlayerMove;
using libboardgame_util::log;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;
using libboardgame_util::Statistics;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::Color;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMap;
using libpentobi_base::Marker;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::MoveMarker;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::PieceMap;
using libpentobi_base::PrecompMoves;
using libpentobi_base::SymmetricPoints;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Constant data shared between the search states. */
struct SharedConst
{
    /** Precomputed moves additionally constrained by moves that are
        non-forbidden at root position. */
    ColorMap<PrecompMoves> precomp_moves;

    /** The game board.
        Contains the current position. */
    const Board* board;

    /** The color to play at the root of the search. */
    const Color& to_play;

    bool avoid_symmetric_draw;

    /** Lookup table for symmetric points (only used in Duo and Trigon). */
    SymmetricPoints symmetric_points;

    /** Precomputed information if move is forbidden at the start position (and
        therefore in all positions in the search). */
    ColorMap<MoveMarker> is_forbidden_at_root;

    /** Minimum move number where all pieces are considered until the rest
        of the simulation. */
    unsigned min_move_all_considered;

    /** Precomputed lists of considered pieces depending on the move number.
        Only initialized for move numbers less than min_move_all_considered.
        Contains pointers to unique values auch that the comparison of the
        lists can be done by comparing the pointers to the lists. */
    array<const PieceMap<bool>*, Board::max_game_moves> is_piece_considered;

    /** List of unique values for is_piece_considered. */
    ArrayList<PieceMap<bool>, Board::max_game_moves> is_piece_considered_list;

    /** Precomputed lists of considered pieces if all pieces are enforced to be
        considered (because using the restricted set of pieces would generate
        no moves). */
    PieceMap<bool> is_piece_considered_all;

    SharedConst(const Color& to_play);
};

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
    typedef libboardgame_mcts::Node<Move, Float> Node;

    typedef libboardgame_mcts::Tree<Node> Tree;

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

    PlayerInt get_to_play() const;

    void start_search();

    void start_simulation(size_t n);

    void gen_children(Tree::NodeExpander& expander, Float init_val);

    void start_playout();

    /** Generate a playout move.
        @return @c false if end of game was reached, and no move was
        generated. */
    bool gen_playout_move(Move lgr1, Move lgr2, PlayerMove<Move>& move);

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

    PriorKnowledge m_prior_knowledge;

    /** Gamma value for a piece. */
    PieceMap<double> m_gamma_piece;

    /** Gamma value for PlayoutFeatures::get_nu_attach(). */
    array<double, PieceInfo::max_size + 1> m_gamma_nu_attach;

    /** Moves played by a color since the last update of its move list. */
    ColorMap<ArrayList<Move, Board::max_player_moves>> m_new_moves;

    ColorMap<bool> m_is_move_list_initialized;

    ColorMap<bool> m_has_moves;

    /** Marks moves contained in m_moves. */
    ColorMap<MoveMarker> m_marker;

    ColorMap<PlayoutFeatures> m_playout_features;

    RandomGenerator m_random;

    /** Statistics of the score for each color.
        Used for normalizing the score modification of the game result. */
    ColorMap<Statistics<Float>> m_stat_score;

    /** Statistics of the length of a simulation.
        Used for normalizing the length modification of the game result. */
    Statistics<Float> m_stat_len;

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
    ColorMap<Marker> m_moves_added_at;

    void add_moves(Point p, Color c,
                   const Board::PiecesLeftList& pieces_considered);

    void add_moves(Point p, Color c, Piece piece, unsigned adj_status,
                   double& total_gamma);

    LIBBOARDGAME_NOINLINE
    void add_starting_moves(Color c,
                            const Board::PiecesLeftList& pieces_considered,
                            bool with_gamma);

    void evaluate_duo(array<Float, 6>& result);

    void evaluate_multicolor(array<Float, 6>& result);

    void evaluate_multiplayer(array<Float, 6>& result);

    Point find_best_starting_point(Color c) const;

    Float get_eval_bonus(Color c, Float result, Float score);

    /** Equivalent to but faster than m_bd.get_move_info() */
    const MoveInfo& get_move_info(Move move) const;

    /** Equivalent to but faster than m_bd.get_move_info_ext() */
    const MoveInfoExt& get_move_info_ext(Move move) const;

    PrecompMoves::LocalMovesListRange get_moves(Color c, Piece piece, Point p,
                                                unsigned adj_status) const;

    const PieceMap<bool>& get_pieces_considered() const;

    void init_moves_with_gamma(Color c);

    void init_moves_without_gamma(Color c);

    bool check_forbidden(const Grid<bool>& is_forbidden, Move mv);

    bool check_move(Move mv, const MoveInfo& info, MoveList& moves,
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
        log("Finish in-tree");
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

inline bool State::gen_playout_move(Move lgr1, Move lgr2, PlayerMove<Move>& mv)
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
    Color to_play = m_bd.get_to_play();
    if (! lgr2.is_null() && m_bd.is_legal(lgr2))
    {
        if (log_simulations)
            log("Playing last good reply 2");
        mv = PlayerMove<Move>(to_play.to_int(), lgr2);
        return true;
    }
    if (! lgr1.is_null() && m_bd.is_legal(lgr1))
    {
        if (log_simulations)
            log("Playing last good reply 1");
        mv = PlayerMove<Move>(to_play.to_int(), lgr1);
        return true;
    }
    return gen_playout_move_full(mv);
}

inline const MoveInfo& State::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_nu_all_moves());
    return *(m_move_info_array + mv.to_int());
}

inline const MoveInfoExt& State::get_move_info_ext(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_nu_all_moves());
    return *(m_move_info_ext_array + mv.to_int());
}

inline PrecompMoves::LocalMovesListRange State::get_moves(
                      Color c, Piece piece, Point p, unsigned adj_status) const
{
    return m_shared_const.precomp_moves[c].get_moves(piece, p, adj_status);
}

inline PlayerInt State::get_to_play() const
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
        log(m_bd);
}

inline void State::play_playout(Move mv)
{
    LIBBOARDGAME_ASSERT(m_bd.is_legal(mv));
    auto to_play = m_bd.get_to_play();
    m_new_moves[to_play].push_back(mv);
    m_bd.play(mv);
    m_nu_passes = 0;
    if (! m_is_symmetry_broken)
        update_symmetry_broken(mv);
    if (log_simulations)
        log(m_bd);
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
    for (ColorIterator i(m_nu_colors); i; ++i)
        m_playout_features[*i].set_forbidden(info);
    m_playout_features[c].set_forbidden(get_move_info_ext(mv));
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
