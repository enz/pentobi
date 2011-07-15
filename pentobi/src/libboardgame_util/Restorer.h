//-----------------------------------------------------------------------------
/** @file Restorer.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RESTORER_H
#define LIBBOARDGAME_UTIL_RESTORER_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

/** Temprarily change a variable and automatically restore its old value. */
template<typename T>
class Restorer
{
public:
    Restorer(T& variable, const T& value);

    ~Restorer() throw();

private:
    T& m_variable;

    T m_old_value;
};

template<typename T>
inline Restorer<T>::Restorer(T& variable, const T& value)
    : m_variable(variable),
      m_old_value(variable)
{
    m_variable = value;
}

template<typename T>
inline Restorer<T>::~Restorer() throw()
{
    m_variable = m_old_value;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RESTORER_H
