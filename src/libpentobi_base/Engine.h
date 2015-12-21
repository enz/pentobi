//-----------------------------------------------------------------------------
/** @file libpentobi_base/Engine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_ENGINE_H
#define LIBPENTOBI_BASE_ENGINE_H

#include "libpentobi_base/Game.h"
#include "libpentobi_base/PlayerBase.h"
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

    void cmd_all_legal(const Arguments&, Response&);
    void cmd_clear_board();
    void cmd_final_score(Response&);
    void cmd_g(Response&);
    void cmd_genmove(const Arguments&, Response&);
    void cmd_get_place(const Arguments& args, Response&);
    void cmd_loadsgf(const Arguments&);
    void cmd_move_info(const Arguments&, Response&);
    void cmd_p(const Arguments&);
    void cmd_param_base(const Arguments&, Response&);
    void cmd_play(const Arguments&);
    void cmd_point_integers(Response&);
    void cmd_showboard(Response&);
    void cmd_reg_genmove(const Arguments&, Response&);
    void cmd_set_game(const Arguments&);
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
    Color get_color_arg(const Arguments& args, unsigned i) const;

    Color get_color_arg(const Arguments& args) const;

private:
    bool m_accept_illegal = false;

    bool m_show_board = false;

    bool m_resign = true;

    Game m_game;

    PlayerBase* m_player = nullptr;

    void board_changed();

    void genmove(Color c, Response& response);

    PlayerBase& get_player() const;

    void play(Color c, const Arguments& args, unsigned arg_move_begin);
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
