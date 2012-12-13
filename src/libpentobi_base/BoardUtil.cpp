//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUtil.h"

#include <iostream>
#include "SgfUtil.h"

namespace libpentobi_base {
namespace boardutil {

using namespace std;
using sgf_util::get_color_id;
using sgf_util::get_setup_id;

//-----------------------------------------------------------------------------

void dump(const Board& bd, ostream& out)
{
    Variant variant = bd.get_variant();
    Writer writer(out);
    writer.set_one_prop_per_line(true);
    writer.set_one_prop_value_per_line(true);
    writer.begin_tree();
    writer.begin_node();
    writer.write_property("GM", to_string(variant));
    write_setup(writer, variant, bd.get_setup());
    writer.end_node();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        writer.begin_node();
        ColorMove mv =  bd.get_move(i);
        const char* id = get_color_id(variant, mv.color);
        if (! mv.is_pass())
            writer.write_property(id, bd.to_string(mv.move, false));
        else
            writer.write_property(id, "");
        writer.end_node();
    }
    writer.end_tree();
}

void get_current_position_as_setup(const Board& bd, Setup& setup)
{
    setup = bd.get_setup();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv = bd.get_move(i);
        if (! mv.is_pass())
            setup.placements[mv.color].push_back(mv.move);
    }
    setup.to_play = bd.get_to_play();
}

void write_setup(Writer& writer, Variant variant, const Setup& setup)
{
    const BoardConst& board_const = BoardConst::get(variant);
    for (ColorIterator i(get_nu_colors(variant)); i; ++i)
        if (! setup.placements[*i].empty())
        {
            vector<string> values;
            for (Move mv : setup.placements[*i])
                values.push_back(board_const.to_string(mv, false));
            writer.write_property(get_setup_id(variant, *i), values);
        }
}

//-----------------------------------------------------------------------------

} // namespace boardutil
} // namespace libpentobi_base
