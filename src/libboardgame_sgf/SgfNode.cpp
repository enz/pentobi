//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfNode.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "SgfNode.h"

#include <algorithm>

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

Property::~Property() = default; // Non-inline to avoid GCC -Winline warning

//-----------------------------------------------------------------------------

SgfNode::~SgfNode() = default;  // Non-inline to avoid GCC -Winline warning

void SgfNode::append(unique_ptr<SgfNode> node)
{
    node->m_parent = this;
    if (! m_first_child)
        m_first_child = move(node);
    else
        get_last_child()->m_sibling = move(node);
}

SgfNode& SgfNode::create_new_child()
{
    auto node = make_unique<SgfNode>();
    node->m_parent = this;
    SgfNode& result = *(node.get());
    auto last_child = get_last_child();
    if (last_child == nullptr)
        m_first_child = move(node);
    else
        last_child->m_sibling = move(node);
    return result;
}

void SgfNode::delete_variations()
{
    if (m_first_child)
        m_first_child->m_sibling.reset(nullptr);
}

forward_list<Property>::const_iterator SgfNode::find_property(
        const string& id) const
{
    return find_if(m_properties.begin(), m_properties.end(),
                   [&](const Property& p) { return p.id == id; });
}

const vector<string>& SgfNode::get_multi_property(const string& id) const
{
    auto property = find_property(id);
    if (property == m_properties.end())
        throw MissingProperty(id);
    return property->values;
}

bool SgfNode::has_property(const string& id) const
{
    return find_property(id) != m_properties.end();
}

const SgfNode& SgfNode::get_child(unsigned i) const
{
    LIBBOARDGAME_ASSERT(i < get_nu_children());
    auto child = m_first_child.get();
    while (i > 0)
    {
        child = child->m_sibling.get();
        --i;
    }
    return *child;
}

unsigned SgfNode::get_child_index(const SgfNode& child) const
{
    auto current = m_first_child.get();
    unsigned i = 0;
    while (true)
    {
        if (current == &child)
            return i;
        current = current->m_sibling.get();
        LIBBOARDGAME_ASSERT(current);
        ++i;
    }
}

SgfNode* SgfNode::get_last_child() const
{
    auto node = m_first_child.get();
    if (node == nullptr)
        return nullptr;
    while (node->m_sibling)
        node = node->m_sibling.get();
    return node;
}

unsigned SgfNode::get_nu_children() const
{
    unsigned n = 0;
    auto child = m_first_child.get();
    while (child != nullptr)
    {
        ++n;
        child = child->m_sibling.get();
    }
    return n;
}

const SgfNode* SgfNode::get_previous_sibling() const
{
    if (m_parent == nullptr)
        return nullptr;
    auto child = &m_parent->get_first_child();
    if (child == this)
        return nullptr;
    do
    {
        if (child->get_sibling() == this)
            return child;
        child = child->get_sibling();
    }
    while (child != nullptr);
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

const string& SgfNode::get_property(const string& id) const
{
    auto property = find_property(id);
    if (property == m_properties.end())
        throw MissingProperty(id);
    return property->values[0];
}

const string& SgfNode::get_property(const string& id,
                                 const string& default_value) const
{
    auto property = find_property(id);
    if (property == m_properties.end())
        return default_value;
    return property->values[0];
}

void SgfNode::make_first_child()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current_child = m_parent->m_first_child.get();
    if (current_child == this)
        return;
    while (true)
    {
        auto sibling = current_child->m_sibling.get();
        if (sibling == this)
        {
            unique_ptr<SgfNode> tmp = move(m_parent->m_first_child);
            m_parent->m_first_child = move(current_child->m_sibling);
            current_child->m_sibling = move(m_sibling);
            m_sibling = move(tmp);
            return;
        }
        current_child = sibling;
    }
}

bool SgfNode::move_property_to_front(const string& id)
{
    auto i = m_properties.begin();
    forward_list<Property>::const_iterator previous = m_properties.end();
    for ( ; i != m_properties.end(); ++i)
        if (i->id == id)
            break;
        else
            previous = i;
    if (i == m_properties.begin() || i == m_properties.end())
        return false;
    auto property = *i;
    m_properties.erase_after(previous);
    m_properties.push_front(property);
    return true;
}

void SgfNode::move_down()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current = m_parent->m_first_child.get();
    if (current == this)
    {
        unique_ptr<SgfNode> tmp = move(m_parent->m_first_child);
        m_parent->m_first_child = move(m_sibling);
        m_sibling = move(m_parent->m_first_child->m_sibling);
        m_parent->m_first_child->m_sibling = move(tmp);
        return;
    }
    while (true)
    {
        auto sibling = current->m_sibling.get();
        if (sibling == this)
        {
            if (! m_sibling)
                return;
            unique_ptr<SgfNode> tmp = move(current->m_sibling);
            current->m_sibling = move(m_sibling);
            m_sibling = move(current->m_sibling->m_sibling);
            current->m_sibling->m_sibling = move(tmp);
            return;
        }
        current = sibling;
    }
}

void SgfNode::move_up()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current = m_parent->m_first_child.get();
    if (current == this)
        return;
    SgfNode* prev = nullptr;
    while (true)
    {
        auto sibling = current->m_sibling.get();
        if (sibling == this)
        {
            if (prev == nullptr)
            {
                make_first_child();
                return;
            }
            unique_ptr<SgfNode> tmp = move(prev->m_sibling);
            prev->m_sibling = move(current->m_sibling);
            current->m_sibling = move(m_sibling);
            m_sibling = move(tmp);
            return;
        }
        prev = current;
        current = sibling;
    }
}

bool SgfNode::remove_property(const string& id)
{
    forward_list<Property>::const_iterator previous = m_properties.end();
    for (auto i = m_properties.begin() ; i != m_properties.end(); ++i)
        if (i->id == id)
        {
            if (previous == m_properties.end())
                m_properties.pop_front();
            else
                m_properties.erase_after(previous);
            return true;
        }
        else
            previous = i;
    return false;
}

unique_ptr<SgfNode> SgfNode::remove_child(SgfNode& child)
{
    auto node = &m_first_child;
    unique_ptr<SgfNode>* previous = nullptr;
    while (true)
    {
        if (node->get() == &child)
        {
            unique_ptr<SgfNode> result = move(*node);
            if (previous == nullptr)
                m_first_child = move(child.m_sibling);
            else
                (*previous)->m_sibling = move(child.m_sibling);
            result->m_parent = nullptr;
            return result;
        }
        previous = node;
        node = &(*node)->m_sibling;
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
