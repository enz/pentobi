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
    : SearchBase(nu_threads == 0 ? get_nu_threads() : nu_threads, memory),
      m_variant(initial_variant),
      m_shared_const(m_to_play)
{
    set_default_param(m_variant);
    create_threads();
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
            && has_central_symmetry(bd.get_variant())
            && ! check_symmetry_broken(bd))
        is_followup = false;

    m_last_history = m_history;
    return is_followup;
}

unique_ptr<State> Search::create_state()
{
    return make_unique<State>(m_variant, m_shared_const);
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
                    Float max_count, size_t min_simulations,
                    double max_time, TimeSource& time_source)
{
    m_shared_const.board = &bd;
    m_to_play = to_play;
    auto variant = bd.get_variant();
    if (variant != m_variant)
        set_default_param(variant);
    m_variant = variant;
    bool result = SearchBase::search(mv, max_count, min_simulations, max_time,
                                      time_source);
    // Search doesn't generate all useless one-piece moves in Callisto
    if (result && mv.is_null() && bd.get_piece_set() == PieceSet::callisto
            && bd.is_piece_left(to_play, bd.get_one_piece()))
    {
        for (Point p : bd)
            if (! bd.is_forbidden(p, to_play) && ! bd.is_center_section(p))
            {
                auto moves = bd.get_board_const().get_moves(bd.get_one_piece(),
                                                            p, 0);
                LIBBOARDGAME_ASSERT(moves.size() == 1);
                mv = *moves.begin();
                result = true;
                break;
            }
    }
    return result;
}

void Search::set_default_param(Variant variant)
{
    LIBBOARDGAME_LOG("Setting default parameters for ", to_string(variant));
    set_rave_weight(0.7f);
    set_rave_child_max(2000);
    // The following parameters are currently tuned for duo, classic_2 and
    // trigon_2 and used for all other game variants with the same board type
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::gembloq:
    case Variant::gembloq_2_4:
    case Variant::gembloq_3:
        // Tuned for classic_2
        set_exploration_constant(4.5f);
        set_rave_parent_max(50000);
        break;
    case Variant::duo:
    case Variant::junior:
    case Variant::gembloq_2:
        // Tuned for duo
        set_exploration_constant(4.0f);
        set_rave_parent_max(25000);
        break;
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
    case Variant::callisto:
    case Variant::callisto_2_4:
    case Variant::callisto_3:
        // Tuned for trigon_2
        set_exploration_constant(6.0f);
        set_rave_parent_max(50000);
        break;
    case Variant::nexos:
    case Variant::nexos_2:
        // Tuned for nexos_2
        set_exploration_constant(3.7f);
        set_rave_parent_max(50000);
        break;
    case Variant::callisto_2:
        set_exploration_constant(4.0f);
        set_rave_parent_max(25000);
        break;
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
