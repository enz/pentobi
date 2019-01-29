//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TreeUtil.h"

#include "NodeUtil.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

const SgfNode* get_move_node(const PentobiTree& tree, const SgfNode& node,
                             unsigned n)
{
    auto move_number = get_move_number(tree, node);
    if (n == move_number)
        return &node;
    if (n < move_number)
    {
        auto current = &node;
        do
        {
            if (tree.has_move(*current))
            {
                if (move_number == n)
                    return current;
                --move_number;
            }
            if (libpentobi_base::has_setup(*current))
                break;
            current = current->get_parent_or_null();
        }
        while (current != nullptr);
    }
    else
    {
        auto current = node.get_first_child_or_null();
        while (current != nullptr)
        {
            if (libpentobi_base::has_setup(*current))
                break;
            if (tree.has_move(*current))
            {
                ++move_number;
                if (move_number == n)
                    return current;
            }
            current = current->get_first_child_or_null();
        }
    }
    return nullptr;
}

unsigned get_move_number(const PentobiTree& tree, const SgfNode& node)
{
    unsigned move_number = 0;
    auto current = &node;
    do
    {
        if (tree.has_move(*current))
            ++move_number;
        if (libpentobi_base::has_setup(*current))
            break;
        current = current->get_parent_or_null();
    }
    while (current != nullptr);
    return move_number;
}

unsigned get_moves_left(const PentobiTree& tree, const SgfNode& node)
{
    unsigned moves_left = 0;
    auto current = node.get_first_child_or_null();
    while (current != nullptr)
    {
        if (libpentobi_base::has_setup(*current))
            break;
        if (tree.has_move(*current))
            ++moves_left;
        current = current->get_first_child_or_null();
    }
    return moves_left;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
