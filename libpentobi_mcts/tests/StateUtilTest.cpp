//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/tests/StateUtilTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_mcts/StateUtil.h"

#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;
using namespace libpentobi_mcts;

//-----------------------------------------------------------------------------

namespace {

void play(Board& bd, Color c, const char* s)
{
    Move mv;
    [[maybe_unused]] auto ok = bd.from_string(mv, s);
    LIBBOARDGAME_ASSERT(ok);
    bd.play(c, mv);
}

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(pentobi_mcts_check_symmetry_broken_1)
{
    auto bd = make_unique<Board>(Variant::duo);
    play(*bd, Color(0), "d8,e8,e9,f9,e10");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
    play(*bd, Color(1), "j5,i6,j6,j7,k7");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
    play(*bd, Color(0), "f4,g4,f5,f6,f7");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
    play(*bd, Color(1), "i8,i9,i10,h11,i11");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
    play(*bd, Color(0), "h3,i3,j3,k3");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
    play(*bd, Color(1), "d12,e12,f12,g12");
    LIBBOARDGAME_CHECK(! check_symmetry_broken(*bd))
}

LIBBOARDGAME_TEST_CASE(pentobi_mcts_check_symmetry_broken_2)
{
    auto bd = make_unique<Board>(Variant::duo);
    play(*bd, Color(0), "d8,e8,e9,f9,e10");
    play(*bd, Color(1), "j5,j6,i7,j7,k7");
    LIBBOARDGAME_CHECK(check_symmetry_broken(*bd))
}

LIBBOARDGAME_TEST_CASE(pentobi_mcts_check_symmetry_broken_3)
{
    auto bd = make_unique<Board>(Variant::duo);
    play(*bd, Color(0), "d8,e8,e9,f9,e10");
    play(*bd, Color(1), "j5,i6,j6,j7,k7");
    play(*bd, Color(0), "f4,g4,f5,f6,f7");
    play(*bd, Color(1), "i8,i9,i10,h11,i11");
    play(*bd, Color(0), "d12,e12,f12,g12");
    play(*bd, Color(1), "e12,f12,g12");
    LIBBOARDGAME_CHECK(check_symmetry_broken(*bd))
}

//-----------------------------------------------------------------------------
