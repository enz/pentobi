//----------------------------------------------------------------------------
/** @file libpentobi_mcts/History.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#include "History.h"

#include "libpentobi_base/BoardUtil.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::get_current_position_as_setup;

//----------------------------------------------------------------------------

void History::get_as_setup(Variant& variant, Setup& setup) const
{
    LIBBOARDGAME_ASSERT(is_valid());
    variant = m_variant;
    auto bd = make_unique<Board>(variant);
    for (ColorMove mv : m_moves)
        bd->play(mv);
    get_current_position_as_setup(*bd, setup);
}

void History::init(const Board& bd, Color to_play)
{
    m_is_valid = true;
    m_variant = bd.get_variant();
    m_nu_colors = bd.get_nu_colors();
    m_moves.clear();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
        m_moves.push_back(bd.get_move(i));
    m_to_play = to_play;
}

bool History::is_followup(
        const History& other,
        ArrayList<Move, SearchParamConst::max_moves>& sequence) const
{
    if (! m_is_valid || ! other.m_is_valid || m_variant != other.m_variant
            || m_moves.size() < other.m_moves.size())
        return false;
    unsigned i = 0;
    for ( ; i < other.m_moves.size(); ++i)
        if (m_moves[i] != other.m_moves[i])
            return false;
    sequence.clear();
    Color to_play = other.m_to_play;
    for ( ; i < m_moves.size(); ++i)
    {
        auto mv = m_moves[i];
        while (mv.color != to_play)
        {
            sequence.push_back(Move::null());
            to_play = to_play.get_next(m_nu_colors);
        }
        sequence.push_back(mv.move);
        to_play = to_play.get_next(m_nu_colors);
    }
    while (m_to_play != to_play)
    {
        sequence.push_back(Move::null());
        to_play = to_play.get_next(m_nu_colors);
    }
    return true;
}

//----------------------------------------------------------------------------

} // namespace libpentobi_mcts
