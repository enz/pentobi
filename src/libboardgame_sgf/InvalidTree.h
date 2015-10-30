//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidTree.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_INVALID_TREE_H
#define LIBBOARDGAME_SGF_INVALID_TREE_H

#include <stdexcept>

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

/** Exception indication a semantic error in the tree.
    This exception is used for semantic errors in SGF trees. If a SGF tree
    is loaded from an external file, it is usually only checked for
    (game-independent) syntax errors, but not for semantic errors (e.g. illegal
    moves) because that would be too expensive when loading large trees and
    not allow the user to partially use a tree if there is an error only in
    some variations. As a consequence, functions that use the tree may cause
    errors later (e.g. when trying to update the game state to a node in the
    tree). In this case, they should throw InvalidTree. */
class InvalidTree
    : public runtime_error
{
    using runtime_error::runtime_error;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_INVALID_TREE_H
