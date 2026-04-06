//-----------------------------------------------------------------------------
/** @file libpentobi_base/tests/BoardConstTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/BoardConst.h"

#include "libboardgame_test/Test.h"

using namespace libpentobi_base;

//-----------------------------------------------------------------------------

/** Test that from_string() handles null moves.
    Used for example in pentobi/AnalyzeGameMode.cpp */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_const_from_string_null)
{
    auto& bc = BoardConst::get(Variant::duo);
    Move mv;
    LIBBOARDGAME_CHECK(bc.from_string(mv, "null"))
    LIBBOARDGAME_CHECK(mv.is_null())
}

/** Test that points in move strings are ordered.
    As specified in doc/blksgf/Pentobi-SGF.html, the order should be
    (a1, b1, ..., a2, b2, ...). There is no restriction on the order when
    parsing move strings in from_string(). */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_const_to_string_ordered)
{
    auto& bc = BoardConst::get(Variant::duo);
    Move mv;
    LIBBOARDGAME_CHECK(bc.from_string(mv, "h7,i7,i6,j6,j5"))
    LIBBOARDGAME_CHECK_EQUAL(bc.to_string(mv), "j5,i6,j6,h7,i7")
}

//-----------------------------------------------------------------------------
