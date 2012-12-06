//-----------------------------------------------------------------------------
/** @file libpentobi_base/Book.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Book.h"

#include "libboardgame_sgf/MissingProperty.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::PointTransfIdent;
using libboardgame_base::PointTransfRot270Refl;
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeReader;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

Book::Book(Variant variant)
    : m_tree(variant)
{
}

Move Book::genmove(const Board& bd, Color c)
{
    if (bd.has_setup())
        // Book cannot handle setup positions
        return Move::null();
    Move mv = genmove(bd, c, PointTransfIdent<Point>(),
                      PointTransfIdent<Point>());
    if (! mv.is_null())
        return mv;
    Variant variant = bd.get_variant();
    if (variant == Variant::duo || variant == Variant::junior)
        mv = genmove(bd, c, PointTransfRot270Refl<Point>(),
                     PointTransfRot270Refl<Point>());
    return mv;
}

Move Book::genmove(const Board& bd, Color c,
                   const PointTransform<Point>& transform,
                   const PointTransform<Point>& inv_transform)
{
    LIBBOARDGAME_ASSERT(! bd.has_setup());
    const libboardgame_sgf::Node* node = &m_tree.get_root();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv = bd.get_move(i);
        mv.move = get_transformed(bd, mv.move, transform);
        node = m_tree.find_child_with_move(*node, mv);
        if (node == 0)
            return Move::null();
    }
    node = select_child(bd, c, m_tree, *node);
    if (node == 0)
        return Move::null();
    return get_transformed(bd, m_tree.get_move(*node).move, inv_transform);
}

Move Book::get_transformed(const Board& bd, Move mv,
                           const PointTransform<Point>& transform) const
{
    if (mv.is_pass())
        return mv;
    unsigned width = bd.get_geometry().get_width();
    unsigned height = bd.get_geometry().get_height();
    MovePoints points;
    BOOST_FOREACH(Point p, bd.get_move_points(mv))
        points.push_back(transform.get_transformed(p, width, height));
    Move transformed_mv;
    bd.find_move(points, transformed_mv);
    return transformed_mv;
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
        throw Exception(format("could not read book: %1%") % e.what());
    }
    unique_ptr<libboardgame_sgf::Node> root
        = reader.get_tree_transfer_ownership();
    m_tree.init(root);
}

const Node* Book::select_child(const Board& bd, Color c, const Tree& tree,
                               const Node& node)
{
    unsigned nu_children = node.get_nu_children();
    if (nu_children == 0)
        return 0;
    vector<const Node*> good_moves;
    for (unsigned i = 0; i < nu_children; ++i)
    {
        const Node& child = node.get_child(i);
        ColorMove mv = tree.get_move(child);
        if (mv.is_null())
        {
            log() << "WARNING: Book contains nodes without moves\n";
            continue;
        }
        if (mv.color != c)
        {
            log() << "WARNING: Book contains non-alternating move sequences\n";
            continue;
        }
        if (! bd.is_legal(mv.color, mv.move))
        {
            log() << "WARNING: Book contains illegal move\n";
            continue;
        }
        if (m_tree.get_good_move(child) > 0)
        {
            log() << bd.to_string(mv.move) << " !\n";
            good_moves.push_back(&child);
        }
        else
            log() << bd.to_string(mv.move) << '\n';
    }
    if (good_moves.empty())
        return 0;
    log() << "Book moves: " << good_moves.size() << '\n';
    unsigned nu_good_moves = static_cast<unsigned>(good_moves.size());
    return good_moves[m_random.generate_small_int(nu_good_moves)];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
