Pentobi GTP Interface
=====================

This document describes the text-based interface to the engine of the
Blokus program [Pentobi](https://pentobi.sourceforge.io). The interface
is an adaption of the
[Go Text Protocol](https://www.lysator.liu.se/~gunnar/gtp/) (GTP) and
allows controller programs to use the engine in an automated way without
the GUI. The most recent version of this document can be found in the
source code distribution of Pentobi.

Go Text Protocol
----------------

The Go Text Protocol is a simple text-based protocol. The engine reads
single-line commands from its standard input stream and writes
multi-line responses to its standard output stream. The first character
of a response is a status character: `=` for success, `?` for failure,
followed by the actual response. The response ends with two consecutive
newline characters. See the
[GTP specification](https://www.lysator.liu.se/~gunnar/gtp/gtp2-spec-draft2/gtp2-spec.html)
for details.

Controllers
-----------

To use the engine from a controller program, the controller typically
creates a child process by running `pentobi-gtp` and then sends commands
and receives responses through the input/output streams of the child
process. How this is done depends on the platform (programming language
and/or operating system). In Java, for example, a child process can be
created with `java.lang.Runtime.exec()`.

Note that the input/output streams of child processes are often fully
buffered. You should explicitly flush the output stream after sending a
command. Another caveat is that `pentobi-gtp` writes debugging
information to its standard error stream. On some platforms the standard
error stream of the child process is automatically connected to the
standard error stream of its parent process. If not (this happens for
example in Java), the controller needs to read everything from the
standard error stream of the child process. This can be done for example
by running a separate thread in the parent process that has a simple
read loop, which writes everything that it reads to its own standard
error stream or discards it. Otherwise the child process will block as
soon as the buffer for its standard error stream is full. Alternatively,
you can disable debugging output of `pentobi-gtp` with the command line
option `--quiet`, but it is generally better to assume that a GTP engine
writes text to standard error.

An example for a controller written in C++ for Linux is included in
Pentobi since version 9.0 in `twogtp`. The controller starts two GTP
engines and plays a number of Blokus games between them. Older versions
of Pentobi included a Python script with a similar functionality in
`tools/twogtp/twogtp.py`.

Building
--------

Since the GTP engine is a developer tool, building it is not enabled by
default. To enable it, run `cmake` with the option
`-DPENTOBI_BUILD_GTP=ON`. After building, there will be an executable in
the build directory named `pentobi_gtp/pentobi-gtp`. The GTP engine
requires only standard C++ and has no dependency on other libraries like
Qt, which is needed for the GUI version of Pentobi. If you only want to
build the GTP engine, you can disable building the GUI with
`-DPENTOBI_BUILD_GUI=OFF`.

Options
-------

The following command-line options are supported by `pentobi-gtp`:

`--book` _file_

Specify a file name for the opening book. Opening books are blksgf files
containing trees, in which moves that Pentobi should select are marked
as good moves with the corresponding SGF property (see the files in
`opening_books`). If no opening book is specified and opening books are
not disabled, `pentobi-gtp` will automatically search for an opening
book for the current game variant in the directory of the executable
using the same file name conventions as in `opening_books`. If no such
file is found it will print an error message to standard error and
disable the use of opening books.

`--config,-c` _file_

Load a file with GTP commands and execute them before starting the main
loop, which reads commands from standard input. This can be used for
configuration files that contain GTP commands for setting parameters of
the engine (see below).

`--color`

Use ANSI escape sequences to colorize the text output of boards (for
example in the response to the `showboard` command or with the
--showboard command line option).

`--game,-g` _variant_

Specify the game variant used at start-up. Valid arguments are classic,
classic_2, duo, trigon, trigon_2, trigon_3, junior, nexos, nexos_2,
gembloq, gembloq_2, gembloq_3, gembloq_2_4, callisto, callisto_2,
callisto_3, callisto_2_4 or the abbreviations c, c2, d, t, t2, t3, j, n,
n2, g, g2, g3, g24, ca, ca2, ca3, ca24. By default, the initial game
variant is classic. The game variant can also be changed at run-time
with a GTP command. If only a single game variant is used, it is
slightly faster and saves memory if the engine is started in the right
variant compared to having it start with classic and then changing it.

`--help,-h`

Print a list of the command-line options and exit.

`--level,-l` _n_

Set the level of playing strength to n. Valid values are 1 to 9.

`--seed,-r` _n_

Use _n_ as the seed for the random generator. Specifying a random seed
will make the move generation deterministic as long as the search is
single-threaded.

`--showboard`

Automatically write a text representation of the current position to
standard error after each command that alters the position.

`--nobook`

Disable the use of opening books.

`--noresign`

Disable resignation. If resignation is disabled, the `genmove` command
will never respond with `resign`. Resignation can speed up the playing
of test games if only the win/loss information is wanted.

`--quiet,-q`

Do not print any debugging messages, errors or warnings to standard
error.

`--threads` _n_

Use _n_ threads during the search. Note that the default is 1, unlike
in the GUI version of Pentobi, which sets the default according to the number
of hardware threads (CPUs, cores or virtual cores) available on the current
system. The reason is that, for example, using 2 threads makes the search twice
as fast but may lose a bit of playing strength compared to the single-threaded
search. Therefore, if the GTP engine is used to play many test games with
twogtp (which supports playing games in parallel), it is better to play the
games with single-threaded search in parallel than with multi-threaded search
sequentially. Using a large number of threads (e.g. more than 8) is untested
and might reduce the playing strength compared to the single-threaded
search.

`--version,-v`

Print the version of Pentobi and exit.

Standard Commands
-----------------

The following GTP commands have the same or an equivalent meaning as
specified by the GTP standard. Colors or players in arguments or
responses are represented as in the property IDs of blksgf files (`B`,
`W` if two colors; `1`, `2`, `3`, `4` if more than two). Moves in
arguments or responses are represented as in the move property values
of blksgf files. See the specification for
[Pentobi SGF files](../libpentobi_base/Pentobi-SGF.md) for details.

`all_legal` _color_

List all legal moves for a color.

`clear_board`

Clear the board and start a new game in the current game variant.

`final_score`

Get the score of a final board position. In two-player game variants,
the format of the response is as in the result property in the SGF
standard for the game of Go (e.g. `B+2` if the first player wins with
two points, or `0` for a draw). In game variants with more than two
players, the response is a list of the points for each player (e.g.
`64 69 70 40`). If the current position is not a final position, the
response is undefined.

`genmove` _color_

Generate and play a move for a given color in the current position. If
the color has no more moves, the response is `pass`. If resignation is
not disabled, the response is `resign` if the player is very likely to
lose. Otherwise the response is the move.

`known_command` _command_

The response is `true` if _command_ is a GTP command supported
by the engine, `false` otherwise.

`list_commands`

List all supported GTP commands, one command per line.

`loadsgf` _file_ [_move_number_]

Load a board position from a blksgf file with name _file_. If
_move_number_ is specified, the board position will be set to the
position in the main variation of the file before the move with
the given number was played, otherwise to the last position in the main
variation.

`name`

Return the name of the GTP engine (`Pentobi`).

`play` _color_ _move_

Play a move for a given color in the current board position.

`quit`

Exit the command loop and quit the engine.

`reg_genmove` _color_

Like the `genmove` command, but only generates a move and does not
play it on the board.

`showboard`

Return a text representation of the current board position.

`undo`

Undo the last move played.

`version`

Return the version of Pentobi.

Generally Useful Extension Commands
-----------------------------------

`cputime`

Return the CPU time used by the engine since the start of the program.

`g`

Shortcut for the `genmove` command with the color argument set to
the current color to play.

`get_place` _color_

Get the place of a given color in the list of scores in a final position
(e.g. in game variant Classic, 1 is the place with the highest score,
4 the one with the lowest, if all players have a different score). If
some colors have the same score, they share the same place and the
string `shared` is appended to the place number.

`get_value`

Get an estimated value of the board position from the view point of the
color of the last generated move. The return value is a win/loss
estimation between 0 (loss) and 1 (win) as produced by the last search
performed by the engine. This command should only be used immediately
after a `reg_genmove` or `genmove` command, otherwise the result is
undefined. The value is not very meaningful at the lowest playing
levels. Note that no searches are performed if the opening book is used
for a move generation and there is currently no way to check if this was
so. Therefore, the opening book should be disabled if the `get_value`
command is used.

`p` _move_

Shortcut for the `play` command with the color argument set to the
current color to play.

`param` [_key_ _value_]

Set or query parameters specific to the Pentobi engine that can be
changed at run-time. If no arguments are given, the response is a list
of the current value with one key/value pair per line, otherwise the
parameter with the given key will be set to the given value. Generally
useful parameters are:

`param avoid_symmetric_draw 0|1`
In some game variants (Duo, Trigon_2), the second player can enforce a
tie by answering each move by its symmetric counterpart if the first
players misses the opportunity to break the symmetry in the center.
Technically, exploiting this mistake by the first player is a good
strategy for the second player because a draw is a good result
considering the first-play advantage. However, playing symmetrically
could be considered bad style, so this behavior is avoided (value `1`)
by default.

`param fixed_simulations` _n_
Use exactly _n_ MCTS simulations during a search. By default, the
search engine uses levels, which determine how many MCTS simulations are
run during a search, but as a function that increases with the move
number (because the simulations become much faster at the end of the
game). For some experiments, it can be desirable to use a fixed number
of simulations for each move. If this number is specified, the playing
level is ignored.

`param use_book 0|1`
Enable or disable the opening book.

The other parameters are only interesting for developers.

`param_base` [_key_ _value_]

Set or query basic parameters that are not specific to the Pentobi
engine. If no arguments are given, the response is a list of the current
value with one key/value pair per line, otherwise the parameter with the
given key will be set to the given value.

`param_base accept_illegal 0|1`
Accept move arguments to the `play` command that violate the rules
of the game. If disabled, the `play` command will respond with an error,
otherwise it will perform the moves.

`param_base resign 0|1`
Allow the engine to respond with `resign` to the `genmove` command.

`set_game` _variant_

Set the current game variant and clear the board. The argument is the
name of the game variant as in the game property value of blksgf files
(e.g. `Blokus Duo`, see the specification for
[Pentobi SGF files](../libpentobi_base/Pentobi-SGF.md) for details).

`set_random_seed` _n_

Set the seed of the random generator to _n_. See the documentation for
the command-line option --seed.

Extension Commands for Developers
---------------------------------

The remaining commands are only interesting for developers. See
Pentobi's source code for details.

Example
-------

The following GTP session queries the engine name and version, plays and
generates a move in game variant Duo and shows the resulting board
position.
```
$ ./pentobi-gtp --quiet
name
= Pentobi

version
= 7.1

set_game Blokus Duo
=

play b e8,d9,e9,f9,e10
=

genmove w
= i4,h5,i5,j5,i6

showboard
=
   A B C D E F G H I J K L M N
14 . . . . . . . . . . . . . . 14  *Blue(X): 5
13 . . . . . . . . . . . . . . 13  1 F L5 N P T5 U V5 W Y
12 . . . . . . . . . . . . . . 12  Z5 I5 O T4 Z4 L4 I4 V3 I3 2
11 . . . . . . . . . . . . . . 11
10 . . . . X . . . . . . . . . 10  Green(O): 5
 9 . . . X X X . . . . . . . . 9   1 F L5 N P T5 U V5 W Y
 8 . . . . X . . . . . . . . . 8   Z5 I5 O T4 Z4 L4 I4 V3 I3 2
 7 . . . . . . . . . . . . . . 7
 6 . . . . . . . .>O . . . . . 6
 5 . . . . . . . O O O . . . . 5
 4 . . . . . . . . O . . . . . 4
 3 . . . . . . . . . . . . . . 3
 2 . . . . . . . . . . . . . . 2
 1 . . . . . . . . . . . . . . 1
   A B C D E F G H I J K L M N

quit
=
```
