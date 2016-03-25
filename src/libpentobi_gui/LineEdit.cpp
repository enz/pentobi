//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LineEdit.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LineEdit.h"

#include <QApplication>

//-----------------------------------------------------------------------------

LineEdit::LineEdit(int nuCharactersHint)
    : m_nuCharactersHint(nuCharactersHint)
{
}

QSize LineEdit::sizeHint() const
{
    QFont font = QApplication::font();
    QFontMetrics metrics(font);
    QSize size = QLineEdit::sizeHint();
    size.setWidth(m_nuCharactersHint * metrics.averageCharWidth());
    return size;
}

//-----------------------------------------------------------------------------

