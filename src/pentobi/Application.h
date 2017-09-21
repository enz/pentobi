//-----------------------------------------------------------------------------
/** @file pentobi/Application.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_APPLICATION_H
#define PENTOBI_APPLICATION_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QApplication>

//-----------------------------------------------------------------------------

class Application
    : public QApplication
{
    Q_OBJECT

public:
    using QApplication::QApplication;

    /** Reimplemented from QApplication::notify().
        Catches exceptions and shows an error message. */
    bool notify(QObject* receiver, QEvent* event) override;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_APPLICATION_H
