//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GuiBoardUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_GUI_BOARD_UTIL_H
#define LIBPENTOBI_GUI_GUI_BOARD_UTIL_H

#include "GuiBoard.h"
#include "libpentobi_base/Game.h"

namespace gui_board_util {

using libpentobi_base::Game;

//-----------------------------------------------------------------------------

void setMarkup(GuiBoard& guiBoard, const Game& game,
               unsigned markMovesBegin, unsigned markMovesEnd,
               bool markVariations);

//-----------------------------------------------------------------------------

} // namespace gui_board_util

#endif // LIBPENTOBI_GUI_GUI_BOARD_UTIL_H
