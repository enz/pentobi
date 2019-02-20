//-----------------------------------------------------------------------------
/** @file libpentobi_gtp/GtpEngine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GTP_GTP_ENGINE_H
#define LIBPENTOBI_GTP_GTP_ENGINE_H

#include "libboardgame_gtp/GtpEngine.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_base/PlayerBase.h"

namespace libpentobi_gtp {

using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Game;
using libpentobi_base::PlayerBase;
using libpentobi_base::Variant;
using libboardgame_gtp::Arguments;
using libboardgame_gtp::Response;

//-----------------------------------------------------------------------------

/** GTP Blokus engine. */
class GtpEngine
    : public libboardgame_gtp::GtpEngine
{
public:
    explicit GtpEngine(Variant variant);

    void cmd_all_legal(Arguments args, Response& response);
    void cmd_clear_board();
    void cmd_cputime(Response& response);
    void cmd_final_score(Response& response);
    void cmd_g(Response& response);
    void cmd_genmove(Arguments args, Response& response);
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
    void cmd_set_random_seed(Arguments args);
    void cmd_undo();

    /** Set the player.
        @param player The player. The lifetime of this parameter must
        exceed the lifetime of the class instance. */
    void set_player(PlayerBase& player);

    void set_accept_illegal(bool enable) { m_accept_illegal = enable; }

    /** Enable or disable resigning. */
    void set_resign(bool enable) { m_resign = enable; }

    void set_show_board(bool enable);

    const Board& get_board() const { return m_game.get_board(); }

protected:
    Color get_color_arg(Arguments args, unsigned i) const;

    Color get_color_arg(Arguments args) const;

    void on_handle_cmd_begin() override;

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

//-----------------------------------------------------------------------------

} // namespace libpentobi_gtp

#endif // LIBPENTOBI_GTP_GTP_ENGINE_H
