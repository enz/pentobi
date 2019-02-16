//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfNode.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_SGF_NODE_H
#define LIBBOARDGAME_SGF_SGF_NODE_H

#include <forward_list>
#include <memory>
#include <string>
#include <vector>
#include "SgfError.h"
#include "libboardgame_base/Assert.h"
#include "libboardgame_base/StringUtil.h"

namespace libboardgame_sgf {

using libboardgame_base::from_string;
using libboardgame_base::to_string;

//-----------------------------------------------------------------------------

struct Property
{
    string id;

    vector<string> values;

    unique_ptr<Property> next;

    Property(const Property& p)
        : id(p.id),
          values(p.values)
    { }

    Property(const string& id, const vector<string>& values)
        : id(id),
          values(values)
    {
        LIBBOARDGAME_ASSERT(! id.empty());
        LIBBOARDGAME_ASSERT(! values.empty());
    }

    ~Property();
};

//-----------------------------------------------------------------------------

class SgfNode
{
public:
    /** Iterates over siblings. */
    class Iterator
    {
    public:
        explicit Iterator(const SgfNode* node)
        {
            m_node = node;
        }

        bool operator==(Iterator it) const
        {
            return m_node == it.m_node;
        }

        bool operator!=(Iterator it) const
        {
            return m_node != it.m_node;
        }

        Iterator& operator++()
        {
            m_node = m_node->get_sibling();
            return *this;
        }

        const SgfNode& operator*() const
        {
            return *m_node;
        }

        const SgfNode* operator->() const
        {
            return m_node;
        }

        bool is_null() const
        {
            return m_node == nullptr;
        }

    private:
        const SgfNode* m_node;
    };

    /** Range for iterating over the children of a node. */
    class Children
    {
    public:
        explicit Children(const SgfNode& node)
            : m_begin(node.get_first_child_or_null())
        { }

        Iterator begin() const { return m_begin; }

        Iterator end() const { return Iterator(nullptr); }

        bool empty() const { return m_begin.is_null(); }

    private:
        Iterator m_begin;
    };


    ~SgfNode();


    /** Append a new child. */
    void append(unique_ptr<SgfNode> node);

    bool has_property(const string& id) const;

    /** Get a property.
        @throws MissingProperty if no such property */
    const string& get_property(const string& id) const;

    const string& get_property(const string& id,
                               const string& default_value) const;

    const vector<string>& get_multi_property(const string& id) const;

    /** Get property parsed as a type.
        @throws InvalidProperty
        @throws MissingProperty */
    template<typename T>
    T parse_property(const string& id) const;

    /** Get property parsed as a type with default value.
        @throws InvalidProperty */
    template<typename T>
    T parse_property(const string& id, const T& default_value) const;

    /** @return true, if property was added or changed. */
    template<typename T>
    bool set_property(const string& id, const T& value);

    /** @return true, if property was added or changed. */
    bool set_property(const string& id, const char* value);

    /** @return true, if property was added or changed. */
    template<typename T>
    bool set_property(const string& id, const vector<T>& values);

    /** @return true, if node contained the property. */
    bool remove_property(const string& id);

    /** @return true, if the property was found and not already at the
        front. */
    bool move_property_to_front(const string& id);

    const forward_list<Property>& get_properties() const
    {
        return m_properties;
    }

    Children get_children() const
    {
        return Children(*this);
    }

    SgfNode* get_sibling();

    SgfNode& get_first_child();

    const SgfNode& get_first_child() const;

    SgfNode* get_first_child_or_null();

    const SgfNode* get_first_child_or_null() const;

    const SgfNode* get_sibling() const;

    const SgfNode* get_previous_sibling() const;

    bool has_children() const;

    bool has_single_child() const;

    unsigned get_nu_children() const;

    /** @pre i < get_nu_children() */
    const SgfNode& get_child(unsigned i) const;

    unsigned get_child_index(const SgfNode& child) const;

    /** Get single child.
        @pre has_single_child() */
    const SgfNode& get_child() const;

    bool has_parent() const;

    /** Get parent node.
        @pre has_parent() */
    const SgfNode& get_parent() const;

    /** Get parent node or null if node has no parent. */
    const SgfNode* get_parent_or_null() const;

    SgfNode& get_parent();

    SgfNode& create_new_child();

    /** Remove a child.
        @return The removed child node. */
    unique_ptr<SgfNode> remove_child(SgfNode& child);

    /** Remove all children. */
    void remove_children();

    /** @pre has_parent() */
    void make_first_child();

    /** Switch place with previous sibling.
        If the node is already the first child, nothing happens.
        @pre has_parent() */
    void move_up();

    /** Switch place with sibling.
        If the node is the last sibling, nothing happens.
        @pre has_parent() */
    void move_down();

    /** Delete all siblings of the first child. */
    void delete_variations();

private:
    SgfNode* m_parent = nullptr;

    unique_ptr<SgfNode> m_first_child;

    unique_ptr<SgfNode> m_sibling;

    /** The properties.
        Often a node has only one property (the move), so it saves memory
        to use a forward_list instead of a vector. */
    forward_list<Property> m_properties;

    forward_list<Property>::const_iterator find_property(
            const string& id) const;

    SgfNode* get_last_child() const;
};

inline const SgfNode& SgfNode::get_child() const
{
    LIBBOARDGAME_ASSERT(has_single_child());
    return *m_first_child;
}

inline const SgfNode& SgfNode::get_parent() const
{
    LIBBOARDGAME_ASSERT(has_parent());
    return *m_parent;
}

inline SgfNode& SgfNode::get_parent()
{
    LIBBOARDGAME_ASSERT(has_parent());
    return *m_parent;
}

inline const SgfNode* SgfNode::get_parent_or_null() const
{
    return m_parent;
}

inline SgfNode& SgfNode::get_first_child()
{
    LIBBOARDGAME_ASSERT(has_children());
    return *m_first_child;
}

inline const SgfNode& SgfNode::get_first_child() const
{
    LIBBOARDGAME_ASSERT(has_children());
    return *m_first_child;
}

inline SgfNode* SgfNode::get_first_child_or_null()
{
    return m_first_child.get();
}

inline const SgfNode* SgfNode::get_first_child_or_null() const
{
    return m_first_child.get();
}

inline SgfNode* SgfNode::get_sibling()
{
    return m_sibling.get();
}

inline const SgfNode* SgfNode::get_sibling() const
{
    return m_sibling.get();
}

inline bool SgfNode::has_children() const
{
    return static_cast<bool>(m_first_child);
}

inline bool SgfNode::has_parent() const
{
    return m_parent != nullptr;
}

inline bool SgfNode::has_single_child() const
{
    return m_first_child && ! m_first_child->m_sibling;
}

template<typename T>
T SgfNode::parse_property(const string& id) const
{
    string value = get_property(id);
    T result;
    if (! from_string(value, result))
        throw InvalidProperty(id, value);
    return result;
}

template<typename T>
T SgfNode::parse_property(const string& id, const T& default_value) const
{
    if (! has_property(id))
        return default_value;
    return parse_property<T>(id);
}

inline void SgfNode::remove_children()
{
    m_first_child.reset();
}

template<typename T>
bool SgfNode::set_property(const string& id, const T& value)
{
    vector<T> values(1, value);
    return set_property(id, values);
}

inline bool SgfNode::set_property(const string& id, const char* value)
{
    return set_property<string>(id, value);
}

template<typename T>
bool SgfNode::set_property(const string& id, const vector<T>& values)
{
    vector<string> values_to_string;
    values_to_string.reserve(values.size());
    for (const T& v : values)
        values_to_string.push_back(to_string(v));
    forward_list<Property>::const_iterator last = m_properties.end();
    for (auto i = m_properties.begin(); i != m_properties.end(); ++i)
        if (i->id == id)
        {
            bool was_changed = (i->values != values_to_string);
            i->values = values_to_string;
            return was_changed;
        }
        else
            last = i;
    if (last == m_properties.end())
        m_properties.emplace_front(id, values_to_string);
    else
        m_properties.emplace_after(last, id, values_to_string);
    return true;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_SGF_NODE_H
