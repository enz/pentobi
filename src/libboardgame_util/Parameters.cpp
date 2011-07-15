//-----------------------------------------------------------------------------
/** @file Parameters.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Parameters.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

ParameterBase::~ParameterBase()
{
}

//-----------------------------------------------------------------------------

Parameters::Parameters(const Parameters& p)
{
    *this = p;
}

Parameters& Parameters::operator=(const Parameters& p)
{
    m_map.clear();
    for (auto i = p.m_map.begin(); i != p.m_map.end(); ++i)
        create_base(i->first, i->second->clone());
    return *this;
}

bool Parameters::operator==(const Parameters& p) const
{
    if (m_map.size() != p.m_map.size())
        return false;
    auto i2 = p.m_map.begin();
    for (auto i = m_map.begin(); i != m_map.end(); ++i, ++i2)
        if (i->first != i2->first || *(i->second) != *(i2->second))
            return false;
    return true;
}

void Parameters::create_base(const string& key, ParameterBase* param)
{
    auto pos = m_map.find(key);
    if (pos != m_map.end())
        throw Exception(format("parameter '%1% already exists'") % key);
    m_map.insert(make_pair(key, shared_ptr<ParameterBase>(param)));
}

ParameterBase& Parameters::get(const string& key)
{
    auto i = m_map.find(key);
    if (i == m_map.end())
        throw Exception(format("unknown parameter '%1%'") % key);
    return *(i->second);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
