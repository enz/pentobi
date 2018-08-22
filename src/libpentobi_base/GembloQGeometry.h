//-----------------------------------------------------------------------------
/** @file libpentobi_base/GembloQGeometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GEMBLOQ_GEOMETRY_H
#define LIBPENTOBI_BASE_GEMBLOQ_GEOMETRY_H

#include "Geometry.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Geometry for the board game GembloQ.
    Each square on the board consists of four triangles, each half-square of
    two triangles. The coordinates are like this:
    <tt>
       0 1 2 3 4 5 6 7 8
    0 | / | \ | / | \ | /
    1 | \ | / | \ | / | \
    2 | / | \ | / | \ | /
    3 | \ | / | \ | / | \
    </tt>
    The point types are determined by the location of the right angle of the
    triangle: 0: top/left, 1=down/right, 2=down/left, 3=up/right. */
class GembloQGeometry final
    : public Geometry
{
public:
    /** Create or reuse an already created geometry.
        @param nu_players The number of players (2, 3, or 4). */
    static const GembloQGeometry& get(unsigned nu_players);


    explicit GembloQGeometry(unsigned nu_players);

    AdjCoordList get_adj_coord(int x, int y) const override;

    DiagCoordList get_diag_coord(int x, int y) const override;

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    bool init_is_onboard(unsigned x, unsigned y) const override;

private:
    unsigned m_edge;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GEMBLOQ_GEOMETRY_H
