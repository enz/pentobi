//-----------------------------------------------------------------------------
/** @file libpentobi_gui/StatusBarButton.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StatusBarButton.h"

//-----------------------------------------------------------------------------

StatusBarButton::StatusBarButton(QAction* action, QWidget* parent)
    : QLabel(parent),
      m_action(action)
{

    QIcon icon = action->icon();
    if (! icon.isNull())
        setPixmap(icon.pixmap(16));
    else
        setText(action->text());
    setToolTip(action->toolTip());
}

void StatusBarButton::mouseReleaseEvent(QMouseEvent* event)
{
    int x = event->x();
    int y = event->x();
    if (x >= 0 && y >= 0 && x < width() && y < height())
        m_action->activate(QAction::Trigger);
}

//-----------------------------------------------------------------------------
