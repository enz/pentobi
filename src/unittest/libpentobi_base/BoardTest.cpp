//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libpentobi_base/Board.h"

#include <boost/foreach.hpp>
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;

//-----------------------------------------------------------------------------

/** Check the number of generated moves at each attach point in a known
    case. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_gen_moves)
{
    Board bd(variant_classic);
    Color c(0);
    bd.play(c, bd.from_string("i11,j11,k11,l11"));
    MoveList moves;
    MoveMarker marker;
    BOOST_FOREACH(Point p, bd.get_attach_points(c))
    {
        bd.gen_moves(c, p, marker, moves);
        LIBBOARDGAME_CHECK_EQUAL(moves.size(), 124u);
        marker.clear_all_set_known(moves);
        moves.clear();
    }
}

/** Check the number of generated moves at a starting point. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_gen_moves_2)
{
    Board bd(variant_classic);
    MoveList moves;
    MoveMarker marker;
    bd.gen_moves(Color(0), Point("A20"), marker, moves);
    LIBBOARDGAME_CHECK_EQUAL(moves.size(), 58u);
}

//-----------------------------------------------------------------------------
