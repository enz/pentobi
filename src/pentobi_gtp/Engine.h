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
    Engine(GameVariant game_variant, int level = 5,
           bool use_book = true, const path& books_dir = path(),
           size_t memory = 0);

    ~Engine() throw();

    void cmd_gen_playout_move(Response&);
    void cmd_param(const Arguments&, Response&);
    void cmd_get_value(Response&);
    void cmd_move_values(Response&);
    void cmd_name(Response&);
    void cmd_save_tree(const Arguments&);
    void cmd_version(Response&);

    Player& get_mcts_player();

    /** @see Search::set_deterministic() */
    void set_deterministic();

private:
    unique_ptr<libpentobi_base::Player> m_player;

    void create_player(GameVariant game_variant, const path& books_dir,
                       size_t memory);

    Search& get_search();
};

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp

#endif // PENTOBI_GTP_ENGINE_H
