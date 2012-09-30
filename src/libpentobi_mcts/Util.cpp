//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include "libboardgame_sgf/Writer.h"
#include "libpentobi_base/BoardUtil.h"
#include "libpentobi_base/SgfUtil.h"

namespace libpentobi_mcts {
namespace util {

using libboardgame_mcts::ChildIterator;
using libboardgame_mcts::Node;
using libboardgame_mcts::Tree;
using libboardgame_sgf::Writer;
using libpentobi_base::boardutil::write_setup;
using libpentobi_base::sgf_util::get_color_id;

//-----------------------------------------------------------------------------

namespace {

void dump_tree_recurse(Writer& writer, Variant variant,
                       const Tree<Move>& tree, const Node<Move>& node,
                       Color to_play)
{
    ostringstream comment;
    comment << "Cnt: " << node.get_count();
    if (node.get_count() > 0)
        comment << "\nVal: " << node.get_value();
    comment << "\nRAVE cnt: " << node.get_rave_count();
    if (node.get_rave_count() > 0)
        comment << "\nRAVE val: " << node.get_rave_value();
    writer.write_property("C", comment.str());
    writer.end_node();
    Color next_to_play = to_play.get_next(get_nu_colors(variant));
    vector<const Node<Move>*> children;
    for (ChildIterator<Move> i(tree, node); i; ++i)
        children.push_back(&(*i));
    sort(children.begin(), children.end(), compare_node);
    BOOST_FOREACH(const Node<Move>* i, children)
    {
        writer.begin_tree();
        writer.begin_node();
        Move mv =  i->get_move();
        if (! mv.is_null())
        {
            const BoardConst& board_const = BoardConst::get(variant);
            const char* id = get_color_id(variant, to_play);
            if (! mv.is_pass())
                writer.write_property(id, board_const.to_string(mv, false));
            else
                writer.write_property(id, "");
        }
        dump_tree_recurse(writer, variant, tree, *i, next_to_play);
        writer.end_tree();
    }
}

} // namespace

//-----------------------------------------------------------------------------

bool compare_node(const Node<Move>* n1, const Node<Move>* n2)
{
    Float count1 = n1->get_count();
    Float count2 = n2->get_count();
    if (count1 != count2)
        return count1 > count2;
    if (count1 > 0 && count2 > 0)
        return n1->get_value() > n2->get_value();
    return false;
}

void dump_tree(ostream& out, const Search& search)
{
    Variant variant;
    Setup setup;
    search.get_root_position(variant, setup);
    Writer writer(out);
    writer.set_one_prop_per_line(true);
    writer.set_one_prop_value_per_line(true);
    writer.begin_tree();
    writer.begin_node();
    writer.write_property("GM", to_string(variant));
    write_setup(writer, variant, setup);
    writer.write_property("PL", get_color_id(variant, setup.to_play));
    const Tree<Move>& tree = search.get_tree();
    dump_tree_recurse(writer, variant, tree, tree.get_root(), setup.to_play);
    writer.end_tree();
}

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libpentobi_mcts
