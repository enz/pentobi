import QtQuick 2.0
import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save")
    selectExisting: false
    folder: root.folder != "" ? root.folder : (isAndroid ? "file:///sdcard" : shortcuts.desktop)
    nameFilters: [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
    onAccepted: {
        root.folder = folder
        // Save dialog on Android doesn't warn about overwriting (last tested on Qt 5.6)
        if (isAndroid && gameModel.checkFileExists(Logic.getFileFromUrl(fileUrl)))
            Logic.showQuestion(qsTr("Overwrite existing file?"), save)
        else
            save()
        // We always create a new save file dialog because currently there is
        // no way to initialize the default file in FileDialog and we don't
        // want the dialog to default to the last file saved, which might be
        // different from the currently loaded file.
        saveDialog.source = ""
    }
    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456

    function save() { Logic.saveFile(Logic.getFileFromUrl(fileUrl)) }
}
