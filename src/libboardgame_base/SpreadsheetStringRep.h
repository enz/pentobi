//-----------------------------------------------------------------------------
/** @file libboardgame_base/SpreadsheetStringRep.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_SPREADSHEET_STRING_REP_H
#define LIBBOARDGAME_BASE_SPREADSHEET_STRING_REP_H

#include <climits>
#include <iosfwd>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Spreadsheet-style string representation of points.
    Can be used as a template argument for libboardgame_base::Point.
    Columns are represented as letters including the letter 'J'. After 'Z',
    multi-letter combinations are used: 'AA', 'AB', etc. Rows are represented
    by numbers starting with '1'. Note that unlike in spreadsheets, row number
    1 is at the bottom to be compatible with the coordinate convention used
    in class Point. */
struct SpreadsheetStringRep
{
    static const unsigned int max_width = UINT_MAX;

    static const unsigned int max_height = UINT_MAX;

    static bool read(istream& in, unsigned int width, unsigned int height,
                     unsigned int& x, unsigned int& y);

    static void write(ostream& out, unsigned int x, unsigned int y);
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_SPREADSHEET_STRING_REP_H
