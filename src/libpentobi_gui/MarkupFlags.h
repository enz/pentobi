//-----------------------------------------------------------------------------
/** @file libpentobi_gui/MarkupFlags.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_MARKUP_FLAGS_H
#define LIBPENTOBI_GUI_MARKUP_FLAGS_H

#include <bitset>

//-----------------------------------------------------------------------------

enum MarkupFlag
{
    /** Visual indication that a move has variations.
        Currently only visible if the point has a (move number) label, in which
        case the label is underlined. */
    markup_variation,

    _nu_markup_flags
};

//-----------------------------------------------------------------------------

typedef std::bitset<_nu_markup_flags> MarkupFlags;

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_MARKUP_FLAGS_H
