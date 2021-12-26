//-----------------------------------------------------------------------------
/** @file pentobi_gtp/GtpEngine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_GTP_GTP_ENGINE_H
#define PENTOBI_GTP_GTP_ENGINE_H

#include "libpentobi_gtp/GtpEngine.h"
#include "libpentobi_mcts/Player.h"

using namespace std;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;
using libpentobi_base::PlayerBase;
using libpentobi_base::Variant;
using libpentobi_mcts::Player;
using libpentobi_mcts::Search;

//-----------------------------------------------------------------------------

class GtpEngine
    : public libpentobi_gtp::GtpEngine
{
public:
    explicit GtpEngine(
            Variant variant, unsigned level = 5, bool use_book = true,
            const string& books_dir = {}, unsigned nu_threads = 0);

    ~GtpEngine() override;

    void cmd_param(Arguments args, Response& response);
    void cmd_get_value(Response& response);
    void cmd_move_values(Response& response);
    static void cmd_name(Response& response);
    void cmd_selfplay(Arguments args);
    void cmd_save_tree(Arguments args);
    static void cmd_version(Response& response);

    Player& get_mcts_player();

private:
    unique_ptr<PlayerBase> m_player;

    void create_player(Variant variant, unsigned level,
                       const string& books_dir, unsigned nu_threads);

    Search& get_search();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_GTP_GTP_ENGINE_H
