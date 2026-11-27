//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/tests/HistoryTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_mcts/History.h"

#include "libboardgame_test/Test.h"

using namespace libpentobi_base;
using namespace libpentobi_mcts;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(pentobi_mcts_history_get_as_setup_with_setup)
{
    auto bd = make_unique<Board>(Variant::duo);
    Setup setup;
    setup.to_play = Color(0);
    Move mv1;
    LIBBOARDGAME_CHECK(bd->from_string(mv1, "a1,a2"));
    setup.placements[Color(0)].push_back(mv1);
    bd->init(&setup);
    Move mv2;
    LIBBOARDGAME_CHECK(bd->from_string(mv2, "e10"));
    bd->play(Color(0), mv2);
    History history;
    history.init(*bd, Color(0));
    Setup merged_setup;
    Variant variant;
    history.get_as_setup(variant, merged_setup);
    auto& placements = merged_setup.placements[Color(0)];
    LIBBOARDGAME_CHECK(placements.size() == 2);
    LIBBOARDGAME_CHECK(placements.contains(mv1));
    LIBBOARDGAME_CHECK(placements.contains(mv2));
}

LIBBOARDGAME_TEST_CASE(pentobi_mcts_history_is_followup_different_setup)
{
    auto bd = make_unique<Board>(Variant::duo);
    Setup setup;
    setup.to_play = Color(0);
    Move mv;
    LIBBOARDGAME_CHECK(bd->from_string(mv, "a1,a2"));
    setup.placements[Color(0)].push_back(mv);
    bd->init(&setup);
    History hist_1;
    hist_1.init(*bd, Color(0));
    LIBBOARDGAME_CHECK(bd->from_string(mv, "b1,b2"));
    setup.placements[Color(1)].push_back(mv);
    bd->init(&setup);
    History hist_2;
    hist_2.init(*bd, Color(0));
    ArrayList<Move, SearchParamConst::max_moves> sequence;
    LIBBOARDGAME_CHECK(! hist_2.is_followup(hist_1, sequence));
}

//-----------------------------------------------------------------------------
