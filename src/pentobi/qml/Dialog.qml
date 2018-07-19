import QtQuick.Controls 2.2

Dialog {
    function centerDialog() {
        // Don't bind x and y because that can cause a binding loop if the
        // application window is interactively resized
        if (ApplicationWindow.window) {
            x = (ApplicationWindow.window.width - width) / 2
            y = (ApplicationWindow.window.height - height) / 2
        }
    }

    focus: true
    clip: true
    onWidthChanged: centerDialog()
    onHeightChanged: centerDialog()
    ApplicationWindow.onWindowChanged:
        if (ApplicationWindow.window) {
            ApplicationWindow.window.onWidthChanged.connect(centerDialog)
            ApplicationWindow.window.onHeightChanged.connect(centerDialog)
        }
}
