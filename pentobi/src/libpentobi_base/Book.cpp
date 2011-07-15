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
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeReader;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

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

Move Book::genmove(const Board& bd, Color c, double delta, double max_delta)
{
    const libboardgame_sgf::Node* node = &m_tree.get_root();
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
    {
        ColorMove mv = bd.get_move(i);
        node = m_tree.find_child_with_move(*node, mv);
        if (node == 0)
            return Move::null();
    }
    node = select_child(m_random, bd, c, m_tree, *node, delta, max_delta);
    if (node == 0)
        return Move::null();
    return m_tree.get_move(*node).move;
}

double Book::get_value(const Tree& tree, const Node& node)
{
    istringstream in(tree.get_comment(node));
    double result;
    in >> result;
    if (! in)
        throw Exception("Missing value in book");
    return result;
}

void Book::get_value_and_count(const Tree& tree, const Node& node,
                               double& value, unsigned int& count)
{
    istringstream in(tree.get_comment(node));
    in >> value >> count;
    if (! in)
        throw Exception("Missing value and/or count in book");
}

const Node* Book::select_child(RandomGenerator& random, const Board& bd,
                               Color c, const Tree& tree, const Node& node,
                               double delta, double max_delta)
{
    unsigned int nu_children = node.get_nu_children();
    if (nu_children == 0)
        return 0;
    double max_value = -numeric_limits<double>::max();
    double sum_count = 0;
    for (ChildIterator i(node); i; ++i)
    {
        double value;
        unsigned int count;
        get_value_and_count(tree, *i, value, count);
        sum_count += count;
        ColorMove mv = tree.get_move(*i);
        log() << (format("%s %.3f %.0f\n")
                  % bd.to_string(mv.move) % value % count);
        max_value = max(value, max_value);
    }
    log() << "Sum count " << sum_count << '\n';
    vector<double> weight(nu_children);
    double sum_weight = 0;
    unsigned int n = 0;
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
        double value;
        unsigned int count;
        get_value_and_count(tree, child, value, count);
        if (value < max_value - max_delta)
            weight[i] = 0;
        else
        {
            weight[i] = exp(value / delta);
            sum_weight += weight[i];
            ++n;
        }
    }
    log() << "Book moves: " << n << " (max_delta=" << max_delta << ")\n";
    double rand = random.generate_float() * sum_weight;
    unsigned int i = 0;
    double s = 0;
    for (i = 0; i < nu_children - 1; ++i)
        if (weight[i] > 0)
        {
            s += weight[i];
            if (s > rand)
                break;
        }
    log(format("Prob: %.1f %%") % (100 * weight[i] / sum_weight));
    return &node.get_child(i);
}

void Book::set_value_and_count(Game& game, double value, double count)
{
    ostringstream s;
    s << value << ' ' << count;
    game.set_comment(s.str());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
