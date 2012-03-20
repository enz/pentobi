//-----------------------------------------------------------------------------
/** @file pentobi/Application.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_APPLICATION_H
#define PENTOBI_APPLICATION_H

#include <QtGui>

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
