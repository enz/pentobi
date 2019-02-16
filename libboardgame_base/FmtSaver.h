//----------------------------------------------------------------------------
/** @file libboardgame_base/FmtSaver.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_FMT_SAVER_H
#define LIBBOARDGAME_BASE_FMT_SAVER_H

#include <iostream>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Saves the formatting state of a stream and restores it in its
    destructor. */
class FmtSaver
{
public:
    explicit FmtSaver(ostream& out)
        : m_out(out)
    {
        m_dummy.copyfmt(out);
    }

    ~FmtSaver()
    {
        m_out.copyfmt(m_dummy);
    }

private:
    ostream& m_out;

    ios m_dummy{nullptr};
};

//----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_FMT_SAVER_H
