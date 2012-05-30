//-----------------------------------------------------------------------------
/** @file libpentobi_gui/RatingDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_DIALOG_H
#define PENTOBI_RATING_DIALOG_H

#include <QtGui>
#include "RatingGraph.h"
#include "libpentobi_base/GameVariant.h"

using namespace std;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

class RatingDialog
    : public QDialog
{
    Q_OBJECT

public:
    RatingDialog(QWidget* parent, GameVariant variant);

    void setHistory(const RatingHistory& history);

private:
    RatingGraph* m_graph;
};

inline void RatingDialog::setHistory(const RatingHistory& history)
{
    m_graph->setHistory(history);
}

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
