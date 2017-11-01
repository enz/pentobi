//-----------------------------------------------------------------------------
/** @file libpentobi_base/NodeUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_NODE_UTIL_H
#define LIBPENTOBI_BASE_NODE_UTIL_H

#include "Color.h"
#include "MovePoints.h"
#include "Variant.h"
#include "libboardgame_sgf/SgfNode.h"

namespace libpentobi_base {

using libboardgame_sgf::SgfNode;

//-----------------------------------------------------------------------------

/** Get move points.
    @param node
    @param variant
    @param[out] c The move color (only defined if return value is true)
    @param[out] points The move points (only defined if return value is
    true)
    @return true if the node has a move property. */
bool get_move(const SgfNode& node, Variant variant, Color& c,
              MovePoints& points);

bool has_move(const SgfNode& node, Variant variant);

/** Check if a node has setup properties (not including the PL property). */
bool has_setup(const SgfNode& node);

/** Get the color to play in a setup position (PL property). */
bool get_player(const SgfNode& node, Color::IntType nu_colors, Color& c);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_NODE_UTIL_H
