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
    unique_ptr<Board> bd(new Board(Variant::classic));
    Color c(0);
    bd->play(c, bd->from_string("i11,j11,k11,l11"));
    unique_ptr<MoveList> moves(new MoveList());
    unique_ptr<MoveMarker> marker(new MoveMarker());;
    BOOST_FOREACH(Point p, bd->get_attach_points(c))
    {
        bd->gen_moves(c, p, *marker, *moves);
        LIBBOARDGAME_CHECK_EQUAL(moves->size(), 124u);
        marker->clear_all_set_known(*moves);
        moves->clear();
    }
}

/** Check the number of generated moves at a starting point. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_gen_moves_2)
{
    unique_ptr<Board> bd(new Board(Variant::classic));
    unique_ptr<MoveList> moves(new MoveList());
    unique_ptr<MoveMarker> marker(new MoveMarker());;
    bd->gen_moves(Color(0), Point("A20"), *marker, *moves);
    LIBBOARDGAME_CHECK_EQUAL(moves->size(), 58u);
}

/** Test get_place() in a 4-color, 2-player game when the player 1 has
    a higher score but color 1 has less points than color 2. */
LIBBOARDGAME_TEST_CASE(pentobi_base_get_place)
{
    unique_ptr<Board> bd(new Board(Variant::classic_2));
    bd->play(Color(0), bd->from_string("a20,b20"));
    bd->play(Color(1), bd->from_string("r20,s20,t20"));
    bd->play(Color(2), bd->from_string("q1,r1,s1,t1"));
    bd->play(Color(3), bd->from_string("a1,b1"));
    // Not a final position but Board::get_place() should not care about that
    unsigned place;
    bool isPlaceShared;
    bd->get_place(Color(0), place, isPlaceShared);
    LIBBOARDGAME_CHECK_EQUAL(place, 0u);
    LIBBOARDGAME_CHECK(! isPlaceShared);
    bd->get_place(Color(1), place, isPlaceShared);
    LIBBOARDGAME_CHECK_EQUAL(place, 1u);
    LIBBOARDGAME_CHECK(! isPlaceShared);
    bd->get_place(Color(2), place, isPlaceShared);
    LIBBOARDGAME_CHECK_EQUAL(place, 0u);
    LIBBOARDGAME_CHECK(! isPlaceShared);
    bd->get_place(Color(3), place, isPlaceShared);
    LIBBOARDGAME_CHECK_EQUAL(place, 1u);
    LIBBOARDGAME_CHECK(! isPlaceShared);
}

//-----------------------------------------------------------------------------
