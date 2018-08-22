//-----------------------------------------------------------------------------
/** @file libpentobi_base/NexosGeometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_NEXOS_GEOMETRY_H
#define LIBPENTOBI_BASE_NEXOS_GEOMETRY_H

#include "Geometry.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Geometry as used in the game Nexos.
    The points of the board are horizontal or vertical segments and junctions.
    Junctions only need to be included in piece definitions if they are
    necessary to indicate that the opponent cannot cross the junction
    (i.e. if exactly two segments of the piece with the same orientation
    connect to the junction).
    The coordinates are like:
    <tt>
      0 1 2 3 4 5 6 ...
    0 + - + - + - +
    1 |   |   |   |
    2 + - + - + - +
    3 |   |   |   |
    4 + - + - + - +
    </tt>
    There are four point types: 0=junction, 1=horizontal segment, 2=vertical
    segment, 3=hole surrounded by segments.
    To fit with the generalizations used in the Blokus engine, points have no
    adjacent points, and points are diagonal to each other if they are segments
    that connect to the same junction. */
class NexosGeometry final
    : public Geometry
{
public:
    /** Create or reuse an already created geometry. */
    static const NexosGeometry& get();


    AdjCoordList get_adj_coord(int x, int y) const override;

    DiagCoordList get_diag_coord(int x, int y) const override;

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    bool init_is_onboard(unsigned x, unsigned y) const override;

private:
    NexosGeometry();
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_NEXOS_GEOMETRY_H
