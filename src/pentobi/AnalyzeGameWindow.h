//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGameWindow.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_GAME_WINDOW_H
#define PENTOBI_ANALYZE_GAME_WINDOW_H

#include <vector>
#include <QtGui>
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
