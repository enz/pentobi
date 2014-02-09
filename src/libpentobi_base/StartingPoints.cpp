//-----------------------------------------------------------------------------
/** @file libpentobi_base/StartingPoints.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StartingPoints.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

void StartingPoints::add_colored_starting_point(const Geometry& geo,
                                                unsigned x, unsigned y,
                                                Color c)
{
    Point p(x, y, geo.get_width());
    m_is_colored_starting_point[p] = true;
    m_starting_point_color[p] = c;
    m_starting_points[c].push_back(p);
}

void StartingPoints::add_colorless_starting_point(const Geometry& geo,
                                                  unsigned x, unsigned y)
{
    Point p(x, y, geo.get_width());
    m_is_colorless_starting_point[p] = true;
    m_starting_points[Color(0)].push_back(p);
    m_starting_points[Color(1)].push_back(p);
    m_starting_points[Color(2)].push_back(p);
    m_starting_points[Color(3)].push_back(p);
}

void StartingPoints::init(Variant variant, const Geometry& geo)
{
    m_is_colored_starting_point.fill(false, geo);
    m_is_colorless_starting_point.fill(false, geo);
    m_starting_points[Color(0)].clear();
    m_starting_points[Color(1)].clear();
    m_starting_points[Color(2)].clear();
    m_starting_points[Color(3)].clear();
    if (variant == Variant::classic || variant == Variant::classic_2)
    {
        add_colored_starting_point(geo, 0, 19, Color(0));
        add_colored_starting_point(geo, 19, 19, Color(1));
        add_colored_starting_point(geo, 19, 0, Color(2));
        add_colored_starting_point(geo, 0, 0, Color(3));
    }
    else if (variant == Variant::duo || variant == Variant::junior)
    {
        add_colored_starting_point(geo, 4, 9, Color(0));
        add_colored_starting_point(geo, 9, 4, Color(1));
    }
    else if (variant == Variant::trigon || variant == Variant::trigon_2)
    {
        add_colorless_starting_point(geo, 17, 3);
        add_colorless_starting_point(geo, 17, 14);
        add_colorless_starting_point(geo, 9, 6);
        add_colorless_starting_point(geo, 9, 11);
        add_colorless_starting_point(geo, 25, 6);
        add_colorless_starting_point(geo, 25, 11);
    }
    else if (variant == Variant::trigon_3)
    {
        add_colorless_starting_point(geo, 15, 2);
        add_colorless_starting_point(geo, 15, 13);
        add_colorless_starting_point(geo, 7, 5);
        add_colorless_starting_point(geo, 7, 10);
        add_colorless_starting_point(geo, 23, 5);
        add_colorless_starting_point(geo, 23, 10);
    }
    else
        LIBBOARDGAME_ASSERT(false);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
