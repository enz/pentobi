//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Book.h"

#include "BoardUtil.h"
#include "libboardgame_base/Log.h"
#include "libboardgame_base/TreeReader.h"

//-----------------------------------------------------------------------------

namespace libpentobi_base {

using libboardgame_base::TreeReader;

//-----------------------------------------------------------------------------

Book::Book(Variant variant)
    : m_tree(variant)
{
    get_transforms(variant, m_transforms, m_inv_transforms);
}

Book::~Book() = default; // Non-inline to avoid GCC -Winline warning

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
        if (node == nullptr)
            return false;
    }
    node = select_child(bd, c, m_tree, *node, inv_transform);
    if (node == nullptr)
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
            LIBBOARDGAME_LOG("WARNING: Book contains nodes without moves");
            continue;
        }
        if (color_mv.color != c)
        {
            LIBBOARDGAME_LOG("WARNING: Book contains non-alternating move sequences");
            continue;
        }
        auto mv = get_transformed(bd, color_mv.move, inv_transform);
        if (! bd.is_legal(color_mv.color, mv))
        {
            LIBBOARDGAME_LOG("WARNING: Book contains illegal move");
            continue;
        }
        if (SgfTree::get_good_move(child) > 0)
        {
            LIBBOARDGAME_LOG(bd.to_string(mv), " !");
            good_moves.push_back(&child);
        }
        else
            LIBBOARDGAME_LOG(bd.to_string(mv));
    }
    if (good_moves.empty())
        return nullptr;
    LIBBOARDGAME_LOG("Book moves: ", good_moves.size());
    auto nu_good_moves = static_cast<unsigned>(good_moves.size());
    return good_moves[m_random.generate() % nu_good_moves];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
