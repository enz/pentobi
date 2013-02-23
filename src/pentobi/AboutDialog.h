//-----------------------------------------------------------------------------
/** @file pentobi/AboutDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ABOUT_DIALOG_H
#define PENTOBI_ABOUT_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QDialog>

class QAbstractButton;

//-----------------------------------------------------------------------------

class AboutDialog
    : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent, const QString& version);

private:
    QPushButton* m_donateButton;

    QPushButton* m_flattrButton;

private slots:
    void buttonClicked(QAbstractButton*);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ABOUT_DIALOG_H
