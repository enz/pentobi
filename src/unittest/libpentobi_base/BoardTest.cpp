//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/Board.h"

#include "libboardgame_test/Test.h"
#include "libpentobi_base/MoveMarker.h"

using namespace std;
using namespace libpentobi_base;

//-----------------------------------------------------------------------------

namespace {

void play(Board& bd, Color c, const char* s)
{
    Move mv;
    auto ok = bd.from_string(mv, s);
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(ok);
    LIBBOARDGAME_ASSERT(ok);
    bd.play(c, mv);
}

} // namespace

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
    auto bd = make_unique<Board>(Variant::classic_2);
    play(*bd, Color(0), "a20,b20,c20,d20,e20");
    play(*bd, Color(1), "q20,r20,s20,t20");
    play(*bd, Color(2), "p1,q1,r1,s1,t1");
    play(*bd, Color(3), "a1,b1,c1,d1");
    play(*bd, Color(0), "f19,g19,h19,i19");
    play(*bd, Color(1), "o19,p19");
    play(*bd, Color(2), "m1,l2,m2,n2,o2");
    play(*bd, Color(3), "e2,f2");
    play(*bd, Color(0), "j18,k18,l18,l19,m19");
    play(*bd, Color(1), "n20");
    play(*bd, Color(2), "h2,i2,i3,j3,k3");
    play(*bd, Color(3), "g1");
    play(*bd, Color(0), "o17,n18,o18,p18,q18");
    play(*bd, Color(2), "d2,d3,e3,f3,g3");
    play(*bd, Color(0), "n13,o13,n14,n15,n16");
    play(*bd, Color(2), "p3,p4,p5,p6");
    play(*bd, Color(0), "n10,n11,o11,p11,p12");
    play(*bd, Color(2), "l4,m4,m5,n5");
    play(*bd, Color(0), "o7,p7,q7,o8,o9");
    play(*bd, Color(2), "j5,k5");
    play(*bd, Color(0), "l6,m6,n6,m7,m8");
    play(*bd, Color(2), "a3,a4,b4,c4");
    play(*bd, Color(0), "i6,j6,j7,k7,j8");
    play(*bd, Color(2), "d5,e5,f5");
    play(*bd, Color(0), "g6,f7,g7,h7");
    play(*bd, Color(2), "j1");
    play(*bd, Color(0), "c6,d6,e6,c7");
    play(*bd, Color(0), "a8,b8,b9,c9");
    play(*bd, Color(0), "d10,e10,d11,e11");
    play(*bd, Color(0), "f9,g9,h9");
    play(*bd, Color(0), "r4,s4,r5,r6,s6");
    play(*bd, Color(0), "t7,s8,t8,r9,s9");
    play(*bd, Color(0), "q13,r13,p14,q14,r14");
    play(*bd, Color(0), "s16,r17,s17,t17,s18");
    play(*bd, Color(0), "l9,k10,l10");
    play(*bd, Color(0), "j11,j12");
    play(*bd, Color(0), "i10");
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 37u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(109));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(7));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(2)), ScoreType(38));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(3)), ScoreType(7));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(0)), ScoreType(133));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(1)), ScoreType(-133));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(2)), ScoreType(133));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_score(Color(3)), ScoreType(-133));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(0)), 21u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(1)), 3u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(2)), 10u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_onboard_pieces(Color(3)), 3u);
}

LIBBOARDGAME_TEST_CASE(pentobi_base_board_gen_moves_classic_initial)
{
    auto bd = make_unique<Board>(Variant::classic);
    auto moves = make_unique<MoveList>();
    auto marker = make_unique<MoveMarker>();
    bd->gen_moves(Color(0), *marker, *moves);
    LIBBOARDGAME_CHECK_EQUAL(moves->size(), 58u);
}

/** Test get_place() in a 4-color, 2-player game when the player 1 has
    a higher score but color 1 has less points than color 2. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_get_place)
{
    auto bd = make_unique<Board>(Variant::classic_2);
    play(*bd, Color(0), "a20,b20");
    play(*bd, Color(1), "r20,s20,t20");
    play(*bd, Color(2), "q1,r1,s1,t1");
    play(*bd, Color(3), "a1,b1");
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
