//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardConstTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/BoardConst.h"

#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;

//-----------------------------------------------------------------------------

/** Test that points in move strings are ordered.
    As specified in doc/blksgf/Pentobi-SGF.html, the order should be
    (a1, b1, ..., a2, b2, ...). There is no restriction on the order when
    parsing move strings in from_string(). */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_const_move_string)
{
    auto& bc = BoardConst::get(Variant::duo);
    Move mv;
    LIBBOARDGAME_CHECK(bc.from_string(mv, "h7,i7,i6,j6,j5"));
    LIBBOARDGAME_CHECK_EQUAL(bc.to_string(mv), "j5,i6,j6,h7,i7");
}

/** Check symmetry information in MoveInfoExt for some moves. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_const_symmetry_info)
{
    auto& bc = BoardConst::get(Variant::trigon_2);
    Move mv;
    LIBBOARDGAME_CHECK(bc.from_string(mv, "q9,q10,r10,q11,r11,s11"));
    auto& info_ext_2 = bc.get_move_info_ext_2(mv);
    LIBBOARDGAME_CHECK(! info_ext_2.breaks_symmetry);
    LIBBOARDGAME_CHECK(bc.from_string(mv, "q8,r8,s8,r9,s9,s10"));
    LIBBOARDGAME_CHECK_EQUAL(info_ext_2.symmetric_move.to_int(), mv.to_int());
}

//-----------------------------------------------------------------------------
