Pentobi SGF Files
=================

This document describes the file format for
[Blokus](https://en.wikipedia.org/wiki/Blokus) game records as used by
the program [Pentobi](https://pentobi.sourceforge.io). The most recent
version of this document can be found in the source code distribution of
Pentobi.

Introduction
------------

The file format is a derivative of the
[Smart Game Format](https://www.red-bean.com/sgf/) (SGF). The current
SGF version 4 does not define standard properties for Blokus. Therefore,
a number of game-specific properties and value types had to be defined.
The definitions follow the recommendations of SGF 4 and the proposals
for multi-player games from the
[discussions](http://www.red-bean.com/sgf/ff5/ff5.htm) about the future
SGF version 5.

### Note
Older versions of Pentobi (up to version 13.1) did not accept
whitespaces before and after property identifiers, so it is recommended
to avoid them for compatibility.

File Extension and MIME Type
----------------------------

The file extension `.blksgf` and the
[MIME type](https://en.wikipedia.org/wiki/Internet_media_type)
`application/x-blokus-sgf` are used for Blokus SGF files.

### Note
Since this is a non-standard MIME type, links to Blokus SGF files on web
servers will not automatically open the file with Pentobi even if
Pentobi is installed locally and registered as a handler for Blokus SGF
files. To make this work, you can put a file named
[`.htaccess`](https://en.wikipedia.org/wiki/.htaccess) on the web server
in the same directory that contains the `.blksgf` files or in one of its
parent directories. This file needs to contain the line:
```
AddType application/x-blokus-sgf blksgf
```

Character Set
-------------

Only UTF-8 is supported as the character set. Files without a `CA`
property are interpreted as UTF-8. This differs from the original SGF
standard, which specified ISO-8859-1 (Latin1) as the default.

### Note
Pentobi has always written files with UTF-8 encoding and indicated that
with the `CA[UTF-8]` property. Some earlier versions of Pentobi
supported loading files with different encodings and assumed
ISO-8859-1 for files without a `CA` property. Pentobi versions before
13.0 could read SGF files encoded in UTF-8 or ISO-8859-1. Versions
before 19.0 could read all encodings handled by the Qt 5 library.
Starting with Pentobi 19.0, support for encodings other than UTF-8 was
dropped.

Game Property
-------------

Since there is no number for Blokus defined in SGF 4, a string instead
of a number is used as the value for the `GM` property. Currently, the
following strings are used:

* `Blokus`
* `Blokus Two-Player`
* `Blokus Three-Player`
* `Blokus Duo`
* `Blokus Trigon`
* `Blokus Trigon Two-Player`
* `Blokus Trigon Three-Player`
* `Blokus Junior`
* `Nexos`
* `Nexos Two-Player`
* `Callisto`
* `Callisto Two-Player`
* `Callisto Two-Player Four-Color`
* `Callisto Three-Player`
* `GembloQ`
* `GembloQ Two-Player`
* `GembloQ Three-Player`
* `GembloQ Two-Player Four-Color`

The strings are case-sensitive, words must be separated by exactly one
space and must not contain whitespaces at the beginning or end of the
string.

### Note
Although the SGF standard does not require an ordering of properties,
it is recommended to put the `GM` property at the beginning because
Pentobi's automatic MIME type detection looks only at the beginning
of files.

Color and Player Properties
---------------------------

In game variants with two players and two colors, `B` denotes the first
player or color, `W` the second player or color. In game variants with
three or four players and one color per player, `1`, `2`, `3`, `4`
denote the first, second, third, and fourth player or color. In game
variants with two players and four colors, `B` denotes the first player,
`W` the second player, and `1`, `2`, `3`, `4` denote the first, second,
third, and fourth color. This applies to move properties and properties
related to a player or a color.

### Example 1
In the game variant Blokus Two-Player `PB` is the name of the first
player, and `1` is a move of the first color.

### Example 2
In the game variant Blokus Two-Player, one could either use the `BL`,
`WL` properties to indicate the time left for a player, if the game is
played with a time limit for each player, or one could use the `1L`,
`2L`, `3L`, `4L` properties to indicate the time left for a color, if
the game is played with a time limit for each color. (This is only an
example how the properties should be interpreted. Pentobi currently has
no support for game clocks.)

### Note
Pentobi versions before 0.2 used the properties `BLUE`, `YELLOW`, `RED`,
`GREEN` in the four-color game variants, which did not reflect the
current state of discussion for SGF 5. Pentobi 12.0 erroneously used
multi-player properties for two-player Callisto. Current versions of
Pentobi can still read games written by older versions and will convert
old properties.

Coordinate System
-----------------

Fields on the board (called points in SGF) are identified by a
case-insensitive string with a letter for the column followed by a
number for the row. The letters start with 'a', the numbers start with
'1'. The lower left corner of the board is 'a1'. The strings must not
contain whitespaces. Note that, unlike the common convention in the game
of Go, the letter 'i' is used.

If there are more than 26 columns, the columns continue with 'aa', 'ab',
..., 'ba', 'bb', ... More than 26 columns are presently required for
Trigon and GembloQ.

For Trigon, hexagonal boards are mapped to rectangular coordinates as in
the following example of a hexagon with edge size 3:
```
6     / \ / \ / \ / \
5   / \ / \ / \ / \ / \
4 / \ / \ / \ / \ / \ / \
3 \ / \ / \ / \ / \ / \ /
2   \ / \ / \ / \ / \ /
1     \ / \ / \ / \ /
   a b c d e f g h i j k
```

In Nexos, the 13×13 line grid is mapped to a 25×25 coordinate system, in
which rows with horizontal line segments and intersections alternate
with rows with vertical line segments and holes:
```
6 |   |   |
5 + - + - + -
4 |   |   |
3 + - + - + -
2 |   |   |
1 + - + - + -
  a b c d e f
```

In GembloQ, each square field is divided into four triangles with their
own coordinates, like in this example:
```
4 | / | \ | / | \ | /
3 | \ | / | \ | / | \
2 | / | \ | / | \ | /
1 | \ | / | \ | / | \
   a b c d e f g h i
```

Move Properties
---------------

The value of a move property is a string with the coordinates of the
played piece on the board separated by commas. No whitespace characters
are allowed before, after, or in-between the coordinates.

Pentobi currently does not require a certain order of the coordinates of
a move. However, move properties should be written with an ordered list
of coordinates (using the order a1, b1, …, a2, b2, …) such that each
move has a unique string representation.

### Example
`B[f9,e10,f10,g10,f11]`

In Nexos, moves contain only the coordinates of line segments occupied
by the piece, no coordinates of junctions.

### Note
Old versions of Pentobi (before version 0.3) used to represent moves by
a list of points, which did not follow the convention used by other
games in SGF to use single-value properties for moves. Current versions
of Pentobi can still read games containing the old move property values
but they are deprecated and should no longer be used.

Setup Properties
----------------

The setup properties `AB`, `AW`, `A1`, `A2`, `A3`, `A4` can be used to
place several pieces simultaneously on the board. The setup property
`AE` can be used to remove pieces from the board. All these properties
can have multiple values, each value represents a piece by its
coordinates as in the move properties. The `PL` can be used to set the
color to play in a setup position.

### Example
```
AB[e8,e9,f9,d10,e10][g6,f7,g7,h7,g8]
AW[i4,h5,i5,j5,i6][j7,j8,j9,k9,j10]
PL[B]
```

### Note
Older versions of Pentobi (before version 2.0) did not support setup
properties, you need a newer version of Pentobi to read such files.
Currently, Pentobi is able to read files with setup properties in any
node, but can create only files with setup in the root node.
