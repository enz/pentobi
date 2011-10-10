//-----------------------------------------------------------------------------
/** @file pentobi_gtp/Engine.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_GTP_ENGINE_H
#define PENTOBI_GTP_ENGINE_H

#include "libpentobi_base/Engine.h"
#include "libpentobi_mcts/Player.h"

namespace pentobi_gtp {

using namespace std;
using boost::filesystem::path;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;
using libpentobi_base::GameVariant;
using libpentobi_mcts::Player;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class Engine
    : public libpentobi_base::Engine
{
public:
    Engine(GameVariant game_variant, int level = 5, bool use_book = true,
           const path& application_dir_path = path());

    ~Engine() throw();

    void cmd_gen_playout_move(Response& response);

    void cmd_param(const Arguments& args, Response& response);

    void cmd_move_values(Response& response);

    Player& get_mcts_player();

private:
    unique_ptr<libpentobi_base::Player> m_player;

    void create_player(GameVariant game_variant,
                       const path& application_dir_path);

    Search& get_search();
};

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp

#endif // PENTOBI_GTP_ENGINE_H
