//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_HELP_WINDOW_H
#define LIBPENTOBI_GUI_HELP_WINDOW_H

#include <QtGui>

//-----------------------------------------------------------------------------

class HelpWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    HelpWindow(QWidget* parent, const QString& mainPage);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_HELP_WINDOW_H
