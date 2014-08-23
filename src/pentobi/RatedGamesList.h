//-----------------------------------------------------------------------------
/** @file pentobi/RatedGamesList.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATED_GAMES_LIST
#define PENTOBI_RATED_GAMES_LIST

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QTableView>
#include "RatingHistory.h"

class QStandardItemModel;

//-----------------------------------------------------------------------------

class RatedGamesList
    : public QTableView
{
    Q_OBJECT

public:
    explicit RatedGamesList(QWidget* parent = nullptr);

    void updateContent(Variant variant, const RatingHistory& history);

signals:
    void openRatedGame(unsigned n);

protected:
    void focusInEvent(QFocusEvent* event) override;

    void focusOutEvent(QFocusEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    QStandardItemModel* m_model;

private slots:
    void activateGame(const QModelIndex& index);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATED_GAMES_LIST
