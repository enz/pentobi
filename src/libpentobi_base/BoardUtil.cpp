//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUtil.h"

#include "PentobiSgfUtil.h"
#ifdef LIBBOARDGAME_DEBUG
#include <sstream>
#endif

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

#ifdef LIBBOARDGAME_DEBUG
string dump(const Board& bd)
{
    ostringstream s;
    auto variant = bd.get_variant();
    Writer writer(s);
    writer.begin_tree();
    writer.begin_node();
    writer.write_property("GM", to_string(variant));
    write_setup(writer, variant, bd.get_setup());
    writer.end_node();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        writer.begin_node();
        auto mv = bd.get_move(i);
        auto id = get_color_id(variant, mv.color);
        if (! mv.is_null())
            writer.write_property(id, bd.to_string(mv.move, false));
        writer.end_node();
    }
    writer.end_tree();
    return s.str();
}
#endif

void get_current_position_as_setup(const Board& bd, Setup& setup)
{
    setup = bd.get_setup();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        auto mv = bd.get_move(i);
        setup.placements[mv.color].push_back(mv.move);
    }
    setup.to_play = bd.get_to_play();
}

Move get_transformed(const Board& bd, Move mv,
                     const PointTransform<Point>& transform)
{
    auto& geo = bd.get_geometry();
    MovePoints points;
    for (auto p : bd.get_move_points(mv))
        points.push_back(transform.get_transformed(p, geo));
    Move transformed_mv;
    bd.find_move(points, bd.get_move_piece(mv), transformed_mv);
    return transformed_mv;
}

void write_setup(Writer& writer, Variant variant, const Setup& setup)
{
    auto& board_const = BoardConst::get(variant);
    for (Color c : get_colors(variant))
    {
        auto& placements = setup.placements[c];
        if (placements.empty())
            continue;
        vector<string> values;
        values.reserve(placements.size());
        for (Move mv : placements)
            values.push_back(board_const.to_string(mv, false));
        writer.write_property(get_setup_id(variant, c), values);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
