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
    RatingDialog(QWidget* parent);

    void updateContent(GameVariant variant, const RatingHistory& history);

private:
    QLabel* m_labelVariant;

    QLabel* m_labelNuGames;

    QLabel* m_labelRating;

    RatingGraph* m_graph;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
