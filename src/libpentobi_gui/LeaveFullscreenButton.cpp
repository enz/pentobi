//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LeaveFullscreenButton.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LeaveFullscreenButton.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QToolButton>

//-----------------------------------------------------------------------------

LeaveFullscreenButton::LeaveFullscreenButton(QWidget* parent, QAction* action)
    : QObject(parent)
{
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_triggerArea = new QWidget(parent);
    m_triggerArea->setMouseTracking(true);
    m_button = new QToolButton(parent);
    m_button->setDefaultAction(action);
    m_button->setToolTip("");
    m_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_button->show();
    // Resize to size hint as a workaround for a bug that clips the
    // long button text (tested on Qt 4.8.3 on Linux/KDE).
    m_button->resize(m_button->sizeHint());
    int x = qApp->desktop()->screenGeometry().width() - m_button->width();
    m_buttonPos = QPoint(x, 0);
    m_triggerArea->resize(QSize(m_button->width(), m_button->height() / 2));
    m_triggerArea->move(m_buttonPos);
    m_animation = new QPropertyAnimation(m_button, "pos");
    m_animation->setDuration(1000);
    m_animation->setStartValue(m_buttonPos);
    m_animation->setEndValue(QPoint(x, -m_button->height() + 5));
    qApp->installEventFilter(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(slideOut()));
}

void LeaveFullscreenButton::hideButton()
{
    m_animation->stop();
    m_timer->stop();
    m_triggerArea->hide();
    m_button->hide();
}

bool LeaveFullscreenButton::eventFilter(QObject* watched, QEvent* event)
{
    if (m_button->isVisible() && event->type() == QEvent::MouseMove
        && (watched == m_triggerArea || watched == m_button))
        showButton();
    return false;
}

void LeaveFullscreenButton::showButton()
{
    m_animation->stop();
    m_button->move(m_buttonPos);
    m_button->show();
    m_triggerArea->hide();
    m_timer->start(5000);
}

void LeaveFullscreenButton::slideOut()
{
    m_triggerArea->show();
    m_animation->start();
}

//-----------------------------------------------------------------------------
