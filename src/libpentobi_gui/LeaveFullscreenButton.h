//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LeaveFullscreenButton.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H
#define LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H

#include <QObject>
#include <QPoint>

class QPropertyAnimation;
class QTimer;
class QToolButton;

//-----------------------------------------------------------------------------

/** A button at the top right of the screen to leave fullscreen mode that
    slides of the screen after a few seconds.
    A few pixels of the button stay visible and also an invisible slightly
    larger trigger area. If the mouse is moved over this area, the button
    becomes visible again. */
class LeaveFullscreenButton
    : public QObject
{
    Q_OBJECT

public:
    LeaveFullscreenButton(QWidget* parent);

    bool eventFilter(QObject* watched, QEvent* event);

    void showButton();

    void hideButton();

signals:
    void clicked();

private:
    QToolButton* m_button;

    QWidget* m_triggerArea;

    QPoint m_buttonPos;

    QTimer* m_timer;

    QPropertyAnimation* m_animation;

private slots:
    void handleClicked(bool checked);

    void slideOut();
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H
