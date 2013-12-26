//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/State.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_STATE_H
#define LIBPENTOBI_MCTS_STATE_H

#include "Float.h"
#include "LocalValue.h"
#include "libboardgame_mcts/PlayerMove.h"
#include "libboardgame_mcts/Tree.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/Unused.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/MoveList.h"
#include "libpentobi_base/MoveMarker.h"
#include "libpentobi_base/PieceMap.h"
#include "libpentobi_base/PointList.h"
#include "libpentobi_base/SymmetricPoints.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::PlayerInt;
using libboardgame_mcts::PlayerMove;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;
using libboardgame_util::Statistics;
using libboardgame_util::StatisticsBase;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::ColorMove;
using libpentobi_base::Variant;
using libpentobi_base::Grid;
using libpentobi_base::Marker;
using libpentobi_base::Move;
using libpentobi_base::MoveInfo;
using libpentobi_base::MoveInfoExt;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::PieceMap;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::SymmetricPoints;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;

//-----------------------------------------------------------------------------

/** Constant data shared between the search states. */
struct SharedConst
{
    /** Like BoardConst::m_moves_range but for SharedConst::move_lists.
        Only elements for pieces still available and non-forbidden points
        are initialized. */
    ColorMap<Grid<array<PieceMap<BoardConst::ListIndex>,
                        BoardConst::nu_adj_status>>>
        moves_range;

    /** Like BoardConst::m_move_lists but moves that are forbidden at the
        root position filtered out. */
    ColorMap<array<Move,BoardConst::max_move_lists_sum_length>> move_lists;

    /** The game board.
        Contains the current position. */
    const Board* board;

    /** The color to play at the root of the search. */
    const Color& to_play;

    bool detect_symmetry;

    bool avoid_symmetric_draw;

    /** Maximum value to modify the win/loss result by the score. */
    Float score_modification;

    /** Lookup table for symmetric points (only used in Duo). */
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
    array<const PieceMap<bool>*,Board::max_game_moves> is_piece_considered;

    /** List of unique values for is_piece_considered. */
    ArrayList<PieceMap<bool>,Board::max_game_moves> is_piece_considered_list;

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
    class. */
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

    ~State() throw();

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

    /** Generate and play a playout move.
        @return @c false if end of game was reached, and no move was played */
    bool gen_and_play_playout_move(Move lgr1, Move lgr2);

    array<Float,4> evaluate_playout();

    array<Float,4> evaluate_terminal();

    /** Get number of moves in the current simulation. */
    unsigned get_nu_moves() const;

    /** Get move in the current simulation. */
    PlayerMove<Move> get_move(unsigned n) const;

    /** Do not update RAVE values for n'th move of the current simulation. */
    bool skip_rave(Move mv) const;

    void dump(ostream& out) const;

    void write_info(ostream& out) const;

private:
    struct MoveFeatures
    {
        /** Heuristic value of the move expressed in score points. */
        Float heuristic;

        /** Only used on Classic and Trigon boards. */
        unsigned dist_to_center;

        /** Does the move touch a piece of the same player? */
        bool connect;
    };

    static const bool log_simulations = false;

    bool m_has_connect_move;

    unsigned m_nu_moves_initial;

    Color::IntType m_nu_passes;

    unsigned m_max_local_value;

    unsigned m_max_playable_piece_size;

    unsigned m_max_playable_piece_size_local;

    /** Maximum of Features::heuristic for all moves. */
    Float m_max_heuristic;

    unsigned m_min_dist_to_center;

    const SharedConst& m_shared_const;

    Board m_bd;

    const BoardConst* m_bc;

    Color::IntType m_nu_colors;

    const MoveInfo* m_move_info_array;

    const MoveInfoExt* m_move_info_ext_array;

    /** Incrementally updated lists of legal moves for both colors.
        Only the move list for the color to play van be used in any given
        position, the other color is not updated immediately after a move. */
    ColorMap<unique_ptr<MoveList>> m_moves;

    /** Temporary variable reused for efficiency. */
    unique_ptr<MoveList> m_tmp_moves;

    ColorMap<const PieceMap<bool>*> m_is_piece_considered;

    array<MoveFeatures, Move::range> m_features;

    /** Moves played by a color since the last update of its move list. */
    ColorMap<ArrayList<Move, Board::max_nonpass_player_moves>> m_new_moves;

    ColorMap<bool> m_is_move_list_initialized;

    ColorMap<bool> m_has_moves;

    /** Moves that are a local response to the last move.
        These moves occupy at least one of the corner points of the last
        piece played. */
    MoveList m_local_moves;

    /** Marks moves contained in m_moves. */
    ColorMap<MoveMarker> m_marker;

    LocalValue m_local_value;

    RandomGenerator m_random;

    /** Precomputed State::m_score_modification / BoardConst::max_score. */
    Float m_score_modification_factor;

    /** Number of simulations of this state in the current search. */
    size_t m_nu_simulations;

    size_t m_nu_playout_moves;

    size_t m_nu_last_good_reply_moves;

    bool m_check_symmetric_draw;

    bool m_check_terminate_early;

    bool m_is_symmetry_broken;

    /** Enforce all pieces to be considered for the rest of the simulation. */
    bool m_force_consider_all_pieces;

    /** Minimum number of pieces on board to perform a symmetry check.
        3 in Duo/Junior or 5 in Trigon because this is the earliest move number
        to break the symmetry. The early playout termination that evaluates all
        symmetric positions as a draw should not be used earlier because it can
        case bad move selection in very short searches if all moves are
        evaluated as draw and the search is not deep enough to find that the
        symmetry can be broken a few moves later. */
    unsigned m_symmetry_min_nu_pieces;

    /** Remember attach points that were already used for move generation.
        Allows the incremental update of the move lists to skip attach points
        of newly played pieces that were already attach points of previously
        played pieces. */
    ColorMap<Marker> m_moves_added_at;

    /** Distance to center heuristic. */
    Grid<unsigned> m_dist_to_center;

    void add_moves(Point p, Color c,
                   const Board::PiecesLeftList& pieces_considered);

    void add_moves(Point p, Color c, Piece piece, unsigned adj_status);

    void add_starting_moves(Color c,
                            const Board::PiecesLeftList& pieces_considered);

    void compute_features(bool check_dist_to_center, bool check_connect);

    Point find_best_starting_point(Color c) const;

    bool gen_playout_move(Move lgr1, Move lgr2, Move& result);

    /** Equivalent to but faster than m_bd.get_move_info() */
    const MoveInfo& get_move_info(Move move) const;

    /** Equivalent to but faster than m_bd.get_move_info_ext() */
    const MoveInfoExt& get_move_info_ext(Move move) const;

    BoardConst::LocalMovesListRange get_moves(Color c, Piece piece, Point p,
                                              unsigned adj_status) const;

    const PieceMap<bool>& get_pieces_considered() const;

    void init_moves_with_local(Color c);

    void init_moves_without_local(Color c);

    void play_playout(Move mv);

    bool check_move(const Grid<bool>& is_forbidden, Move mv,
                    const MoveInfo& info);

    bool check_move_without_local(const Grid<bool>& is_forbidden, Move mv);

    void update_moves(Color c);

    bool check_symmetry_broken();

    void update_symmetry_broken(Move mv);
};

inline void State::finish_in_tree()
{
    if (log_simulations)
        log() << "Finish in-tree\n";
    if (m_check_symmetric_draw)
        m_is_symmetry_broken = check_symmetry_broken();
    else
        // Pretending that the symmetry is always broken is equivalent to
        // ignoring symmetric draws
        m_is_symmetry_broken = true;
}

inline PlayerMove<Move> State::get_move(unsigned n) const
{
    auto mv = m_bd.get_move(m_nu_moves_initial + n);
    return PlayerMove<Move>(mv.color.to_int(), mv.move);
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

inline BoardConst::LocalMovesListRange State::get_moves(Color c, Piece piece,
                                           Point p, unsigned adj_status) const
{
    BoardConst::ListIndex idx =
        m_shared_const.moves_range[c][p][adj_status][piece];
    auto begin = &m_shared_const.move_lists[c][idx.begin];
    auto end = begin + idx.size;
    return BoardConst::LocalMovesListRange(begin, end);
}

inline unsigned State::get_nu_moves() const
{
    LIBBOARDGAME_ASSERT(m_bd.get_nu_moves() >= m_nu_moves_initial);
    return m_bd.get_nu_moves() - m_nu_moves_initial;
}

inline PlayerInt State::get_to_play() const
{
    return m_bd.get_to_play().to_int();
}

inline void State::play_in_tree(Move mv)
{
    Color to_play = m_bd.get_to_play();
    if (! mv.is_pass())
    {
        LIBBOARDGAME_ASSERT(m_bd.is_legal(to_play, mv));
        m_bd.play_nonpass(to_play, mv);
        m_nu_passes = 0;
    }
    else
    {
        m_bd.play_pass(to_play);
        ++m_nu_passes;
    }
    if (log_simulations)
        log() << m_bd;
}

inline void State::start_playout()
{
}

inline bool State::skip_rave(Move mv) const
{
    LIBBOARDGAME_UNUSED(mv);
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_STATE_H
