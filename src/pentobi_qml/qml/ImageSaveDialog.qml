import QtQuick 2.0
import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save Image")
    selectExisting: false
    folder: shortcuts.pictures
    nameFilters: [ qsTr("PNG image files (*.png)"), qsTr("All files (*)") ]
    onAccepted: Logic.exportImage(fileUrl)
    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456
}
