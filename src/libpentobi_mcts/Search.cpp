//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Search.h"

#include <boost/format.hpp>

namespace libpentobi_mcts {

using boost::format;
using libpentobi_base::board_type_classic;
using libpentobi_base::board_type_duo;
using libpentobi_base::board_type_trigon;
using libpentobi_base::board_type_trigon_3;
using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

namespace {

void set_piece_considered(const BoardConst& board_const, const char* name,
                          PieceMap<bool>& is_piece_considered)
{
    Piece piece;
    bool found = board_const.get_piece_by_name(name, piece);
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(found);
    LIBBOARDGAME_ASSERT(found);
    is_piece_considered[piece] = true;
}

void set_pieces_considered(const BoardConst& board_const, unsigned int nu_moves,
                           PieceMap<bool>& is_piece_considered)
{
    BoardType board_type = board_const.get_board_type();
    unsigned int min_piece_size = 0;
    if (board_type == board_type_duo)
    {
        if (nu_moves < 4)
            min_piece_size = 5;
        else if (nu_moves < 6)
            min_piece_size = 4;
        else if (nu_moves < 10)
            min_piece_size = 3;
    }
    else if (board_type == board_type_classic)
    {
        if (nu_moves < 4)
        {
            is_piece_considered.fill(false);
            set_piece_considered(board_const, "V5", is_piece_considered);
            set_piece_considered(board_const, "Z5", is_piece_considered);
            return;
        }
        if (nu_moves < 12)
            min_piece_size = 5;
        else if (nu_moves < 20)
            min_piece_size = 4;
        else if (nu_moves < 30)
            min_piece_size = 3;
    }
    else if (board_type == board_type_trigon
             || board_type == board_type_trigon_3)
    {
        if (nu_moves < 4)
        {
            is_piece_considered.fill(false);
            set_piece_considered(board_const, "V", is_piece_considered);
            return;
        }
        if (nu_moves < 16)
            min_piece_size = 6;
        else if (nu_moves < 20)
            min_piece_size = 5;
        else if (nu_moves < 28)
            min_piece_size = 4;
        else if (nu_moves < 36)
            min_piece_size = 3;
    }
    for (unsigned int i = 0; i < board_const.get_nu_pieces(); ++i)
    {
        Piece piece(i);
        const PieceInfo& piece_info = board_const.get_piece_info(piece);
        is_piece_considered[piece] = (piece_info.get_size() >= min_piece_size);
    }
}

} // namespace

//-----------------------------------------------------------------------------

Search::Search(Variant initial_variant, size_t memory)
    : ParentClass(State(initial_variant, m_shared_const),
                  memory == 0 ? 384000000 : memory),
      m_auto_param(true),
      m_variant(initial_variant),
      m_shared_const(m_to_play)
{
    set_rave(true);
    set_unexplored_value(0.5);
    set_expand_threshold(1);
    set_widening_parameter(0);
    set_last_good_reply(true);
    set_default_param(m_variant);
}

Search::~Search() throw()
{
}

bool Search::check_followup(vector<Move>& sequence)
{
    m_state.init(get_board(), m_to_play);
    bool is_followup = m_state.is_followup(m_last_state, sequence);
    m_last_state = m_state;
    return is_followup;
}

string Search::get_move_string(Move mv) const
{
    const Board& bd = get_board();
    const MoveInfo& move_info = bd.get_move_info(mv);
    const PieceInfo& piece_info = bd.get_piece_info(move_info.piece);
    return piece_info.get_name() + " " + bd.to_string(mv);
}

void Search::get_root_position(Variant& variant, Setup& setup) const
{
    m_last_state.get_as_setup(variant, setup);
    setup.to_play = m_to_play;
}

void Search::on_start_search()
{
    const Board& bd = get_board();
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
    {
        MoveMarker& is_forbidden_at_root =
            m_shared_const.is_forbidden_at_root[*i];
        is_forbidden_at_root.set_all();
        for (BoardIterator j(bd); j; ++j)
            if (! bd.is_forbidden(*j, *i))
            {
                unsigned int adj_status = bd.get_adj_status_index(*j, *i);
                BOOST_FOREACH(Piece piece, bd.get_pieces_left(*i))
                {
                    BOOST_FOREACH(Move mv, bd.get_moves(piece, *j, adj_status))
                    {
                        if (! is_forbidden_at_root[mv])
                            continue;
                        if (! bd.is_forbidden(*i, mv))
                            is_forbidden_at_root.clear(mv);
                    }
                }
            }
    }
    for (unsigned int i = 0; i < Board::max_game_moves; ++i)
        set_pieces_considered(bd.get_board_const(), i,
                              m_shared_const.is_piece_considered[i]);
    m_shared_const.is_piece_considered_all.fill(true);
}

bool Search::search(Move& mv, const Board& bd, Color to_play,
                    ValueType max_count, size_t min_simulations,
                    double max_time, TimeSource& time_source)
{
    m_shared_const.board = &bd;
    m_to_play = to_play;
    Variant variant = bd.get_variant();
    if (m_auto_param && variant != m_variant)
        set_default_param(variant);
    m_variant = variant;
    bool result = ParentClass::search(mv, max_count, min_simulations, max_time,
                                      time_source, 0);
    return result;
}

void Search::set_default_param(Variant variant)
{
    log() << "Setting default parameters for " << to_string(variant)
          << '\n';
    switch (variant)
    {
    case variant_duo:
        set_bias_term_constant(0.09f);
        break;
    case variant_junior:
        set_bias_term_constant(0.12f);
        break;
    case variant_classic:
        // Not tuned. Use same value as for variant_classic_2
        set_bias_term_constant(0.11f);
        break;
    case variant_classic_2:
        set_bias_term_constant(0.11f);
        break;
    case variant_trigon:
        // Not tuned. Use same value as for variant_trigon_2
        set_bias_term_constant(0.10f);
        break;
    case variant_trigon_2:
        set_bias_term_constant(0.10f);
        break;
    case variant_trigon_3:
        // Not tuned. Use same value as for variant_trigon_2
        set_bias_term_constant(0.10f);
        break;
    default:
        LIBBOARDGAME_ASSERT(false);
        set_bias_term_constant(0.10f);
        break;
    }
}

void Search::write_info(ostream& out) const
{
    if (get_nu_simulations() == 0)
        return;
    const Node& root = get_tree().get_root();
    if (! root.has_children())
        return;
    ParentClass::write_info(out);
    out << (format("Mov: %i, ") % root.get_nu_children());
    if (libpentobi_base::get_nu_players(m_variant) > 2)
    {
        out << "All:";
        BOOST_FOREACH(const StatisticsBase& i, get_root_eval())
            out << (format(" %.2f") % i.get_mean());
        out << ", ";
    }
    get_state().write_info(out);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
