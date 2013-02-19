//-----------------------------------------------------------------------------
/** @file pentobi/Application.h */
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
    Application(int& argc, char** argv);

    /** Reimplemented from QApplication::notify().
        Catches exceptions and shows an error message. */
    bool notify(QObject* receiver, QEvent* event);

private:
    void showError(const QString& detailedText);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_APPLICATION_H
