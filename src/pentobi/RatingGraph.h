//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_GRAPH_H
#define PENTOBI_RATING_GRAPH_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QFrame>
#include "RatingHistory.h"

//-----------------------------------------------------------------------------

class RatingGraph
    : public QFrame
{
    Q_OBJECT

public:
    explicit RatingGraph(QWidget* parent = nullptr);

    void updateContent(const RatingHistory& history);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_yMin;

    double m_yMax;

    vector<double> m_values;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_GRAPH_H
