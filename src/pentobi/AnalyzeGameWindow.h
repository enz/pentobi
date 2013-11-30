//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_GAME_WINDOW_H
#define PENTOBI_ANALYZE_GAME_WINDOW_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
#include <QDialog>
#include "AnalyzeGameWidget.h"

using namespace std;

//-----------------------------------------------------------------------------

class AnalyzeGameWindow
    : public QDialog
{
    Q_OBJECT

public:
    AnalyzeGameWidget* analyzeGameWidget;

    AnalyzeGameWindow(QWidget* parent);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_GAME_WINDOW_H
