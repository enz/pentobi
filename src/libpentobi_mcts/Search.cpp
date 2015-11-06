//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Search.h"

#include "Util.h"

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

Search::Search(Variant initial_variant, unsigned nu_threads, size_t memory)
    : SearchBase(nu_threads == 0 ? util::get_nu_threads() : nu_threads,
                 memory),
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

bool Search::check_followup(ArrayList<Move, max_moves>& sequence)
{
    auto& bd = get_board();
    m_history.init(bd, m_to_play);
    bool is_followup = m_history.is_followup(m_last_history, sequence);

    // If avoid_symmetric_draw is enabled, class State uses a different
    // evaluation function depending on which player is to play in the root
    // position (the first player knows about symmetric draws to be able to
    // play a symmetry breaker but the second player pretends not to know about
    // symmetric draws to avoid going for such a draw). In this case, we cannot
    // reuse parts of the old search tree if the computer plays both colors.
    if (m_shared_const.avoid_symmetric_draw
            && is_followup && m_to_play != m_last_history.get_to_play()
            && ! check_symmetry_broken(bd, m_shared_const.symmetric_points))
        is_followup = false;

    m_last_history = m_history;
    return is_followup;
}

unique_ptr<State> Search::create_state()
{
    return unique_ptr<State>(new State(m_variant, m_shared_const));
}

void Search::get_root_position(Variant& variant, Setup& setup) const
{
    m_last_history.get_as_setup(variant, setup);
    setup.to_play = m_to_play;
}

void Search::on_start_search(bool is_followup)
{
    m_shared_const.init(is_followup);
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
    bool result = SearchBase::search(mv, max_count, min_simulations, max_time,
                                      time_source);
    return result;
}

void Search::set_default_param(Variant variant)
{
    LIBBOARDGAME_LOG("Setting default parameters for ", to_string(variant));
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
        set_bias_term_constant(0.04f);
        set_rave_parent_max(25000);
        break;
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::classic:
        set_bias_term_constant(0.05f);
        set_rave_parent_max(50000);
        break;
    case Variant::trigon_2:
    case Variant::trigon_3:
    case Variant::trigon:
        set_bias_term_constant(0.05f);
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
    s << SearchBase::get_info()
      << "Mov: " << root.get_nu_children() << ", ";
    if (libpentobi_base::get_nu_players(m_variant) > 2)
    {
        s << "All:";
        for (PlayerInt i = 0; i < libpentobi_base::get_nu_colors(m_variant);
             ++i)
        {
            if (get_root_val(i).get_count() == 0)
                s << " -";
            else
                s << " " << setprecision(2) << get_root_val(i).get_mean();
        }
        s << ", ";
    }
    s << get_state(0).get_info();
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
