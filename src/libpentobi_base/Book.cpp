//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Book.h"

#include "libboardgame_sgf/MissingProperty.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/BoardUtil.h"

//-----------------------------------------------------------------------------

namespace libpentobi_base {

using namespace std;
using libboardgame_base::PointTransfIdent;
using libboardgame_base::PointTransfRefl;
using libboardgame_base::PointTransfReflRot180;
using libboardgame_base::PointTransfRot180;
using libboardgame_base::PointTransfRot270Refl;
using libboardgame_base::PointTransfTrigonReflRot60;
using libboardgame_base::PointTransfTrigonReflRot120;
using libboardgame_base::PointTransfTrigonReflRot240;
using libboardgame_base::PointTransfTrigonReflRot300;
using libboardgame_base::PointTransfTrigonRot60;
using libboardgame_base::PointTransfTrigonRot120;
using libboardgame_base::PointTransfTrigonRot240;
using libboardgame_base::PointTransfTrigonRot300;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeReader;
using libboardgame_util::log;
using boardutil::get_transformed;

//-----------------------------------------------------------------------------

Book::Book(Variant variant)
    : m_tree(variant)
{
    get_transforms(variant, m_transforms, m_inv_transforms);
}

Book::~Book()
{
}

Move Book::genmove(const Board& bd, Color c)
{
    if (bd.has_setup())
        // Book cannot handle setup positions
        return Move::null();
    Move mv;
    for (unsigned i = 0; i < m_transforms.size(); ++i)
        if (genmove(bd, c, mv, *m_transforms[i], *m_inv_transforms[i]))
            return mv;
    return Move::null();
}

bool Book::genmove(const Board& bd, Color c, Move& mv,
                   const PointTransform& transform,
                   const PointTransform& inv_transform)
{
    LIBBOARDGAME_ASSERT(! bd.has_setup());
    auto node = &m_tree.get_root();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove color_mv = bd.get_move(i);
        color_mv.move = get_transformed(bd, color_mv.move, transform);
        node = m_tree.find_child_with_move(*node, color_mv);
        if (! node)
            return false;
    }
    node = select_child(bd, c, m_tree, *node, inv_transform);
    if (! node)
        return false;
    mv = get_transformed(bd, m_tree.get_move(*node).move, inv_transform);
    return true;
}

void Book::load(istream& in)
{
    TreeReader reader;
    try
    {
        reader.read(in);
    }
    catch (const TreeReader::ReadError& e)
    {
        throw runtime_error(string("could not read book: ") + e.what());
    }
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    m_tree.init(root);
    get_transforms(m_tree.get_variant(), m_transforms, m_inv_transforms);
}

const SgfNode* Book::select_child(const Board& bd, Color c,
                                  const PentobiTree& tree, const SgfNode& node,
                                  const PointTransform& inv_transform)
{
    unsigned nu_children = node.get_nu_children();
    if (nu_children == 0)
        return nullptr;
    vector<const SgfNode*> good_moves;
    for (unsigned i = 0; i < nu_children; ++i)
    {
        auto& child = node.get_child(i);
        ColorMove color_mv = tree.get_move(child);
        if (color_mv.is_null())
        {
            log("WARNING: Book contains nodes without moves");
            continue;
        }
        if (color_mv.color != c)
        {
            log("WARNING: Book contains non-alternating move sequences");
            continue;
        }
        auto mv = get_transformed(bd, color_mv.move, inv_transform);
        if (! bd.is_legal(color_mv.color, mv))
        {
            log("WARNING: Book contains illegal move");
            continue;
        }
        if (m_tree.get_good_move(child) > 0)
        {
            log(bd.to_string(mv), " !");
            good_moves.push_back(&child);
        }
        else
            log(bd.to_string(mv));
    }
    if (good_moves.empty())
        return nullptr;
    log("Book moves: ", good_moves.size());
    unsigned nu_good_moves = static_cast<unsigned>(good_moves.size());
    return good_moves[m_random.generate() % nu_good_moves];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
