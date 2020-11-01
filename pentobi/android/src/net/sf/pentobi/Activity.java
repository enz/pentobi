package net.sf.pentobi;

public class Activity
    extends org.qtproject.qt5.android.bindings.QtActivity
{
    public Activity()
    {
        super();
        // Workaround for QTBUG-55600, which causes a white border on
        // some devices when switching orientation/fullscreen
        QT_ANDROID_THEMES = new String[] {"Theme_Black_NoTitleBar"};
        QT_ANDROID_DEFAULT_THEME = "Theme_Black_NoTitleBar";
    }
}
