//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/SharedConst.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SharedConst.h"

namespace libpentobi_mcts {

using libboardgame_base::PointTransfRot180;
using libpentobi_base::BoardConst;
using libpentobi_base::BoardType;
using libpentobi_base::Move;
using libpentobi_base::Point;
using libpentobi_base::Piece;

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

/** Check if an adjacent status is a possible follow-up status for another
    one. */
inline bool is_followup_adj_status(unsigned status_new, unsigned status_old)
{
    return (status_new & status_old) == status_old;
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
        if (nu_moves < nu_colors)
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
        if (nu_moves < nu_colors)
        {
            is_piece_considered.fill(false);
            set_piece_considered(board_const, "V", is_piece_considered);
            set_piece_considered(board_const, "I6", is_piece_considered);
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

SharedConst::SharedConst(const Color& to_play)
    : board(nullptr),
      to_play(to_play),
      avoid_symmetric_draw(true)
{
}

void SharedConst::init(bool is_followup)
{
    auto& bd = *board;
    auto& bc = bd.get_board_const();
    for (Color c : bd.get_colors())
    {
        auto& is_forbidden_at_root = this->is_forbidden_at_root[c];
        is_forbidden_at_root.set();
        for (Point p : bd)
            if (! bd.is_forbidden(p, c))
            {
                auto adj_status = bd.get_adj_status(p, c);
                for (Piece piece : bd.get_pieces_left(c))
                    for (Move mv : bd.get_moves(piece, p, adj_status))
                    {
                        if (! is_forbidden_at_root[mv])
                            continue;
                        if (! bd.is_forbidden(c, mv))
                            is_forbidden_at_root.clear(mv);
                    }
            }
    }

    // Initialize m_shared_const.precomp_moves
    for (Color c : bd.get_colors())
    {
        auto& precomp_moves = this->precomp_moves[c];
        precomp_moves.clear();
        // Construct new lists in-place from old if it is a follow-up position
        const auto& old_precomp_moves =
            (is_followup ? precomp_moves : bc.get_precomp_moves());
        for (Point p : bd)
        {
            if (bd.is_forbidden(p, c))
                continue;
            auto adj_status = bd.get_adj_status(p, c);
            for (unsigned i = 0; i < PrecompMoves::nu_adj_status; ++i)
            {
                if (is_followup && ! is_followup_adj_status(i, adj_status))
                    continue;
                // Don't iterate over bd.get_pieces_left(*i) because its
                // ordering is not preserved if a piece is removed and the
                // in-place construction requires that the iteration in these
                // loops is in the same order as during the last construction
                // such that it will never overwrite any old content it still
                // needs to read during the construction.
                for (Piece::IntType j = 0; j < bc.get_nu_pieces(); ++j)
                {
                    Piece piece(j);
                    if (! bd.is_piece_left(c, piece))
                        continue;
                    auto begin = precomp_moves.get_size();
                    auto moves = old_precomp_moves.get_moves(piece, p, i);
                    for (auto& mv : moves)
                        if (! is_forbidden_at_root[c][mv])
                        {
                            if (is_followup)
                                // Assert that we don't overwrite old content
                                // we still need during in-place construction
                                LIBBOARDGAME_ASSERT(
                                    &mv - precomp_moves.move_lists_begin()
                                            >= static_cast<ptrdiff_t>(
                                                precomp_moves.get_size()));
                            precomp_moves.push_move(mv);
                        }
                    auto size = precomp_moves.get_size() - begin;
                    precomp_moves.set_list_range(p, i, piece, begin, size);
                }
            }
        }
    }

    if (! is_followup)
        init_pieces_considered();

    symmetric_points.init(bd.get_geometry(), PointTransfRot180<Point>());
}

void SharedConst::init_pieces_considered()
{
    auto& bd = *board;
    auto& bc = bd.get_board_const();
    is_piece_considered_list.clear();
    for (auto i = bd.get_nu_onboard_pieces(); i < Board::max_game_moves; ++i)
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
            min_move_all_considered = i;
            break;
        }
        auto pos = find(is_piece_considered_list.begin(),
                        is_piece_considered_list.end(),
                        is_piece_considered);
        if (pos != is_piece_considered_list.end())
            this->is_piece_considered[i] = &(*pos);
        else
        {
            is_piece_considered_list.push_back(is_piece_considered);
            this->is_piece_considered[i] = &is_piece_considered_list.back();
        }
    }
    is_piece_considered_all.fill(true);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
