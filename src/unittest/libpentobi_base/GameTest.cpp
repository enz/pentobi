//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/GameTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libpentobi_base/Game.h"

#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::get_last_node;
using libpentobi_base::variant_classic;

//-----------------------------------------------------------------------------

/** Test that the current node is in a defined state if the root node contains
    invalid properties. */
LIBBOARDGAME_TEST_CASE(pentobi_base_game_current_defined_invalid_root)
{
    istringstream in("(;GM[Blokus]1[a99999])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Game game(variant_classic);
    try
    {
        game.init(root);
    }
    catch (const Exception&)
    {
        // ignore
    }
    LIBBOARDGAME_CHECK_EQUAL(&game.get_current(), &game.get_tree().get_root());
}

//-----------------------------------------------------------------------------
