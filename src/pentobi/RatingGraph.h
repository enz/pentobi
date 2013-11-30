//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
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
    RatingGraph(QWidget* parent = nullptr);

    void updateContent(const RatingHistory& history);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    float m_yMin;

    float m_yMax;

    vector<float> m_values;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_GRAPH_H
