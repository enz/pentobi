//-----------------------------------------------------------------------------
/** @file libpentobi_base/Player.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Player.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

Player::~Player()
{
}

bool Player::resign() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
