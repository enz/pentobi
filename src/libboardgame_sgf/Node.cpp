//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Node.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Node.h"

#include "MissingProperty.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

Property::Property(const string& id, const vector<string>& values)
    : id(id),
      values(values)
{
    LIBBOARDGAME_ASSERT(! values.empty());
}

//-----------------------------------------------------------------------------

Node::Node()
    : m_parent(0)
{
}

void Node::append(unique_ptr<Node> node)
{
    node->m_parent = this;
    if (m_first_child.get() == 0)
        m_first_child = move(node);
    else
        get_last_child()->m_sibling = move(node);
}

Node& Node::create_new_child()
{
    unique_ptr<Node> node(new Node());
    node->m_parent = this;
    Node& result = *(node.get());
    Node* last_child = get_last_child();
    if (last_child == 0)
        m_first_child = move(node);
    else
        last_child->m_sibling = move(node);
    return result;
}

Property* Node::find_property(const string& id) const
{
    Property* property = m_first_property.get();
    while (property != 0)
    {
        if (property->id == id)
            break;
        property = property->next.get();
    }
    return property;
}

const vector<string> Node::get_multi_property(const string& id) const
{
    Property* property = find_property(id);
    if (property == 0)
        return vector<string>();
    else
        return property->values;
}

bool Node::has_property(const string& id) const
{
    return find_property(id) != 0;
}

const Node& Node::get_child(unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < get_nu_children());
    const Node* child = m_first_child.get();
    while (i > 0)
    {
        child = child->m_sibling.get();
        --i;
    }
    return *child;
}

unsigned int Node::get_child_index(const Node& child) const
{
    const Node* current = m_first_child.get();
    unsigned int i = 0;
    while (true)
    {
        if (current == &child)
            return i;
        current = current->m_sibling.get();
        LIBBOARDGAME_ASSERT(current != 0);
        ++i;
    }
}

Node* Node::get_last_child() const
{
    Node* node = m_first_child.get();
    if (node == 0)
        return 0;
    while (node->m_sibling.get() != 0)
        node = node->m_sibling.get();
    return node;
}

unsigned int Node::get_nu_children() const
{
    unsigned int n = 0;
    const Node* child = m_first_child.get();
    while (child != 0)
    {
        ++n;
        child = child->m_sibling.get();
    }
    return n;
}

const Node* Node::get_previous_sibling() const
{
    if (m_parent == 0)
        return 0;
    const Node* child = &m_parent->get_first_child();
    if (child == this)
        return 0;
    do
    {
        if (child->get_sibling() == this)
            return child;
        child = child->get_sibling();
    }
    while (child != 0);
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

const string& Node::get_property(const string& id) const
{
    Property* property = find_property(id);
    if (property == 0)
        throw MissingProperty(id);
    return property->values[0];
}

const string& Node::get_property(const string& id,
                                 const string& default_value) const
{
    Property* property = find_property(id);
    if (property == 0)
        return default_value;
    else
        return property->values[0];
}

void Node::make_first_child()
{
    LIBBOARDGAME_ASSERT(has_parent());
    Node* current_child = m_parent->m_first_child.get();
    if (current_child == this)
        return;
    while (true)
    {
        Node* sibling = current_child->m_sibling.get();
        if (sibling == this)
        {
            unique_ptr<Node> tmp = move(m_parent->m_first_child);
            m_parent->m_first_child = move(current_child->m_sibling);
            current_child->m_sibling = move(m_sibling);
            m_sibling = move(tmp);
            return;
        }
        current_child = sibling;
    }
}

bool Node::remove_property(const string& id)
{
    Property* property = m_first_property.get();
    Property* last = 0;
    while (property != 0)
    {
        if (property->id == id)
            break;
        last = property;
        property = property->next.get();
    }
    if (property == 0)
        return false;
    if (last != 0)
        last->next = move(property->next);
    return true;
}

void Node::remove_child(Node& child)
{
    Node* node = m_first_child.get();
    Node* previous = 0;
    while (true)
    {
        if (node == &child)
        {
            if (previous == 0)
                m_first_child = move(child.m_sibling);
            else
                previous->m_sibling = move(child.m_sibling);
            return;
        }
        previous = node;
        node = node->m_sibling.get();
        LIBBOARDGAME_ASSERT(node != 0);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
