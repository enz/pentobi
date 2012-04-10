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
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;
using libpentobi_base::BoardIterator;
using libpentobi_base::BoardType;
using libpentobi_base::ColorIterator;

//-----------------------------------------------------------------------------

namespace {

void set_piece_considered(const BoardConst& board_const, const char* name,
                          array<bool,Board::max_pieces>& is_piece_considered)
{
    unsigned int index;
    bool found = board_const.get_piece_index_by_name(name, index);
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(found);
    LIBBOARDGAME_ASSERT(found);
    is_piece_considered[index] = true;
}

void set_pieces_considered(const BoardConst& board_const, unsigned int nu_moves,
                           array<bool,Board::max_pieces>& is_piece_considered)
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
            for (unsigned int i = 0; i < board_const.get_nu_pieces(); ++i)
                is_piece_considered[i] = false;
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
            for (unsigned int i = 0; i < board_const.get_nu_pieces(); ++i)
                is_piece_considered[i] = false;
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
        const Piece& piece = board_const.get_piece(i);
        is_piece_considered[i] = (piece.get_size() >= min_piece_size);
    }
}

} // namespace

//-----------------------------------------------------------------------------

Search::Search(GameVariant initial_game_variant, size_t memory)
    : ParentClass(State(initial_game_variant, m_shared_const),
                  memory == 0 ? 384000000 : memory),
      m_auto_param(true),
      m_game_variant(initial_game_variant),
      m_shared_const(m_to_play)
{
    set_rave(true);
    set_unexplored_value(0.5);
    set_expand_threshold(1);
    set_widening_parameter(0);
    set_last_good_reply(true);
    set_default_param(m_game_variant);
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
    return bd.get_move_piece(mv).get_name() + " " + bd.to_string(mv);
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
                BOOST_FOREACH(unsigned int k, bd.get_pieces_left(*i))
                {
                    BOOST_FOREACH(Move mv, bd.get_moves(k, *j, adj_status))
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
    for (unsigned int i = 0; i < Board::max_pieces; ++i)
        m_shared_const.is_piece_considered_all[i] = true;
}

bool Search::search(Move& mv, const Board& bd, Color to_play,
                    ValueType max_count, size_t min_simulations,
                    double max_time, TimeSource& time_source)
{
    m_shared_const.board = &bd;
    m_to_play = to_play;
    GameVariant game_variant = bd.get_game_variant();
    if (m_auto_param && game_variant != m_game_variant)
        set_default_param(game_variant);
    m_game_variant = game_variant;
    bool result = ParentClass::search(mv, max_count, min_simulations, max_time,
                                      time_source, 0);
    return result;
}

void Search::set_default_param(GameVariant game_variant)
{
    log() << "Setting default parameters for " << to_string(game_variant)
          << '\n';
    switch (game_variant)
    {
    case game_variant_duo:
        set_bias_term_constant(0.08f);
        break;
    case game_variant_classic:
        // Not tuned. Use same value as for game_variant_classic_2
    case game_variant_classic_2:
        set_bias_term_constant(0.11f);
        break;
    case game_variant_trigon:
        // Not tuned. Use same value as for game_variant_trigon_2
    case game_variant_trigon_2:
        set_bias_term_constant(0.09f);
        break;
    case game_variant_trigon_3:
        // Not tuned. Use same value as for game_variant_trigon
        set_bias_term_constant(0.09f);
        break;
    default:
        LIBBOARDGAME_ASSERT(false);
        set_bias_term_constant(0.09f);
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
    get_state().write_info(out);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
