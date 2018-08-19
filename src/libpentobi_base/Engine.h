//-----------------------------------------------------------------------------
/** @file libpentobi_base/Engine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_ENGINE_H
#define LIBPENTOBI_BASE_ENGINE_H

#include "Game.h"
#include "PlayerBase.h"
#include "libboardgame_base/Engine.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;

//-----------------------------------------------------------------------------

/** GTP Blokus engine. */
class Engine
    : public libboardgame_base::Engine
{
public:
    explicit Engine(Variant variant);

    void cmd_all_legal(Arguments args, Response& response);
    void cmd_clear_board();
    void cmd_final_score(Response& response);
    void cmd_g(Response& response);
    void cmd_genmove(Arguments args, Response& response);
    void cmd_get_place(Arguments args, Response& response);
    void cmd_loadsgf(Arguments args);
    void cmd_move_info(Arguments args, Response& response);
    void cmd_p(Arguments args);
    void cmd_param_base(Arguments args, Response& response);
    void cmd_play(Arguments args);
    void cmd_point_integers(Response& response);
    void cmd_showboard(Response& response);
    void cmd_reg_genmove(Arguments args, Response& response);
    void cmd_savesgf(Arguments args);
    void cmd_set_game(Arguments args);
    void cmd_undo();

    /** Set the player.
        @param player The player (@ref libboardgame_doc_storesref) */
    void set_player(PlayerBase& player);

    void set_accept_illegal(bool enable);

    /** Enable or disable resigning. */
    void set_resign(bool enable);

    void set_show_board(bool enable);

    const Board& get_board() const;

protected:
    Color get_color_arg(Arguments args, unsigned i) const;

    Color get_color_arg(Arguments args) const;

private:
    bool m_accept_illegal = false;

    bool m_show_board = false;

    bool m_resign = true;

    Game m_game;

    PlayerBase* m_player = nullptr;

    void board_changed();

    void genmove(Color c, Response& response);

    PlayerBase& get_player() const;

    void play(Color c, Arguments args, unsigned arg_move_begin);
};

inline const Board& Engine::get_board() const
{
    return m_game.get_board();
}

inline void Engine::set_accept_illegal(bool enable)
{
    m_accept_illegal = enable;
}

inline void Engine::set_resign(bool enable)
{
    m_resign = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_ENGINE_H
