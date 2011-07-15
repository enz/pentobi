//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/BookBuilder.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_BOOK_BUILDER_H
#define LIBPENTOBI_MCTS_BOOK_BUILDER_H

#include <boost/filesystem.hpp>
#include "libpentobi_base/GameVariant.h"

namespace libpentobi_mcts {

using boost::filesystem::path;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

void build_book(const path& file, GameVariant game_variant);

void prune_book(const path& file, unsigned int min_count);

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_BOOK_BUILDER_H
