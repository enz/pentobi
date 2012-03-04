//-----------------------------------------------------------------------------
/** @file pentobi/SettingsDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SettingsDialog.h"

//-----------------------------------------------------------------------------

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    QSettings settings;
    setWindowTitle(tr("Pentobi - Settings"));
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    m_showNoMovesMessage =
        new QCheckBox(tr("Show message if a color has no more moves"));
    m_showNoMovesMessage->setChecked(
                       settings.value("show_no_moves_message", true).toBool());
    layout->addWidget(m_showNoMovesMessage);
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    buttonBox->setFocus();
}

void SettingsDialog::accept()
{
    QSettings settings;
    settings.setValue("show_no_moves_message",
                      m_showNoMovesMessage->isChecked());
    QDialog::accept();
}

//-----------------------------------------------------------------------------
