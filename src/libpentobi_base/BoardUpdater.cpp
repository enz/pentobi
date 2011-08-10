//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUpdater.h"

#include "libboardgame_sgf/Util.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_sgf::util::get_path_from_root;

//-----------------------------------------------------------------------------

void BoardUpdater::update(const Node& node)
{
    LIBBOARDGAME_ASSERT(m_tree.contains(node));
    m_bd.init();
    get_path_from_root(node, m_path);
    BOOST_FOREACH(const Node* i, m_path)
    {
        ColorMove mv = m_tree.get_move(*i);
        if (! mv.is_null())
            m_bd.play(mv);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
