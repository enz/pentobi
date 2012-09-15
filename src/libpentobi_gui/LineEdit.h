//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LineEdit.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_LINE_EDIT_H
#define LIBPENTOBI_GUI_LINE_EDIT_H

#include <QLineEdit>

//-----------------------------------------------------------------------------

/** QLineEdit with a configurable size hint depending on the expected
    number of characters. */
class LineEdit
    : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget* parent, int nuCharactersHint);

    QSize sizeHint() const;

private:
    int m_nuCharactersHint;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_LINE_EDIT_H
