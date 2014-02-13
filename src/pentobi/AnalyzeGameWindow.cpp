//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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
    setWindowTitle(tr("Game Analysis"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto layout = new QVBoxLayout;
    setLayout(layout);
    analyzeGameWidget = new AnalyzeGameWidget(this);
    layout->addWidget(analyzeGameWidget);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    buttonBox->setFocus();
}

//-----------------------------------------------------------------------------
