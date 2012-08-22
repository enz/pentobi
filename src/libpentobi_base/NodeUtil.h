//-----------------------------------------------------------------------------
/** @file libpentobi_base/NodeUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_NODE_UTIL_H
#define LIBPENTOBI_BASE_NODE_UTIL_H

#include "Color.h"
#include "MovePoints.h"
#include "Variant.h"
#include "libboardgame_sgf/Node.h"

namespace libpentobi_base {
namespace node_util {

using libboardgame_sgf::Node;

//-----------------------------------------------------------------------------

/** Get move points.
    @param node
    @param variant
    @param[out] c The move color (only defined if return value is true)
    @param[out] points The move points (only defined if return value is
    true)
    @return true if the node has a move property and the move is not a pass
    move. */
bool get_move(const Node& node, Variant variant, Color& c, MovePoints& points);

/** Check if a node has setup properties (not including the PL property). */
bool has_setup(const Node& node);

/** Get the color to play in a setup position (PL property). */
bool get_player(const Node& node, Color& c);

//-----------------------------------------------------------------------------

} // namespace node_util
} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_NODE_UTIL_H
