//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeSpeedDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeSpeedDialog.h"

//-----------------------------------------------------------------------------

AnalyzeSpeedDialog::AnalyzeSpeedDialog(QWidget* parent, const QString& title)
    : QInputDialog(parent),
      m_speedValue(0)
{
    m_items << tr("Fast") << tr("Normal") << tr("Slow");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(title);
    setLabelText(tr("Analysis speed:"));
    setInputMode(QInputDialog::TextInput);
    setComboBoxItems(m_items);
    setComboBoxEditable(false);
}

void AnalyzeSpeedDialog::accept()
{
    QString item = textValue();
    if (item.isEmpty())
        return;
    m_speedValue = m_items.indexOf(item);
    QDialog::accept();
}

//-----------------------------------------------------------------------------
