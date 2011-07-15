//-----------------------------------------------------------------------------
/** @file libpentobi_gui/SameHeightLayout.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SameHeightLayout.h"

using namespace std;

//-----------------------------------------------------------------------------

SameHeightLayout::SameHeightLayout(QWidget* parent)
    : QLayout(parent)
{
}

SameHeightLayout::~SameHeightLayout()
{
    QLayoutItem* item;
    while ((item = takeAt(0)))
        delete item;
}

void SameHeightLayout::addItem(QLayoutItem* item)
{
    m_list.append(item);
}

QSize SameHeightLayout::sizeHint() const
{
    QSize s(0, 0);
    int count = m_list.count();
    int i = 0;
    while (i < count)
    {
        QSize size = m_list.at(i)->sizeHint();
        s.setWidth(max(size.width(), s.width()));
        s.setHeight(s.height() + size.height());
        ++i;
    }
    return s + (count - 1) * QSize(0, spacing());
}

QSize SameHeightLayout::minimumSize() const
{
    QSize s(0, 0);
    int count = m_list.count();
    int i = 0;
    while (i < count)
    {
        QSize size = m_list.at(i)->minimumSize();
        s.setWidth(max(size.width(), s.width()));
        s.setHeight(s.height() + size.height());
        ++i;
    }
    return s + (count - 1) * QSize(0, spacing());
}

int SameHeightLayout::count() const
{
    return m_list.size();
}

QLayoutItem* SameHeightLayout::itemAt(int i) const
{
    return m_list.value(i);
}

QLayoutItem* SameHeightLayout::takeAt(int i)
{
    return i >= 0 && i < m_list.size() ? m_list.takeAt(i) : 0;
}

void SameHeightLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    if (m_list.size() == 0)
        return;
    int count = m_list.count();
    int width = rect.width();
    int height = (rect.height() - (count - 1) * spacing()) / count;
    int x = rect.x();
    int y = rect.y();
    for (int i = 0; i < count; ++i)
    {
        QRect geom(x, y, width, height);
        m_list.at(i)->setGeometry(geom);
        y = y + height + spacing();
    }
}

//-----------------------------------------------------------------------------
