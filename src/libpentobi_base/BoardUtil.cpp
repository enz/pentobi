//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUtil.h"

#include <iostream>

namespace libpentobi_base {
namespace boardutil {

using namespace std;

//-----------------------------------------------------------------------------

void dump(const Board& bd, ostream& out)
{
    out << bd;
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv =  bd.get_move(i);
        out << (i + 1) << " play " << mv.color << ' '
            << bd.to_string(mv.move) << '\n';
    }
}

void get_current_position_as_setup(const Board& bd, Setup& setup)
{
    setup = bd.get_setup();
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv = bd.get_move(i);
        if (! mv.is_pass())
            setup.placements[mv.color].push_back(mv.move);
    }
    setup.to_play = bd.get_to_play();
}

//-----------------------------------------------------------------------------

} // namespace boardutil
} // namespace libpentobi_base
