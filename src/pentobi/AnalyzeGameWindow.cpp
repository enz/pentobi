//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AnalyzeGameWindow.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------

AnalyzeGameWindow::AnalyzeGameWindow(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Analyze Game"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    analyzeGameWidget = new AnalyzeGameWidget(this);
    layout->addWidget(analyzeGameWidget);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    buttonBox->setFocus();
}

//-----------------------------------------------------------------------------
