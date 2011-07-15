//-----------------------------------------------------------------------------
/** @file Parameters.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_PARAMETERS_H
#define LIBBOARDGAME_UTIL_PARAMETERS_H

#include <map>
#include <memory>
#include "Exception.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

class ParameterBase
{
public:
    virtual ~ParameterBase();

    virtual bool operator==(const ParameterBase& p) const = 0;

    virtual ParameterBase* clone() const = 0;

    bool operator!=(const ParameterBase& p) const;
};

inline bool ParameterBase::operator!=(const ParameterBase& p) const
{
    return ! operator==(p);
}

//-----------------------------------------------------------------------------

template<typename T>
class Parameter
    : public ParameterBase
{
public:
    Parameter(const T& value);

    bool operator==(const ParameterBase& p) const;

    ParameterBase* clone() const;

    void set(const T& value);

    const T& get() const;

private:
    T m_value;
};

template<typename T>
inline Parameter<T>::Parameter(const T& value)
    : m_value(value)
{
}

template<typename T>
bool Parameter<T>::operator==(const ParameterBase& p) const
{
    const Parameter<T>* param = dynamic_cast<const Parameter<T>*>(&p);
    if (param == 0)
        return false;
    return m_value == param->m_value;
}

template<typename T>
ParameterBase* Parameter<T>::clone() const
{
    return new Parameter<T>(m_value);
}

template<typename T>
inline const T& Parameter<T>::get() const
{
    return m_value;
}

template<typename T>
void Parameter<T>::set(const T& value)
{
    m_value = value;
}

//-----------------------------------------------------------------------------

class Parameters
{
public:
    Parameters();

    Parameters(const Parameters& p);

    Parameters& operator=(const Parameters& p);

    bool operator==(const Parameters& p) const;

    bool operator!=(const Parameters& p) const;

    /** @param key
        @param param The new parameter (@ref libboardgame_doc_takesownership) */
    void create_base(const string& key, ParameterBase* param);

    template<typename T>
    void create(const string& key, const T& value);

    ParameterBase& get(const string& key);

    template<typename T>
    Parameter<T> get(const string& key);

private:
    map<string, shared_ptr<ParameterBase>> m_map;
};

inline Parameters::Parameters()
{
}

inline bool Parameters::operator!=(const Parameters& p) const
{
    return ! operator==(p);
}

template<typename T>
void Parameters::create(const string& key, const T& value)
{
    create_base(key, new Parameter<T>(value));
}

template<typename T>
Parameter<T> Parameters::get(const string& key)
{
    return dynamic_cast<Parameter<T>>(get(key));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_PARAMETERS_H
