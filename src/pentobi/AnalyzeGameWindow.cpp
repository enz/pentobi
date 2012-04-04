//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGameWindow.h"

//-----------------------------------------------------------------------------

AnalyzeGameWindow::AnalyzeGameWindow(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Pentobi - Game Analysis"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    analyzeGameWidget = new AnalyzeGameWidget(this);
    layout->addWidget(analyzeGameWidget);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    // Standard button "Close" has reject role
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    buttonBox->setFocus();
}

//-----------------------------------------------------------------------------
