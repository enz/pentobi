//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoardUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoardUtil.h"

#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_util/StringUtil.h"

namespace gui_board_util {

using libpentobi_base::ColorMove;
using libpentobi_base::PentobiTree;
using libboardgame_sgf::SgfNode;
using libboardgame_sgf::util::is_main_variation;
using libboardgame_sgf::util::get_move_annotation;
using libboardgame_util::get_letter_coord;

//-----------------------------------------------------------------------------

namespace {

/** Get the index of a variation.
    This ignores child nodes without moves so that the moves are still labeled
    1a, 1b, 1c, etc. even if this does not correspond to the child node
    index. (Note that this is a different convention from variation strings
    which does not use move number and child move index, but node depth and
    child node index) */
bool getVariationIndex(const PentobiTree& tree, const SgfNode& node,
                       unsigned& moveIndex)
{
    auto parent = node.get_parent_or_null();
    if (! parent || parent->has_single_child())
        return false;
    unsigned nuSiblingMoves = 0;
    moveIndex = 0;
    for (auto& i : parent->get_children())
    {
        if (! tree.has_move(i))
            continue;
        if (&i == &node)
            moveIndex = nuSiblingMoves;
        ++nuSiblingMoves;
    }
    if (nuSiblingMoves == 1)
        return false;
    return true;
}

void markMove(GuiBoard& guiBoard, const Game& game, const SgfNode& node,
              unsigned moveNumber, ColorMove mv, bool markVariations,
              bool markWithDot)
{
    if (mv.is_null())
        return;
    auto& bd = game.get_board();
    Point p = bd.get_move_info_ext_2(mv.move).label_pos;
    if (markWithDot)
    {
        if (markVariations && ! is_main_variation(game.get_current()))
            guiBoard.setMark(p, BoardPainter::circle);
        else
            guiBoard.setMark(p, BoardPainter::dot);
        return;
    }
    QString label;
    label.setNum(moveNumber);
    if (markVariations)
    {
        unsigned moveIndex;
        if (getVariationIndex(game.get_tree(), node, moveIndex))
            label.append(get_letter_coord(moveIndex).c_str());
    }
    label.append(get_move_annotation(game.get_tree(), node));
    guiBoard.setLabel(p, label);
}

} // namespace

//-----------------------------------------------------------------------------

void setMarkup(GuiBoard& guiBoard, const Game& game, unsigned markMovesBegin,
               unsigned markMovesEnd, bool markVariations, bool markWithDot)
{
    guiBoard.clearMarkup();
    if (markMovesBegin == 0)
        return;
    auto& tree = game.get_tree();
    auto& bd = game.get_board();
    unsigned moveNumber = bd.get_nu_moves();
    auto node = &game.get_current();
    do
    {
        auto mv = tree.get_move_ignore_invalid(*node);
        if (! mv.is_null())
        {
            if (moveNumber >= markMovesBegin && moveNumber <= markMovesEnd)
                markMove(guiBoard, game, *node, moveNumber, mv, markVariations,
                         markWithDot);
            --moveNumber;
        }
        node = node->get_parent_or_null();
    }
    while (node);
}

//-----------------------------------------------------------------------------

} // namespace gui_board_util
