//-----------------------------------------------------------------------------
/** @file libpentobi_gui/LeaveFullscreenButton.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H
#define LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QObject>
#include <QPoint>

class QAction;
class QPropertyAnimation;
class QTimer;
class QPushButton;

//-----------------------------------------------------------------------------

/** A button at the top right of the screen to leave fullscreen mode.
    The button behaves as recommended by the human interface guidelines of
    Gnome 2. It slides off the screen after a few seconds. A few pixels of the
    button stay visible and also an invisible slightly larger trigger area.
    If the mouse is moved over this area, the button becomes visible again. */
class LeaveFullscreenButton
    : public QObject
{
    Q_OBJECT

public:
    /** Constructor.
        @param parent The widget that will become fullscreen. This class adds
        two child widgets to the parent: the actual button and the trigger area
        (an invisible widget that listens for mouse movements and triggers the
        button to become visible again if it is slid out).
        @param action The action for leaving fullscreen mode associated with
        the button */
    LeaveFullscreenButton(QWidget* parent, QAction* action);

    bool eventFilter(QObject* watched, QEvent* event) override;

    void showButton();

    void hideButton();

private:
    QPushButton* m_button;

    QWidget* m_triggerArea;

    QPoint m_buttonPos;

    QTimer* m_timer;

    QPropertyAnimation* m_animation;


    void slideOut();
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_LEAVE_FULLSCREEN_BUTTON_H
