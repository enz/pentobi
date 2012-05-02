//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoardUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GuiBoardUtil.h"

#include "libboardgame_util/Log.h"

namespace gui_board_util {

using libpentobi_base::ColorMove;
using libpentobi_base::Tree;
using libboardgame_sgf::Node;
using libboardgame_sgf::ChildIterator;
using libboardgame_util::log;
using libboardgame_util::Exception;

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

void setMoveLabel(GuiBoard& guiBoard, const Game& game, const Node& node,
                  unsigned int moveNumber, ColorMove mv)
{
    if (! mv.is_regular())
        return;
    const Board& bd = game.get_board();
    Point p = bd.get_move_info_ext(mv.move).center;
    QString label;
    label.setNum(moveNumber);
    appendMoveAnnotation(label, game, node);
    guiBoard.setLabel(p, label);
}

void setVariationMarkup(GuiBoard& guiBoard, const Node& node, Point p)
{
    const Node* parent = node.get_parent_or_null();
    if (parent == 0 || parent->get_nu_children() < 2)
        return;
    guiBoard.setMarkupFlag(p, markup_variation);
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
                    {
                        setMoveLabel(guiBoard, game, *node, moveNumber, mv);
                        if (markVariations)
                            setVariationMarkup(guiBoard, *node,
                                          bd.get_move_info_ext(mv.move).center);
                    }
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
