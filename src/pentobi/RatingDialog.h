//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_DIALOG_H
#define PENTOBI_RATING_DIALOG_H

#include <QDialog>
#include "RatedGamesList.h"
#include "RatingGraph.h"
#include "libpentobi_base/Variant.h"

class QLabel;

using namespace std;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class RatingDialog
    : public QDialog
{
    Q_OBJECT

public:
    RatingDialog(QWidget* parent);

    void updateContent(Variant variant, const RatingHistory& history);

signals:
    void openRatedGame(Variant variant, unsigned int n);

private:
    Variant m_variant;

    QLabel* m_labelVariant;

    QLabel* m_labelNuGames;

    QLabel* m_labelRating;

    QLabel* m_labelBestRating;

    RatingGraph* m_graph;

    RatedGamesList* m_list;

    void updateLabels(Rating rating, unsigned int nuGames, Rating bestRating);

private slots:
    void activateGame(unsigned int n);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
