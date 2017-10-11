//-----------------------------------------------------------------------------
/** @file libpentobi_gui/SameHeightLayout.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SameHeightLayout.h"

#include <QStyle>
#include <QWidget>

using namespace std;

//-----------------------------------------------------------------------------

SameHeightLayout::~SameHeightLayout()
{
    QLayoutItem* item;
    while ((item = takeAt(0)) != nullptr)
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
    return s + (count - 1) * QSize(0, getSpacing());
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
    return s + (count - 1) * QSize(0, getSpacing());
}

int SameHeightLayout::count() const
{
    return m_list.size();
}

int SameHeightLayout::getSpacing() const
{
    // spacing() returns -1 with Qt 4.7 on KDE. It returns 6 on Gnome. Is this a
    // bug? The documentation says: "If no value is explicitly set, the layout's
    // spacing is inherited from the parent layout, or from the style settings
    // for the parent widget."
    int result = spacing();
    if (result < 0 && (parentWidget() != nullptr))
        result = parentWidget()->style()->layoutSpacing(QSizePolicy::Frame,
                                                        QSizePolicy::Frame,
                                                        Qt::Vertical);
    if (result < 0)
        result = 5;
    return result;
}

QLayoutItem* SameHeightLayout::itemAt(int i) const
{
    return m_list.value(i);
}

QLayoutItem* SameHeightLayout::takeAt(int i)
{
    return i >= 0 && i < m_list.size() ? m_list.takeAt(i) : nullptr;
}

void SameHeightLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    if (m_list.size() == 0)
        return;
    int count = m_list.count();
    int width = rect.width();
    int height = (rect.height() - (count - 1) * getSpacing()) / count;
    int x = rect.x();
    int y = rect.y();
    for (int i = 0; i < count; ++i)
    {
        QRect geom(x, y, width, height);
        m_list.at(i)->setGeometry(geom);
        y = y + height + getSpacing();
    }
}

//-----------------------------------------------------------------------------
