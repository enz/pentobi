//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_DIALOG_H
#define PENTOBI_RATING_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
    void openRecentFile(const QString& file);

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
    void activateGame(unsigned n);

    void buttonClicked(QAbstractButton* button);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_DIALOG_H
