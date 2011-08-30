//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceValueHeuristic.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceValueHeuristic.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

PieceValueHeuristic::PieceValueHeuristic(const Board& bd)
{
    set(bd, "X",  1.00f);
    set(bd, "F",  0.95f);
    set(bd, "W",  0.95f);
    set(bd, "N",  0.90f);
    set(bd, "T5", 0.90f);
    set(bd, "U",  0.90f);
    set(bd, "Y",  0.90f);
    set(bd, "Z5", 0.90f);
    set(bd, "P",  0.85f);
    set(bd, "L5", 0.80f);
    set(bd, "V5", 0.80f);
    set(bd, "I5", 0.75f);
    set(bd, "T4", 0.70f);
    set(bd, "Z4", 0.70f);
    set(bd, "L4", 0.65f);
    set(bd, "O",  0.60f);
    set(bd, "I4", 0.55f);
    set(bd, "V3", 0.50f);
    set(bd, "I3", 0.45f);
    set(bd, "2",  0.40f);
    set(bd, "1",  0.35f);
    LIBBOARDGAME_ASSERT(BoardConst::nu_pieces == 21);
}

void PieceValueHeuristic::set(const Board& bd, const string& name, float value)
{
    unsigned int i;
    if (! bd.get_piece_index_by_name(name, i))
        LIBBOARDGAME_ASSERT(false);
    m_value[i] = value;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
