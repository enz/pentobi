//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LineEdit.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LineEdit.h"

//-----------------------------------------------------------------------------

LineEdit::LineEdit(QWidget* parent, int nuCharactersHint)
    : QLineEdit(parent),
      m_nuCharactersHint(nuCharactersHint)
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

