//-----------------------------------------------------------------------------
/** @file libpentobi_gui/StatusBarButton.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_STATUS_BAR_BUTTON_H
#define LIBPENTOBI_GUI_STATUS_BAR_BUTTON_H

#include <QtGui>

//-----------------------------------------------------------------------------

/** Borderless button to be used in status bars.
    It should be possible to use a QToolButton instead and make it borderless
    with setStyleSheet("border-style: none"). This does work but it has the
    effect that the tooltip is not rendered correctly (due to a bug? tested
    with Qt 4.7.4/Linux) */
class StatusBarButton
    : public QLabel
{
    Q_OBJECT

public:
    StatusBarButton(QAction* action, QWidget* parent = 0);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QAction* m_action;
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_STATUS_BAR_BUTTON_H
