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
    Move mv;
    if (genmove(bd, c, mv,
                PointTransfIdent<Point>(),
                PointTransfIdent<Point>()))
        return mv;
    BoardType board_type = bd.get_board_type();
    if (board_type == BoardType::duo)
        if (genmove(bd, c, mv,
                    PointTransfRot270Refl<Point>(),
                    PointTransfRot270Refl<Point>()))
            return mv;
    if (board_type == BoardType::trigon
        || board_type == BoardType::trigon_3)
    {
        if (genmove(bd, c, mv,
                    PointTransfTrigonRot60<Point>(),
                    PointTransfTrigonRot300<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonRot120<Point>(),
                    PointTransfTrigonRot240<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfRot180<Point>(),
                    PointTransfRot180<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonRot240<Point>(),
                    PointTransfTrigonRot120<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonRot300<Point>(),
                    PointTransfTrigonRot60<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfRefl<Point>(),
                    PointTransfRefl<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonReflRot60<Point>(),
                    PointTransfTrigonReflRot60<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonReflRot120<Point>(),
                    PointTransfTrigonReflRot120<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfReflRot180<Point>(),
                    PointTransfReflRot180<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonReflRot240<Point>(),
                    PointTransfTrigonReflRot240<Point>()))
            return mv;
        if (genmove(bd, c, mv,
                    PointTransfTrigonReflRot300<Point>(),
                    PointTransfTrigonReflRot300<Point>()))
            return mv;
    }
    return Move::null();
}

bool Book::genmove(const Board& bd, Color c, Move& mv,
                   const PointTransform& transform,
                   const PointTransform& inv_transform)
{
    LIBBOARDGAME_ASSERT(! bd.has_setup());
    const libboardgame_sgf::Node* node = &m_tree.get_root();
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove color_mv = bd.get_move(i);
        color_mv.move = get_transformed(bd, color_mv.move, transform);
        node = m_tree.find_child_with_move(*node, color_mv);
        if (node == 0)
            return false;
    }
    node = select_child(bd, c, m_tree, *node, inv_transform);
    if (node == 0)
        return false;
    mv = get_transformed(bd, m_tree.get_move(*node).move, inv_transform);
    return true;
}

Move Book::get_transformed(const Board& bd, Move mv,
                           const PointTransform& transform) const
{
    if (mv.is_pass())
        return mv;
    unsigned width = bd.get_geometry().get_width();
    unsigned height = bd.get_geometry().get_height();
    MovePoints points;
    for (Point p : bd.get_move_info(mv))
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
                               const Node& node,
                               const PointTransform& inv_transform)
{
    unsigned nu_children = node.get_nu_children();
    if (nu_children == 0)
        return 0;
    vector<const Node*> good_moves;
    for (unsigned i = 0; i < nu_children; ++i)
    {
        const Node& child = node.get_child(i);
        ColorMove color_mv = tree.get_move(child);
        if (color_mv.is_null())
        {
            log() << "WARNING: Book contains nodes without moves\n";
            continue;
        }
        if (color_mv.color != c)
        {
            log() << "WARNING: Book contains non-alternating move sequences\n";
            continue;
        }
        Move mv = get_transformed(bd, color_mv.move, inv_transform);
        if (! bd.is_legal(color_mv.color, mv))
        {
            log() << "WARNING: Book contains illegal move\n";
            continue;
        }
        if (m_tree.get_good_move(child) > 0)
        {
            log() << bd.to_string(mv) << " !\n";
            good_moves.push_back(&child);
        }
        else
            log() << bd.to_string(mv) << '\n';
    }
    if (good_moves.empty())
        return 0;
    log() << "Book moves: " << good_moves.size() << '\n';
    unsigned nu_good_moves = static_cast<unsigned>(good_moves.size());
    return good_moves[m_random.generate_small_int(nu_good_moves)];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
