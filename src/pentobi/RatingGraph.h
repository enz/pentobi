//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_GRAPH_H
#define PENTOBI_RATING_GRAPH_H

#include <QtGui>
#include "RatingHistory.h"

//-----------------------------------------------------------------------------

class RatingGraph
    : public QWidget
{
    Q_OBJECT

public:
    RatingGraph(QWidget* parent = 0);

    void setHistory(const RatingHistory& history);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    float m_yMin;

    float m_yMax;

    vector<float> m_values;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_GRAPH_H
