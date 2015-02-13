//-----------------------------------------------------------------------------
/** @file pentobi_gtp/Engine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_GTP_ENGINE_H
#define PENTOBI_GTP_ENGINE_H

#include "libpentobi_base/Engine.h"
#include "libpentobi_mcts/Player.h"

namespace pentobi_gtp {

using namespace std;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;
using libpentobi_base::PlayerBase;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class Engine
    : public libpentobi_base::Engine
{
public:
    Engine(Variant variant, int level = 5,
           bool use_book = true, const string& books_dir = "",
           unsigned nu_threads = 0, size_t memory = 0);

    ~Engine();

    void cmd_param(const Arguments&, Response&);
    void cmd_get_value(Response&);
    void cmd_move_values(Response&);
    void cmd_name(Response&);
    void cmd_save_tree(const Arguments&);
    void cmd_version(Response&);

    Player& get_mcts_player();

    /** @see Search::set_deterministic() */
    void set_deterministic();

    /** @see Player::use_cpu_time() */
    void use_cpu_time(bool enable);

private:
    unique_ptr<PlayerBase> m_player;

    void create_player(Variant variant, const string& books_dir,
                       unsigned nu_threads, size_t memory);

    Search& get_search();
};

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp

#endif // PENTOBI_GTP_ENGINE_H
