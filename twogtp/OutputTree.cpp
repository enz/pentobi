//-----------------------------------------------------------------------------
/** @file twogtp/OutputTree.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "OutputTree.h"

#include <fstream>
#include "libboardgame_base/TreeReader.h"
#include "libboardgame_base/TreeWriter.h"
#include "libpentobi_base/BoardUtil.h"

using libboardgame_base::ArrayList;
using libboardgame_base::SgfNode;
using libboardgame_base::SgfTree;
using libboardgame_base::TreeReader;
using libboardgame_base::TreeWriter;
using libpentobi_base::get_transforms;
using libpentobi_base::ColorMove;
using libpentobi_base::MovePoints;
using libpentobi_base::get_transformed;

//-----------------------------------------------------------------------------

namespace {

void add(PentobiTree& tree, const SgfNode& node, bool is_player_black,
         bool is_real_move, float result)
{
    unsigned index = is_player_black ? 0 : 1;
    array<unsigned short, 2> count;
    array<float, 2> avg_result;
    array<unsigned short, 2> real_count;
    auto comment = SgfTree::get_comment(node);
    if (comment.empty())
    {
        count.fill(0);
        avg_result.fill(0);
        real_count.fill(0);
        count[index] = 1;
        real_count[index] = 1;
        avg_result[index] = result;
    }
    else
    {
        istringstream in(comment);
        in >> count[0] >> real_count[0] >> avg_result[0]
           >> count[1] >> real_count[1] >> avg_result[1];
        if (! in)
            throw runtime_error("OutputTree: invalid comment: " + comment);
        ++count[index];
        avg_result[index] += (result - avg_result[index]) / count[index];
        if (is_real_move)
            ++real_count[index];
    }
    ostringstream out;
    out.precision(numeric_limits<double>::digits10);
    out << count[0] << ' ' << real_count[0] << ' ' << avg_result[0] << '\n'
        << count[1] << ' ' << real_count[1] << ' ' << avg_result[1];
    tree.set_comment(node, out.str());
}

bool compare_sequence(ArrayList<ColorMove, Board::max_moves>& s1,
                      ArrayList<ColorMove, Board::max_moves>& s2)
{
    LIBBOARDGAME_ASSERT(s1.size() == s2.size());
    for (unsigned i = 0; i < s1.size(); ++i)
    {
        LIBBOARDGAME_ASSERT(s1[i].color == s2[i].color);
        if (s1[i].move.to_int() < s2[i].move.to_int())
            return true;
        if (s1[i].move.to_int() > s2[i].move.to_int())
            return false;
    }
    return false;
}

unsigned get_real_count(const SgfNode& node, bool is_player_black)
{
    unsigned index = is_player_black ? 0 : 1;
    array<unsigned, 2> count;
    array<double, 2> avg_result;
    array<unsigned, 2> real_count;
    auto comment = SgfTree::get_comment(node);
    istringstream in(comment);
    in >> count[0] >> real_count[0] >> avg_result[0]
       >> count[1] >> real_count[1] >> avg_result[1];
    if (! in)
        throw runtime_error("OutputTree: invalid comment: " + comment);
    return real_count[index];
}

} // namespace

//-----------------------------------------------------------------------------

OutputTree::OutputTree(Variant variant)
    : m_tree(variant)
{
    get_transforms(variant, m_transforms, m_inv_transforms);
}

OutputTree::~OutputTree() = default; // Non-inline to avoid GCC -Winline warning

void OutputTree::add_game(const Board& bd, unsigned player_black,
                          float result, const array<bool,
                          Board::max_moves>& is_real_move)
{
    if (bd.has_setup())
        throw runtime_error("OutputTree: setup not supported");

    // Find the canonical representation
    ArrayList<ColorMove, Board::max_moves> sequence;
    for (auto& transform : m_transforms)
    {
        ArrayList<ColorMove, Board::max_moves> s;
        for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
        {
            auto mv = bd.get_move(i);
            s.push_back(ColorMove(mv.color,
                                  get_transformed(bd, mv.move, *transform)));
        }
        if (sequence.empty() || compare_sequence(s, sequence))
            sequence = s;
    }

    auto node = &m_tree.get_root();
    add(m_tree, *node, player_black == 0, true, result);
    unsigned nu_moves_3 = 0;
    for (unsigned i = 0; i < sequence.size(); ++i)
    {
        unsigned player;
        auto mv = sequence[i];
        Color c = mv.color;
        if (bd.get_variant() == Variant::classic_3 && c == Color(3))
        {
            player = nu_moves_3 % 3;
            ++nu_moves_3;
        }
        else
            player = c.to_int() % bd.get_nu_players();
        auto child = m_tree.find_child_with_move(*node, mv);
        if (child == nullptr)
        {
            child = &m_tree.create_new_child(*node);
            m_tree.set_move(*child, mv);
            add(m_tree, *child, player == player_black, true, result);
            return;
        }
        add(m_tree, *child, player == player_black, is_real_move[i], result);
        node = child;
    }
}

void OutputTree::load(const string& file)
{
    TreeReader reader;
    reader.read(file);
    auto tree = reader.move_tree();
    m_tree.init(tree);
}

void OutputTree::save(const string& file)
{
    ofstream out(file);
    TreeWriter writer(out, m_tree.get_root());
    writer.write();
}

//-----------------------------------------------------------------------------
