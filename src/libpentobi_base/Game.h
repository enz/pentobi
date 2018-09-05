//-----------------------------------------------------------------------------
/** @file libpentobi_base/Game.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GAME_H
#define LIBPENTOBI_BASE_GAME_H

#include "Board.h"
#include "BoardUpdater.h"
#include "NodeUtil.h"
#include "PentobiTree.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class Game
{
public:
    /** Determine a sensible value for the color to play at the current node.
        If the color was explicitly set with a setup property, it will be
        used. Otherwise, the effective color to play will be used, starting
        with the next color of the color of the last move (see
        Board::get_effective_to_play(Color))  */
    static Color get_to_play_default(const Game& game);


    explicit Game(Variant variant);


    void init(Variant variant);

    void init();

    /** Initialize game from a SGF tree.
        @note If the tree contains invalid properties, future calls to
        goto_node() might throw an exception.
        @param root The root node of the SGF tree; the ownership is transferred
        to this class.
        @throws SgfError if the root node contains invalid properties */
    void init(unique_ptr<SgfNode>& root);

    const Board& get_board() const;

    Variant get_variant() const;

    const SgfNode& get_current() const;

    const SgfNode& get_root() const;

    const PentobiTree& get_tree() const;

    /** Get the current color to play.
        Initialized with get_to_play_default() but may be changed with
        set_to_play(). */
    Color get_to_play() const;

    /** @param mv
        @param always_create_new_node Always create a new child of the current
        node even if a child with the move already exists. */
    void play(ColorMove mv, bool always_create_new_node);

    void play(Color c, Move mv, bool always_create_new_node);

    /** Update game state to a node in the tree.
        @throws SgfError if the game was constructed with an
        external SGF tree and the tree contained invalid property values
        (syntactically or semantically, like moves on occupied points). If an
        exception is thrown, the current node is not changed. */
    void goto_node(const SgfNode& node);

    /** Undo the current move and go to parent node.
        @pre get_tree().has_move(get_current())
        @pre get_current()->has_parent()
        @note Even if the implementation of this function calls goto_node(),
        it cannot throw an InvalidPropertyValue because the class Game ensures
        that the current node is always reachable via a path of nodes with
        valid move properties. */
    void undo();

    /** Set the current color to play.
        Does not store a player property in the tree or affect what color is to
        play when navigating away from and back to the current node. */
    void set_to_play(Color c);

    ColorMove get_move() const;

    /** Add final score to root node if the current node is in the main
        variation. */
    void set_result(int score);

    string get_charset() const;

    void set_charset(const string& charset);

    void remove_move_annotation(const SgfNode& node);

    double get_bad_move(const SgfNode& node) const;

    double get_good_move(const SgfNode& node) const;

    bool is_doubtful_move(const SgfNode& node) const;

    bool is_interesting_move(const SgfNode& node) const;

    void set_bad_move(const SgfNode& node, double value = 1);

    void set_good_move(const SgfNode& node, double value = 1);

    void set_doubtful_move(const SgfNode& node);

    void set_interesting_move(const SgfNode& node);

    string get_comment() const;

    void set_comment(const string& s);

    /** Delete the current node and its subtree and go to the parent node.
        @pre get_current().has_parent() */
    void truncate();

    void truncate_children();

    /** Replace the game tree by a new one that has the current position
        as a setup in its root node. */
    void keep_only_position();

    /** Like keep_only_position() but does not delete the children of the
        current node. */
    void keep_only_subtree();

    void make_main_variation();

    void move_up_variation();

    void move_down_variation();

    /** Delete all variations but the main variation.
        If the current node is not in the main variation it will be changed
        to the node as in libboardgame_sgf::back_to_main_variation() */
    void delete_all_variations();

    /** Make the current node the first child of its parent. */
    void make_first_child();

    void set_modified(bool is_modified = true);

    void clear_modified();

    bool is_modified() const;

    /** Set the AP property at the root node. */
    void set_application(const string& name, const string& version = "");

    string get_player_name(Color c) const;

    void set_player_name(Color c, const string& name);

    string get_date() const;

    void set_date(const string& date);

    void set_date_today();

    /** Get event info (standard property EV) from root node. */
    string get_event() const;

    void set_event(const string& event);

    /** Get round info (standard property RO) from root node. */
    string get_round() const;

    void set_round(const string& round);

    /** Get time info (standard property TM) from root node. */
    string get_time() const;

    void set_time(const string& time);

    bool has_setup() const;

    void add_setup(Color c, Move mv);

    void remove_setup(Color c, Move mv);

    /** See libpentobi_base::Tree::set_player() */
    void set_player(Color c);

    /** See libpentobi_base::Tree::remove_player() */
    void remove_player();

private:
    const SgfNode* m_current;

    unique_ptr<Board> m_bd;

    PentobiTree m_tree;

    BoardUpdater m_updater;

    void update(const SgfNode& node);
};

inline void Game::clear_modified()
{
    m_tree.clear_modified();
}

inline double Game::get_bad_move(const SgfNode& node) const
{
    return m_tree.get_bad_move(node);
}

inline const Board& Game::get_board() const
{
    return *m_bd;
}

inline string Game::get_comment() const
{
    return m_tree.get_comment(*m_current);
}

inline string Game::get_date() const
{
    return m_tree.get_date();
}

inline string Game::get_event() const
{
    return m_tree.get_event();
}

inline const SgfNode& Game::get_current() const
{
    return *m_current;
}

inline double Game::get_good_move(const SgfNode& node) const
{
    return m_tree.get_good_move(node);
}

inline ColorMove Game::get_move() const
{
    return m_tree.get_move(*m_current);
}

inline string Game::get_player_name(Color c) const
{
    return m_tree.get_player_name(c);
}

inline Color Game::get_to_play() const
{
    return m_bd->get_to_play();
}

inline string Game::get_round() const
{
    return m_tree.get_round();
}

inline const SgfNode& Game::get_root() const
{
    return m_tree.get_root();
}

inline string Game::get_time() const
{
    return m_tree.get_time();
}

inline const PentobiTree& Game::get_tree() const
{
    return m_tree;
}

inline bool Game::has_setup() const
{
    return libpentobi_base::has_setup(*m_current);
}

inline Variant Game::get_variant() const
{
    return m_bd->get_variant();
}

inline void Game::init()
{
    init(m_bd->get_variant());
}

inline bool Game::is_doubtful_move(const SgfNode& node) const
{
    return m_tree.is_doubtful_move(node);
}

inline bool Game::is_interesting_move(const SgfNode& node) const
{
    return m_tree.is_interesting_move(node);
}

inline bool Game::is_modified() const
{
    return m_tree.is_modified();
}

inline void Game::make_first_child()
{
    m_tree.make_first_child(*m_current);
}

inline void Game::make_main_variation()
{
    m_tree.make_main_variation(*m_current);
}

inline void Game::move_down_variation()
{
    m_tree.move_down(*m_current);
}

inline void Game::move_up_variation()
{
    m_tree.move_up(*m_current);
}

inline void Game::play(Color c, Move mv, bool always_create_new_node)
{
    play(ColorMove(c, mv), always_create_new_node);
}

inline void Game::remove_move_annotation(const SgfNode& node)
{
    m_tree.remove_move_annotation(node);
}

inline void Game::set_application(const string& name, const string& version)
{
    m_tree.set_application(name, version);
}

inline void Game::set_bad_move(const SgfNode& node, double value)
{
    m_tree.set_bad_move(node, value);
}

inline void Game::set_charset(const string& charset)
{
    m_tree.set_charset(charset);
}

inline void Game::set_comment(const string& s)
{
    m_tree.set_comment(*m_current, s);
}

inline void Game::set_date(const string& date)
{
    m_tree.set_date(date);
}

inline void Game::set_event(const string& event)
{
    m_tree.set_event(event);
}

inline void Game::set_date_today()
{
    m_tree.set_date_today();
}

inline void Game::set_doubtful_move(const SgfNode& node)
{
    m_tree.set_doubtful_move(node);
}

inline void Game::set_good_move(const SgfNode& node, double value)
{
    m_tree.set_good_move(node, value);
}

inline void Game::set_interesting_move(const SgfNode& node)
{
    m_tree.set_interesting_move(node);
}

inline void Game::set_modified(bool is_modified)
{
    m_tree.set_modified(is_modified);
}

inline void Game::set_player_name(Color c, const string& name)
{
    m_tree.set_player_name(c, name);
}

inline void Game::set_round(const string& round)
{
    m_tree.set_round(round);
}

inline void Game::set_time(const string& time)
{
    m_tree.set_time(time);
}

inline void Game::truncate_children()
{
    m_tree.remove_children(*m_current);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GAME_H
