//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonGeometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TRIGON_GEOMETRY_H
#define LIBPENTOBI_BASE_TRIGON_GEOMETRY_H

#include "Geometry.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Geometry as used in the game Blokus Trigon.
    The board is a hexagon consisting of triangles. The coordinates are like
    in this example of a hexagon with edge size 3:
    <tt>
       0 1 2 3 4 5 6 7 8 9 10
    0     / \ / \ / \ / \
    1   / \ / \ / \ / \ / \
    2 / \ / \ / \ / \ / \ / \
    3 \ / \ / \ / \ / \ / \ /
    4   \ / \ / \ / \ / \ /
    5     \ / \ / \ / \ /
    </tt>
    There are two point types: 0=upward triangle, 1=downward triangle. */
class TrigonGeometry final
    : public Geometry
{
public:
    /** Create or reuse an already created geometry with a given size.
        @param sz The edge size of the hexagon. */
    static const TrigonGeometry& get(unsigned sz);


    explicit TrigonGeometry(unsigned sz);

    AdjCoordList get_adj_coord(int x, int y) const override;

    DiagCoordList get_diag_coord(int x, int y) const override;

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    bool init_is_onboard(unsigned x, unsigned y) const override;

private:
    unsigned m_sz;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TRIGON_GEOMETRY_H
