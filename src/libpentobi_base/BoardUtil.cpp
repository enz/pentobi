//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUtil.h"

#include <iostream>
#include <boost/foreach.hpp>

namespace libpentobi_base {
namespace boardutil {

using namespace std;

//-----------------------------------------------------------------------------

void dump(const Board& bd, ostream& out)
{
    GameVariant game_variant = bd.get_game_variant();
    out << bd
        << "(\n;GM[" << to_string(game_variant) << "]\n";
    const Setup& setup = bd.get_setup();
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        if (! setup.placements[*i].empty())
        {
            out << " A";
            if (game_variant == game_variant_duo)
                out << ((*i).to_int() == 0 ? 'B' : 'W');
            else
                out << ((*i).to_int() + 1);
            bool is_first = true;
            BOOST_FOREACH(Move mv, setup.placements[*i])
            {
                if (! is_first)
                    out << "   ";
                is_first = false;
                out << '[' << bd.to_string(mv, false) << "]\n";
            }
        }
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv =  bd.get_move(i);
        Color c = mv.color;
        out << ';';
        if (game_variant == game_variant_duo)
            out << (c.to_int() == 0 ? 'B' : 'W');
        else
            out << (c.to_int() + 1);
        out << '[';
        if (! mv.is_pass())
            out << bd.to_string(mv.move, false);
        out << "]\n";
    }
    out << ")\n";
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
