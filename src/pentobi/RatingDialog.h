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

signals:
    void openRatedGame(GameVariant variant, unsigned int n);

private:
    GameVariant m_variant;

    QLabel* m_labelVariant;

    QLabel* m_labelNuGames;

    QLabel* m_labelRating;

    RatingGraph* m_graph;

    QTableView* m_table;

    QStandardItemModel* m_model;

private slots:
    void gameClicked(const QModelIndex& index);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
