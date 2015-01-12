//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TreeUtil.h"

#include "NodeUtil.h"

namespace libpentobi_base {
namespace tree_util {

//-----------------------------------------------------------------------------

unsigned get_move_number(const PentobiTree& tree, const SgfNode& node)
{
    unsigned move_number = 0;
    auto current = &node;
    while (current)
    {
        if (! tree.get_move_ignore_invalid(*current).is_null())
            ++move_number;
        if (libpentobi_base::node_util::has_setup(*current))
            break;
        current = current->get_parent_or_null();
    }
    return move_number;
}

unsigned get_moves_left(const PentobiTree& tree, const SgfNode& node)
{
    unsigned moves_left = 0;
    auto current = node.get_first_child_or_null();
    while (current)
    {
        if (libpentobi_base::node_util::has_setup(*current))
            break;
        if (! tree.get_move_ignore_invalid(*current).is_null())
            ++moves_left;
        current = current->get_first_child_or_null();
    }
    return moves_left;
}

//-----------------------------------------------------------------------------

} // namespace tree_util
} // namespace libpentobi_base
