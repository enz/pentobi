//-----------------------------------------------------------------------------
/** @file libpentobi_base/StartingPoints.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_STARTING_POINTS_H
#define LIBPENTOBI_BASE_STARTING_POINTS_H

#include "Color.h"
#include "ColorMap.h"
#include "Geometry.h"
#include "Grid.h"
#include "Variant.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

class StartingPoints
{
public:
    static constexpr unsigned max_starting_points = 16;

    void init(Variant variant, const Geometry& geo);

    bool is_colored_starting_point(Point p) const;

    bool is_colorless_starting_point(Point p) const;

    Color get_starting_point_color(Point p) const;

    const ArrayList<Point,StartingPoints::max_starting_points>&
                                             get_starting_points(Color c) const;

private:
    Grid<bool> m_is_colored_starting_point;

    Grid<bool> m_is_colorless_starting_point;

    Grid<Color> m_starting_point_color;

    ColorMap<ArrayList<Point,max_starting_points>> m_starting_points;

    void add_colored_starting_point(const Geometry& geo, unsigned x,
                                    unsigned y, Color c);

    void add_colorless_starting_point(const Geometry& geo, unsigned x,
                                      unsigned y);
};

inline Color StartingPoints::get_starting_point_color(Point p) const
{
    LIBBOARDGAME_ASSERT(m_is_colored_starting_point[p]);
    return m_starting_point_color[p];
}

inline const ArrayList<Point,StartingPoints::max_starting_points>&
                              StartingPoints::get_starting_points(Color c) const
{
    return m_starting_points[c];
}

inline bool StartingPoints::is_colored_starting_point(Point p) const
{
    return m_is_colored_starting_point[p];
}

inline bool StartingPoints::is_colorless_starting_point(Point p) const
{
    return m_is_colorless_starting_point[p];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_STARTING_POINTS_H
