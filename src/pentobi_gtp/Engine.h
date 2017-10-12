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
    explicit Engine(Variant variant, unsigned level = 5,
                    bool use_book = true, const string& books_dir = "",
                    unsigned nu_threads = 0);

    ~Engine() override;

    void cmd_param(Arguments args, Response& response);
    void cmd_get_value(Response& response);
    void cmd_move_values(Response& response);
    void cmd_name(Response& response);
    void cmd_selfplay(Arguments args);
    void cmd_save_tree(Arguments args);
    void cmd_version(Response& response);

    Player& get_mcts_player();

    /** @see Player::use_cpu_time() */
    void use_cpu_time(bool enable);

private:
    unique_ptr<PlayerBase> m_player;

    void create_player(Variant variant, unsigned level,
                       const string& books_dir, unsigned nu_threads);

    Search& get_search();
};

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp

#endif // PENTOBI_GTP_ENGINE_H
