//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoardUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoardUtil.h"

#include "libboardgame_util/Log.h"
#include "libboardgame_util/StringUtil.h"

namespace gui_board_util {

using libpentobi_base::ColorMove;
using libpentobi_base::Tree;
using libboardgame_sgf::Node;
using libboardgame_sgf::ChildIterator;
using libboardgame_util::log;
using libboardgame_util::Exception;
using libboardgame_util::string_util::get_letter_coord;

//-----------------------------------------------------------------------------

namespace {

void appendMoveAnnotation(QString& label, const Game& game, const Node& node)
{
    const Tree& tree = game.get_tree();
    double goodMove = tree.get_good_move(node);
    if (goodMove > 1)
    {
        label.append("!!");
        return;
    }
    if (goodMove > 0)
    {
        label.append("!");
        return;
    }
    double badMove = tree.get_bad_move(node);
    if (badMove > 1)
    {
        label.append("??");
        return;
    }
    if (badMove > 0)
    {
        label.append("?");
        return;
    }
    if (tree.is_interesting_move(node))
    {
        label.append("!?");
        return;
    }
    if (tree.is_doubtful_move(node))
    {
        label.append("?!");
        return;
    }
}

/** Get the index of a variation.
    This ignores child nodes without moves so that the moves are still labeled
    1a, 1b, 1c, etc. even if this does not correspond to the child node
    index. (Note that this is a different convention from variation strings
    which does not use move number and child move index, but node depth and
    child node index) */
bool getVariationIndex(const Tree& tree, const Node& node,
                       unsigned int& moveIndex)
{
    const Node* parent = node.get_parent_or_null();
    if (parent == 0 || parent->has_single_child())
        return false;
    unsigned int nuSiblingMoves = 0;
    moveIndex = 0;
    for (ChildIterator i(*parent); i; ++i)
    {
        if (! tree.has_move(*i))
            continue;
        if (&(*i) == &node)
            moveIndex = nuSiblingMoves;
        ++nuSiblingMoves;
    }
    if (nuSiblingMoves == 1)
        return false;
    return true;
}

void setMoveLabel(GuiBoard& guiBoard, const Game& game, const Node& node,
                  unsigned int moveNumber, ColorMove mv, bool markVariations)
{
    if (! mv.is_regular())
        return;
    const Board& bd = game.get_board();
    Point p = bd.get_move_info_ext(mv.move).center;
    QString label;
    label.setNum(moveNumber);
    if (markVariations)
    {
        unsigned int moveIndex;
        if (getVariationIndex(game.get_tree(), node, moveIndex))
            label.append(get_letter_coord(moveIndex).c_str());
    }
    appendMoveAnnotation(label, game, node);
    guiBoard.setLabel(p, label);
}

} // namespace

//-----------------------------------------------------------------------------

void setMarkup(GuiBoard& guiBoard, const Game& game,
               unsigned int markMovesBegin, unsigned int markMovesEnd,
               bool markVariations)
{
    guiBoard.clearMarkup();
    if (markMovesBegin > 0)
    {
        const Tree& tree = game.get_tree();
        const Board& bd = game.get_board();
        unsigned int displayedMoveNumber = 0; // pass moves have no number
        const Node* node = &game.get_current();
        do
        {
            ColorMove mv = tree.get_move_ignore_invalid(*node);
            if (mv.is_regular())
                ++displayedMoveNumber;
            node = node->get_parent_or_null();
        }
        while (node != 0);
        unsigned int moveNumber = bd.get_nu_moves();
        node = &game.get_current();
        do
        {
            ColorMove mv = tree.get_move_ignore_invalid(*node);
            if (! mv.is_null())
            {
                if (! mv.move.is_pass())
                {
                    if (moveNumber >= markMovesBegin
                        && moveNumber <= markMovesEnd)
                        setMoveLabel(guiBoard, game, *node, moveNumber, mv,
                                     markVariations);
                    --displayedMoveNumber;
                }
                --moveNumber;
            }
            node = node->get_parent_or_null();
        }
        while (node != 0);
    }
}

//-----------------------------------------------------------------------------

} // namespace gui_board_util
