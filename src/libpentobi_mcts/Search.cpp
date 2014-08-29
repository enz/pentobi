//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Search.h"

#include <cstddef>
#include "BoardUtil.h"
#include "Util.h"
#include "libboardgame_util/FmtSaver.h"

namespace libpentobi_mcts {

using libboardgame_base::PointTransfRot180;
using libboardgame_util::FmtSaver;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;
using libpentobi_base::Piece;
using libpentobi_mcts::board_util::check_symmetry_broken;

//-----------------------------------------------------------------------------

namespace {

void filter_min_size(const BoardConst& board_const, unsigned min_size,
                     PieceMap<bool>& is_piece_considered)
{
    for (Piece::IntType i = 0; i < board_const.get_nu_pieces(); ++i)
    {
        Piece piece(i);
        auto& piece_info = board_const.get_piece_info(piece);
        if (piece_info.get_size() < min_size)
            is_piece_considered[piece] = false;
    }
}

void set_piece_considered(const BoardConst& board_const, const char* name,
                          PieceMap<bool>& is_piece_considered,
                          bool is_considered = true)
{
    Piece piece;
    bool found = board_const.get_piece_by_name(name, piece);
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(found);
    LIBBOARDGAME_ASSERT(found);
    is_piece_considered[piece] = is_considered;
}

void set_pieces_considered(const Board& bd, unsigned nu_moves,
                           PieceMap<bool>& is_piece_considered)
{
    auto& board_const = bd.get_board_const();
    auto board_type = board_const.get_board_type();
    unsigned nu_colors = bd.get_nu_colors();
    is_piece_considered.fill(true);
    if (board_type == BoardType::duo)
    {
        if (nu_moves < 2 * nu_colors)
            filter_min_size(board_const, 5, is_piece_considered);
        else if (nu_moves < 3 * nu_colors)
            filter_min_size(board_const, 4, is_piece_considered);
        else if (nu_moves < 5 * nu_colors)
            filter_min_size(board_const, 3, is_piece_considered);
    }
    else if (board_type == BoardType::classic)
    {
        if (nu_moves < 1 * nu_colors)
        {
            is_piece_considered.fill(false);
            set_piece_considered(board_const, "V5", is_piece_considered);
            set_piece_considered(board_const, "Z5", is_piece_considered);
        }
        else if (nu_moves < 3 * nu_colors)
            filter_min_size(board_const, 5, is_piece_considered);
        else if (nu_moves < 5 * nu_colors)
            filter_min_size(board_const, 4, is_piece_considered);
        else if (nu_moves < 7 * nu_colors)
            filter_min_size(board_const, 3, is_piece_considered);
    }
    else if (board_type == BoardType::trigon
             || board_type == BoardType::trigon_3)
    {
        if (nu_moves < 1 * nu_colors)
        {
            is_piece_considered.fill(false);
            set_piece_considered(board_const, "V", is_piece_considered);
            // I5 would also be good but the distance to center pruning in
            // State would prune them anyway
        }
        if (nu_moves < 4 * nu_colors)
        {
            filter_min_size(board_const, 6, is_piece_considered);
            // O is a bad early move, it neither extends, nor blocks well
            set_piece_considered(board_const, "O", is_piece_considered, false);
        }
        else if (nu_moves < 5 * nu_colors)
            filter_min_size(board_const, 5, is_piece_considered);
        else if (nu_moves < 7 * nu_colors)
            filter_min_size(board_const, 4, is_piece_considered);
        else if (nu_moves < 9 * nu_colors)
            filter_min_size(board_const, 3, is_piece_considered);
    }
}

} // namespace

//-----------------------------------------------------------------------------

Search::Search(Variant initial_variant, unsigned nu_threads, size_t memory)
    : ParentClass(nu_threads == 0 ? util::get_nu_threads() : nu_threads,
                  memory == 0 ? util::get_memory() : memory),
      m_auto_param(true),
      m_variant(initial_variant),
      m_shared_const(m_to_play)
{
    set_default_param(m_variant);
    create_threads();
}

Search::~Search()
{
}

bool Search::check_followup(vector<Move>& sequence)
{
    auto& bd = get_board();
    m_state.init(bd, m_to_play);
    bool is_followup = m_state.is_followup(m_last_state, sequence);

    // If avoid_symmetric_draw is enabled, class State uses a different
    // evaluation function depending on which player is to play in the root
    // position (the first player knows about symmetric draws to be able to
    // play a symmetry breaker but the second player pretends not to know about
    // symmetric draws to avoid going for such a draw). In this case, we cannot
    // reuse parts of the old search tree if the computer plays both colors.
    if (m_shared_const.avoid_symmetric_draw
        && is_followup && m_to_play != m_last_state.get_to_play()
        && ! check_symmetry_broken(bd, m_shared_const.symmetric_points))
        is_followup = false;

    m_last_state = m_state;
    return is_followup;
}

unique_ptr<State> Search::create_state()
{
    return unique_ptr<State>(new State(m_variant, m_shared_const));
}

string Search::get_move_string(Move mv) const
{
    return get_board().to_string(mv);
}

void Search::get_root_position(Variant& variant, Setup& setup) const
{
    m_last_state.get_as_setup(variant, setup);
    setup.to_play = m_to_play;
}

Float Search::get_tie_value() const
{
    return 0.5;
}

void Search::on_start_search(bool is_followup)
{
    auto& bd = get_board();
    auto& bc = bd.get_board_const();
    auto nu_colors = bd.get_nu_colors();

    for (ColorIterator i(nu_colors); i; ++i)
    {
        auto& is_forbidden_at_root = m_shared_const.is_forbidden_at_root[*i];
        is_forbidden_at_root.set_all();
        for (BoardIterator j(bd); j; ++j)
            if (! bd.is_forbidden(*j, *i))
            {
                auto adj_status = bd.get_adj_status(*j, *i);
                for (Piece piece : bd.get_pieces_left(*i))
                    for (Move mv : bd.get_moves(piece, *j, adj_status))
                    {
                        if (! is_forbidden_at_root[mv])
                            continue;
                        if (! bd.is_forbidden(*i, mv))
                            is_forbidden_at_root.clear(mv);
                    }
            }
    }

    // Initialize m_shared_const.precomp_moves
    for (ColorIterator i(nu_colors); i; ++i)
    {
        auto& precomp_moves = m_shared_const.precomp_moves[*i];
        precomp_moves.clear();
        // Construct new lists in-place from old if it is a follow-up position
        const auto& old_precomp_moves =
            (is_followup ? precomp_moves : bc.get_precomp_moves());
        for (BoardIterator j(bd); j; ++j)
        {
            if (bd.is_forbidden(*j, *i))
                continue;
            for (unsigned k = 0; k < PrecompMoves::nu_adj_status; ++k)
                // Don't iterate over bd.get_pieces_left(*i) because its
                // ordering is not preserved if a piece is removed and the
                // in-place construction requires that the iteration in these
                // loops is in the same order as during the last construction
                // such that it will never overwrite any old content it still
                // needs to read during the construction.
                for (Piece::IntType l = 0; l < bc.get_nu_pieces(); ++l)
                {
                    Piece piece(l);
                    if (! bd.is_piece_left(*i, piece))
                        continue;
                    auto begin = precomp_moves.get_size();
                    auto moves = old_precomp_moves.get_moves(piece, *j, k);
                    for (auto m = moves.begin(); m != moves.end(); ++m)
                        if (! m_shared_const.is_forbidden_at_root[*i][*m])
                        {
                            if (is_followup)
                                // Assert that we don't overwrite old content
                                // we still need during in-place construction
                                LIBBOARDGAME_ASSERT(
                                    m - precomp_moves.move_lists_begin()
                                    >= static_cast<ptrdiff_t>(
                                                 precomp_moves.get_size()));
                            precomp_moves.push_move(*m);
                        }
                    auto end = precomp_moves.get_size() - begin;
                    precomp_moves.set_list_range(*j, k, piece, begin, end);
                }
        }
    }

    auto& is_piece_considered_list = m_shared_const.is_piece_considered_list;
    is_piece_considered_list.clear();
    for (unsigned i = 0; i < Board::max_game_moves; ++i)
    {
        PieceMap<bool> is_piece_considered;
        set_pieces_considered(bd, i, is_piece_considered);
        bool are_all_considered = true;
        for (Piece::IntType j = 0; j < bc.get_nu_pieces(); ++j)
            if (! is_piece_considered[Piece(j)])
            {
                are_all_considered = false;
                break;
            }
        if (are_all_considered)
        {
            m_shared_const.min_move_all_considered = i;
            break;
        }
        auto pos = find(is_piece_considered_list.begin(),
                        is_piece_considered_list.end(),
                        is_piece_considered);
        if (pos != is_piece_considered_list.end())
            m_shared_const.is_piece_considered[i] = pos;
        else
        {
            is_piece_considered_list.push_back(is_piece_considered);
            m_shared_const.is_piece_considered[i] =
                &is_piece_considered_list.back();
        }
    }
    m_shared_const.is_piece_considered_all.fill(true);

    m_shared_const.symmetric_points.init(bd.get_geometry(),
                                         PointTransfRot180<Point>());
}

bool Search::search(Move& mv, const Board& bd, Color to_play,
                    Float max_count, Float min_simulations,
                    double max_time, TimeSource& time_source)
{
    m_shared_const.board = &bd;
    m_to_play = to_play;
    auto variant = bd.get_variant();
    if (m_auto_param && variant != m_variant)
        set_default_param(variant);
    m_variant = variant;
    bool result = ParentClass::search(mv, max_count, min_simulations, max_time,
                                      time_source);
    return result;
}

void Search::set_default_param(Variant variant)
{
    log("Setting default parameters for ", to_string(variant));
    set_full_select_min(10000);
    set_full_select_interval(20);
    set_expand_threshold(1);
    set_expand_threshold_inc(0.5f);
    set_rave_weight(0.7f);
    set_rave_child_max(2000);
    // The following parameters are currently tuned for duo, classic_2 and
    // trigon_2 and used for all other game variants with the same board type
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        set_bias_term_constant(0.05f);
        set_rave_parent_max(25000);
        break;
    case Variant::classic_2:
    case Variant::classic:
        set_bias_term_constant(0.06f);
        set_rave_parent_max(50000);
        break;
    case Variant::trigon_2:
    case Variant::trigon_3:
    case Variant::trigon:
        set_bias_term_constant(0.06f);
        set_rave_parent_max(50000);
        break;
    default:
        LIBBOARDGAME_ASSERT(false);
    }
}

string Search::get_info() const
{
    if (get_nu_simulations() == 0)
        return string();
    auto& root = get_tree().get_root();
    if (! root.has_children())
        return string();
    ostringstream s;
    s << ParentClass::get_info()
      << "Mov: " << root.get_nu_children() << ", ";
    if (libpentobi_base::get_nu_players(m_variant) > 2)
    {
        s << "All:";
        for (unsigned i = 0; i < libpentobi_base::get_nu_colors(m_variant); ++i)
        {
            if (get_root_val()[i].get_count() == 0)
                s << " -";
            else
                s << " " << setprecision(2) << get_root_val()[i].get_mean();
        }
        s << ", ";
    }
    s << get_state(0).get_info();
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
