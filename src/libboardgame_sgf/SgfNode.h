//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfNode.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_SGF_NODE_H
#define LIBBOARDGAME_SGF_SGF_NODE_H

#include <memory>
#include <string>
#include <vector>
#include "InvalidPropertyValue.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {

using namespace std;
using libboardgame_util::from_string;
using libboardgame_util::to_string;

//-----------------------------------------------------------------------------

struct Property
{
    string id;

    vector<string> values;

    unique_ptr<Property> next;

    Property(const string& id, const vector<string>& values);
};

//-----------------------------------------------------------------------------

class SgfNode
{
public:
    SgfNode();

    ~SgfNode();

    /** Append a new child. */
    void append(unique_ptr<SgfNode> node);

    bool has_property(const string& id) const;

    /** Get a property.
        @pre has_property(id) */
    const string& get_property(const string& id) const;

    const string& get_property(const string& id,
                               const string& default_value) const;

    const vector<string> get_multi_property(const string& id) const;

    /** Get property parsed as a type.
        @pre has_property(id)
        @throws InvalidPropertyValue, MissingProperty */
    template<typename T>
    T parse_property(const string& id) const;

    /** Get property parsed as a type with default value.
        @throws InvalidPropertyValue */
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

    bool move_property_to_front(const string& id);

    const Property* get_first_property() const;

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

    /** Remove all children.
        @return A pointer to the first child (which also owns its siblings),
        which can be used to append the children to a different node. */
    unique_ptr<SgfNode> remove_children();

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
    SgfNode* m_parent;

    unique_ptr<SgfNode> m_first_child;

    unique_ptr<SgfNode> m_sibling;

    unique_ptr<Property> m_first_property;

    Property* find_property(const string& id) const;

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
    return *m_first_child.get();
}

inline const SgfNode& SgfNode::get_first_child() const
{
    LIBBOARDGAME_ASSERT(has_children());
    return *(m_first_child.get());
}

inline SgfNode* SgfNode::get_first_child_or_null()
{
    return m_first_child.get();
}

inline const SgfNode* SgfNode::get_first_child_or_null() const
{
    return m_first_child.get();
}

inline const Property* SgfNode::get_first_property() const
{
    return m_first_property.get();
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
    return (m_first_child && ! m_first_child->m_sibling);
}

template<typename T>
T SgfNode::parse_property(const string& id) const
{
    string value = get_property(id);
    T result;
    if (! from_string(value, result))
        throw InvalidPropertyValue(id, value);
    return result;
}

template<typename T>
T SgfNode::parse_property(const string& id, const T& default_value) const
{
    if (! has_property(id))
        return default_value;
    return parse_property<T>(id);
}

inline unique_ptr<SgfNode> SgfNode::remove_children()
{
    if (m_first_child)
        m_first_child->m_parent = nullptr;
    return move(m_first_child);
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
    for (const T& v : values)
        values_to_string.push_back(to_string(v));
    auto property = m_first_property.get();
    if (! property)
    {
        m_first_property.reset(new Property(id, values_to_string));
        return true;
    }
    while (true)
    {
        if (property->id == id)
        {
            bool was_changed = (property->values != values_to_string);
            property->values = values_to_string;
            return was_changed;
        }
        if (! property->next)
        {
            property->next.reset(new Property(id, values_to_string));
            return true;
        }
        property = property->next.get();
    }
}

//-----------------------------------------------------------------------------

class PropertyIterator
{
public:
    PropertyIterator(const SgfNode& node);

    explicit operator bool() const;

    void operator++();

    const Property& operator*() const;

    const Property* operator->() const;

private:
    const Property* m_current;
};

inline PropertyIterator::PropertyIterator(const SgfNode& node)
{
    m_current = node.get_first_property();
}

inline PropertyIterator::operator bool() const
{
    return m_current != nullptr;
}

inline void PropertyIterator::operator++()
{
    LIBBOARDGAME_ASSERT(*this);
    m_current = m_current->next.get();
}

inline const Property& PropertyIterator::operator*() const
{
    LIBBOARDGAME_ASSERT(*this);
    return *m_current;
}

inline const Property* PropertyIterator::operator->() const
{
    LIBBOARDGAME_ASSERT(*this);
    return m_current;
}

//-----------------------------------------------------------------------------

class ChildIterator
{
public:
    ChildIterator(const SgfNode& node);

    explicit operator bool() const;

    void operator++();

    void operator--();

    const SgfNode& operator*() const;

    const SgfNode* operator->() const;

private:
    const SgfNode* m_current;
};

inline ChildIterator::ChildIterator(const SgfNode& node)
{
    m_current = node.get_first_child_or_null();
}

inline ChildIterator::operator bool() const
{
    return m_current != nullptr;
}

inline void ChildIterator::operator++()
{
    LIBBOARDGAME_ASSERT(*this);
    m_current = m_current->get_sibling();
}

inline void ChildIterator::operator--()
{
    LIBBOARDGAME_ASSERT(*this);
    m_current = m_current->get_previous_sibling();
    LIBBOARDGAME_ASSERT(*this);
}

inline const SgfNode& ChildIterator::operator*() const
{
    LIBBOARDGAME_ASSERT(*this);
    return *m_current;
}

inline const SgfNode* ChildIterator::operator->() const
{
    LIBBOARDGAME_ASSERT(*this);
    return m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_SGF_NODE_H
