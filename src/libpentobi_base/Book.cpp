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
    : m_tree(variant),
      m_is_computer_generated(true)
{
}

Move Book::genmove(const Board& bd, Color c, double delta, double max_delta)
{
    if (bd.has_setup())
        // Book dcannot handle setup positions
        return Move::null();
    Move mv = genmove(bd, c, delta, max_delta,
                      PointTransfIdent<Point>(), PointTransfIdent<Point>());
    if (! mv.is_null())
        return mv;
    Variant variant = bd.get_variant();
    if (variant == variant_duo || variant == variant_junior)
        mv = genmove(bd, c, delta, max_delta,
                     PointTransfRot270Refl<Point>(),
                     PointTransfRot270Refl<Point>());
    return mv;
}

Move Book::genmove(const Board& bd, Color c, double delta, double max_delta,
                   const PointTransform<Point>& transform,
                   const PointTransform<Point>& inv_transform)
{
    LIBBOARDGAME_ASSERT(! bd.has_setup());
    const libboardgame_sgf::Node* node = &m_tree.get_root();
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv = bd.get_move(i);
        mv.move = get_transformed(bd, mv.move, transform);
        node = m_tree.find_child_with_move(*node, mv);
        if (node == 0)
            return Move::null();
    }
    if (m_is_computer_generated)
    {
        try
        {
            node = select_child(m_random, bd, c, m_tree, *node, delta,
                                max_delta);
        }
        catch (const Exception&)
        {
            log() << "Position in book but not all children evaluated.\n";
            return Move::null();
        }
    }
    else
        node = select_annotated_child(m_random, bd, c, m_tree, *node);
    if (node == 0)
        return Move::null();
    return get_transformed(bd, m_tree.get_move(*node).move, inv_transform);
}

Move Book::get_transformed(const Board& bd, Move mv,
                           const PointTransform<Point>& transform) const
{
    if (mv.is_pass())
        return mv;
    unsigned int width = bd.get_geometry().get_width();
    unsigned int height = bd.get_geometry().get_height();
    MovePoints points;
    BOOST_FOREACH(Point p, bd.get_move_points(mv))
        points.push_back(transform.get_transformed(p, width, height));
    Move transformed_mv;
    bd.find_move(points, transformed_mv);
    return transformed_mv;
}

double Book::inv_value(double value) const
{
    return 1- value;
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
    bool has_root_annotated_good_children = false;
    for (ChildIterator i(m_tree.get_root()); i; ++i)
        if (m_tree.get_good_move(*i) > 0)
        {
            has_root_annotated_good_children = true;
            break;
        }
    m_is_computer_generated = ! has_root_annotated_good_children;
}

const Node* Book::select_annotated_child(RandomGenerator& random,
                                         const Board& bd, Color c,
                                         const Tree& tree, const Node& node)
{
    unsigned int nu_children = node.get_nu_children();
    if (nu_children == 0)
        return 0;
    vector<const Node*> good_moves;
    for (unsigned int i = 0; i < nu_children; ++i)
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
    unsigned int nu_good_moves = static_cast<unsigned int>(good_moves.size());
    return good_moves[random.generate_small_int(nu_good_moves)];
}

const Node* Book::select_child(RandomGenerator& random, const Board& bd,
                               Color c, const Tree& tree, const Node& node,
                               double delta, double max_delta)
{
    unsigned int nu_children = node.get_nu_children();
    if (nu_children == 0)
        return 0;
    double best_value = numeric_limits<double>::max();
    for (ChildIterator i(node); i; ++i)
    {
        double value = tree.get_comment_property<double>(*i, "v");
        best_value = min(value, best_value);
    }
    // Chose move with the largest difference to the best move that is still
    // below a threshold selected using an exponentially decreasing probability
    // distribution with width delta and cut-off max_delta
    unsigned int n = 0;
    double threshold =
        min(-delta * std::log(random.generate_float()), max_delta);
    const Node* result = 0;
    double result_value = 0;
    for (ChildIterator i(node); i; ++i)
    {
        ColorMove mv = tree.get_move(*i);
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
        double value = tree.get_comment_property<double>(*i, "v");
        if (value < best_value + max_delta)
            ++n;
        if (result == 0 ||
            (value > result_value && value < best_value + threshold))
        {
            result = &(*i);
            result_value = value;
        }
        log() << (format("%s %.3f\n")
                  % bd.to_string(mv.move) % inv_value(value));
    }
    log(format("Book moves: %i (d=%.3f max_d=%.3f thr=%.3f")
        %n % delta % max_delta % threshold);
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
