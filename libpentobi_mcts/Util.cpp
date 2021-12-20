//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Util.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Util.h"

#include <thread>
#include "libboardgame_base/Writer.h"
#include "libpentobi_base/BoardUtil.h"
#include "libpentobi_base/PentobiSgfUtil.h"

namespace libpentobi_mcts {

using libboardgame_base::Writer;

//-----------------------------------------------------------------------------

namespace {

void dump_tree_recurse(Writer& writer, Variant variant,
                       const Search::Tree& tree, const Search::Node& node,
                       Color to_play)
{
    ostringstream comment;
    comment << "Visits: " << node.get_visit_count()
            << "\nPrior: " << node.get_move_prior()
            << "\nVal:   " << node.get_value()
            << "\nCnt:   " << node.get_value_count();
    writer.write_property("C", comment.str());
    writer.end_node();
    auto children = tree.get_children(node);
    if (children.empty())
        return;
    Color next_to_play = to_play.get_next(get_nu_colors(variant));    
    vector<const Search::Node*> sorted_children;
    sorted_children.reserve(children.size());
    for (auto& i : tree.get_children(node))
        sorted_children.push_back(&i);
    sort(sorted_children.begin(), sorted_children.end(), compare_node);
    for (const auto i : sorted_children)
    {
        writer.begin_tree();
        writer.begin_node();
        auto mv =  i->get_move();
        if (! mv.is_null())
        {
            auto& board_const = BoardConst::get(variant);
            auto id = get_color_id(variant, to_play);
            if (! mv.is_null())
                writer.write_property(id, board_const.to_string(mv, false));
        }
        dump_tree_recurse(writer, variant, tree, *i, next_to_play);
        writer.end_tree();
    }
}

} // namespace

//-----------------------------------------------------------------------------

bool compare_node(const Search::Node* n1, const Search::Node* n2)
{
    Float count1 = n1->get_visit_count();
    Float count2 = n2->get_visit_count();
    if (count1 != count2)
        return count1 > count2;
    return n1->get_value() > n2->get_value();
}

void dump_tree(ostream& out, const Search& search)
{
    Variant variant;
    Setup setup;
    search.get_root_position(variant, setup);
    Writer writer(out);
    writer.begin_tree();
    writer.begin_node();
    writer.write_property("GM", to_string(variant));
    write_setup(writer, variant, setup);
    writer.write_property("PL", get_color_id(variant, setup.to_play));
    auto& tree = search.get_tree();
    dump_tree_recurse(writer, variant, tree, tree.get_root(), setup.to_play);
    writer.end_tree();
}

unsigned get_nu_threads()
{
    unsigned nu_threads = thread::hardware_concurrency();
    if (nu_threads == 0)
    {
        LIBBOARDGAME_LOG("Could not determine the number of hardware threads");
        nu_threads = 1;
    }
    // The lock-free search probably scales up to 16-32 threads, but we
    // haven't tested more than 8 threads, we still use single precision
    // float for LIBBOARDGAME_MCTS_FLOAT_TYPE (which limits the maximum number
    // of simulations per search) and CPUs with more than 8 cores are
    // currently not very common anyway. Also, the loss of playing strength
    // of a multi-threaded search with the same count as a single-threaded
    // search will become larger with many threads, so there would need to be
    // a correction factor in the number of simulations per level to take this
    // into account.
    if (nu_threads > 8)
        nu_threads = 8;
    return nu_threads;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
