//-----------------------------------------------------------------------------
/** @file TreeReader.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_TREE_READER_H
#define LIBBOARDGAME_SGF_TREE_READER_H

#include <memory>
#include <stack>
#include "Reader.h"
#include "Node.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

class TreeReader
    : public Reader
{
public:
    TreeReader();

    ~TreeReader() throw();

    void on_begin_tree(bool is_root);

    void on_end_tree(bool is_root);

    void on_begin_node(bool is_root);

    void on_end_node();

    void on_property(const string& identifier, const vector<string>& values);

    const Node& get_tree() const;

    /** Get the tree and transfer the ownership to the caller. */
    unique_ptr<Node> get_tree_transfer_ownership();

private:
    Node* m_current;

    unique_ptr<Node> m_root;

    stack<Node*> m_stack;
};

inline const Node& TreeReader::get_tree() const
{
    return *m_root.get();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_TREE_READER_H
