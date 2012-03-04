//-----------------------------------------------------------------------------
/** @file pentobi/SettingsDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_SETTINGS_DIALOG_H
#define PENTOBI_SETTINGS_DIALOG_H

#include <QtGui>

//-----------------------------------------------------------------------------

class SettingsDialog
    : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent);

public slots:
    void accept();

private:
    QCheckBox* m_showNoMovesMessage;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_SETTINGS_DIALOG_H
