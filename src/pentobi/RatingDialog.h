//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_DIALOG_H
#define PENTOBI_RATING_DIALOG_H

#include <QDialog>
#include "RatedGamesList.h"
#include "RatingGraph.h"
#include "libpentobi_base/Variant.h"

class QAbstractButton;
class QLabel;

using namespace std;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class RatingDialog
    : public QDialog
{
    Q_OBJECT

public:
    /** Constructor.
        @param parent
        @param history (@ref libboardgame_doc_storesref) */
    RatingDialog(QWidget* parent, RatingHistory& history);

    void updateContent();

signals:
    void open(const QString& file);

private:
    RatingHistory& m_history;

    QPushButton* m_clearButton;

    QLabel* m_labelVariant;

    QLabel* m_labelNuGames;

    QLabel* m_labelRating;

    QLabel* m_labelBestRating;

    RatingGraph* m_graph;

    RatedGamesList* m_list;

private slots:
    void activateGame(unsigned int n);

    void buttonClicked(QAbstractButton*);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
