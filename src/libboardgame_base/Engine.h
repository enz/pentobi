//-----------------------------------------------------------------------------
/** @file libboardgame_base/Engine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_ENGINE_H
#define LIBBOARDGAME_BASE_ENGINE_H

#include "libboardgame_gtp/Engine.h"

namespace libboardgame_base {

using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;

//-----------------------------------------------------------------------------

class Engine
    : public libboardgame_gtp::Engine
{
public:
    void cmd_cputime(Response& response);
    void cmd_set_random_seed(Arguments args);

    Engine();

protected:
    void on_handle_cmd_begin() override;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_ENGINE_H
