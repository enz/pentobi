//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/GameTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/Game.h"

#include "libboardgame_base/TreeReader.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_base::TreeReader;

//-----------------------------------------------------------------------------

/** Test that the current node is in a defined state if the root node contains
    invalid properties. */
LIBBOARDGAME_TEST_CASE(pentobi_base_game_current_defined_invalid_root)
{
    istringstream in("(;GM[Blokus]1[a99999])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    Game game(Variant::classic);
    try
    {
        game.init(root);
    }
    catch (const runtime_error&)
    {
        // ignore
    }
    LIBBOARDGAME_CHECK_EQUAL(&game.get_current(), &game.get_root());
}

//-----------------------------------------------------------------------------
