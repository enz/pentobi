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

//-----------------------------------------------------------------------------

Search::Search(const Board& bd)
    : ParentClass(State(bd, m_shared_const)),
      m_shared_const(bd, m_to_play)
{
    set_rave(true);
    set_unexplored_value(0.5);
    set_expand_threshold(1);
    set_bias_term_constant(0.1f);
    set_widening_parameter(0);
    set_last_good_reply(true);
}

Search::~Search() throw()
{
}

string Search::get_move_string(Move mv) const
{
    const Board& bd = get_board();
    return bd.get_move_piece(mv).get_name() + " " + bd.to_string(mv);
}

bool Search::search(Move& mv, Color to_play, ValueType max_count,
                    size_t min_simulations, double max_time,
                    TimeSource& time_source)
{
    m_to_play = to_play;
    bool result = ParentClass::search(mv, max_count, min_simulations, max_time,
                                      time_source, 0);
    return result;
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
