//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libpentobi_base/Board.h"

#include "libboardgame_test/Test.h"
#include "libpentobi_base/MoveList.h"
#include "libpentobi_base/MoveMarker.h"

using namespace std;
using namespace libpentobi_base;

//-----------------------------------------------------------------------------

/** Check some basic functions in a Classic Two-Player game. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_classic_2)
{
    /*
      (
      ;GM[Blokus Two-Player]
      ;1[a20,b20,c20,d20,e20]
      ;2[q20,r20,s20,t20]
      ;3[p1,q1,r1,s1,t1]
      ;4[a1,b1,c1,d1]
      ;1[f19,g19,h19,i19]
      ;2[o19,p19]
      ;3[m1,l2,m2,n2,o2]
      ;4[e2,f2]
      ;1[j18,k18,l18,l19,m19]
      ;2[n20]
      ;3[h2,i2,i3,j3,k3]
      ;4[g1]
      ;1[o17,n18,o18,p18,q18]
      ;3[d2,d3,e3,f3,g3]
      ;1[n13,o13,n14,n15,n16]
      ;3[p3,p4,p5,p6]
      ;1[n10,n11,o11,p11,p12]
      ;3[l4,m4,m5,n5]
      ;1[o7,p7,q7,o8,o9]
      ;3[j5,k5]
      ;1[l6,m6,n6,m7,m8]
      ;3[a3,a4,b4,c4]
      ;1[i6,j6,j7,k7,j8]
      ;3[d5,e5,f5]
      ;1[g6,f7,g7,h7]
      ;3[j1]
      ;1[c6,d6,e6,c7]
      ;1[a8,b8,b9,c9]
      ;1[d10,e10,d11,e11]
      ;1[f9,g9,h9]
      ;1[r4,s4,r5,r6,s6]
      ;1[t7,s8,t8,r9,s9]
      ;1[q13,r13,p14,q14,r14]
      ;1[s16,r17,s17,t17,s18]
      ;1[l9,k10,l10]
      ;1[j11,j12]
      ;1[i10]
      )
    */
    unique_ptr<Board> bd(new Board(Variant::classic_2));
    bd->play(Color(0), bd->from_string("a20,b20,c20,d20,e20"));
    bd->play(Color(1), bd->from_string("q20,r20,s20,t20"));
    bd->play(Color(2), bd->from_string("p1,q1,r1,s1,t1"));
    bd->play(Color(3), bd->from_string("a1,b1,c1,d1"));
    bd->play(Color(0), bd->from_string("f19,g19,h19,i19"));
    bd->play(Color(1), bd->from_string("o19,p19"));
    bd->play(Color(2), bd->from_string("m1,l2,m2,n2,o2"));
    bd->play(Color(3), bd->from_string("e2,f2"));
    bd->play(Color(0), bd->from_string("j18,k18,l18,l19,m19"));
    bd->play(Color(1), bd->from_string("n20"));
    bd->play(Color(2), bd->from_string("h2,i2,i3,j3,k3"));
    bd->play(Color(3), bd->from_string("g1"));
    bd->play(Color(0), bd->from_string("o17,n18,o18,p18,q18"));
    bd->play(Color(2), bd->from_string("d2,d3,e3,f3,g3"));
    bd->play(Color(0), bd->from_string("n13,o13,n14,n15,n16"));
    bd->play(Color(2), bd->from_string("p3,p4,p5,p6"));
    bd->play(Color(0), bd->from_string("n10,n11,o11,p11,p12"));
    bd->play(Color(2), bd->from_string("l4,m4,m5,n5"));
    bd->play(Color(0), bd->from_string("o7,p7,q7,o8,o9"));
    bd->play(Color(2), bd->from_string("j5,k5"));
    bd->play(Color(0), bd->from_string("l6,m6,n6,m7,m8"));
    bd->play(Color(2), bd->from_string("a3,a4,b4,c4"));
    bd->play(Color(0), bd->from_string("i6,j6,j7,k7,j8"));
    bd->play(Color(2), bd->from_string("d5,e5,f5"));
    bd->play(Color(0), bd->from_string("g6,f7,g7,h7"));
    bd->play(Color(2), bd->from_string("j1"));
    bd->play(Color(0), bd->from_string("c6,d6,e6,c7"));
    bd->play(Color(0), bd->from_string("a8,b8,b9,c9"));
    bd->play(Color(0), bd->from_string("d10,e10,d11,e11"));
    bd->play(Color(0), bd->from_string("f9,g9,h9"));
    bd->play(Color(0), bd->from_string("r4,s4,r5,r6,s6"));
    bd->play(Color(0), bd->from_string("t7,s8,t8,r9,s9"));
    bd->play(Color(0), bd->from_string("q13,r13,p14,q14,r14"));
    bd->play(Color(0), bd->from_string("s16,r17,s17,t17,s18"));
    bd->play(Color(0), bd->from_string("l9,k10,l10"));
    bd->play(Color(0), bd->from_string("j11,j12"));
    bd->play(Color(0), bd->from_string("i10"));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 37u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), 89u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), 7u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(2)), 38u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(3)), 7u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(0)), 20u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(1)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(2)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(3)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(0)), 109u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(1)), 7u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(2)), 38u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(3)), 7u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(0)), 133);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(1)), -133);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(2)), 133);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(3)), -133);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(0)), 21u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(1)), 3u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(2)), 10u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(3)), 3u);
    // Make sure that bonus computation still works if after the 1-piece an
    // additional pass move was played
    bd->play_pass(Color(0));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(0)), 20u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(1)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(2)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_bonus(Color(3)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(0)), 109u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(1)), 7u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(2)), 38u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points_with_bonus(Color(3)), 7u);
}

/** Check the number of generated moves at each attach point in a known
    case. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_gen_moves)
{
    unique_ptr<Board> bd(new Board(Variant::classic));
    Color c(0);
    bd->play(c, bd->from_string("i11,j11,k11,l11"));
    unique_ptr<MoveList> moves(new MoveList());
    unique_ptr<MoveMarker> marker(new MoveMarker());
    for (Point p : bd->get_attach_points(c))
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
    unique_ptr<MoveMarker> marker(new MoveMarker());
    bd->gen_moves(Color(0), Point("A20"), *marker, *moves);
    LIBBOARDGAME_CHECK_EQUAL(moves->size(), 58u);
}

/** Test get_place() in a 4-color, 2-player game when the player 1 has
    a higher score but color 1 has less points than color 2. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_get_place)
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
