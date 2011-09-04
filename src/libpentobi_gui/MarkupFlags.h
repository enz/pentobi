//-----------------------------------------------------------------------------
/** @file libpentobi_gui/MarkupFlags.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_MARKUP_FLAGS_H
#define LIBPENTOBI_GUI_MARKUP_FLAGS_H

#include <bitset>

//-----------------------------------------------------------------------------

enum MarkupFlag
{
    markup_variation_triangle,

    _nu_markup_flags
};

//-----------------------------------------------------------------------------

typedef std::bitset<_nu_markup_flags> MarkupFlags;

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_MARKUP_FLAGS_H
