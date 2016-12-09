import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save Image")
    selectExisting: false
    folder: shortcuts.pictures
    nameFilters: [ qsTr("Image files (*.png *.jpg)"), qsTr("All files (*)") ]
    onAccepted: {
        Logic.exportImage(fileUrl)
        gameDisplay.forceActiveFocus() // QTBUG-48456
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
}
