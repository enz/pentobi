//-----------------------------------------------------------------------------
/** @file libboardgame_base/Engine.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_ENGINE_H
#define LIBBOARDGAME_BASE_ENGINE_H

#include <memory>
#include "libboardgame_gtp/Engine.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;

//-----------------------------------------------------------------------------

class Engine
    : public libboardgame_gtp::Engine
{
public:
    void cmd_cputime(Response&);
    void cmd_cputime_diff(Response&);
    void cmd_interrupt();
    void cmd_set_random_seed(const Arguments&);

    Engine();

    ~Engine() throw();

    void interrupt();

protected:
    void on_handle_cmd_begin();

private:
    double m_cpu_time_last;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_ENGINE_H
