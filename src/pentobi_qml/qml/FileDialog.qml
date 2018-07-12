import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import Qt.labs.folderlistmodel 2.1
import "." as Pentobi
import "Main.js" as Logic

Pentobi.Dialog {
    id: root

    property bool selectExisting: true
    property url folder
    property url fileUrl: folderModel.folder
    property string nameFilterText
    property string nameFilter

    // We don't use standardButtons because on Android, QtCreator does not
    // automatically include the qtbase translations and Dialog in Qt 5.11
    // has no mnemonics for the buttons.
    footer: DialogButtonBox {
        Button {
            text: Logic.removeShortcut(selectExisting ? qsTr("&Open") : qsTr("&Save"))
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: Logic.removeShortcut(qsTr("&Cancel"))
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    onVisibleChanged: {
        if (visible) {
            view.currentIndex = -1
            textField.text = Logic.getFileFromUrl(fileUrl)
        }
    }
    onAccepted: {
        fileUrl = "file://" + textField.text
        folder = folderModel.folder
    }

    Column {
        width: Math.min(textField.height * 20, 0.9 * rootWindow.width)
        spacing: Math.round(Screen.pixelDensity * 1.5)

        TextField {
            id: textField

            width: parent.width
            enabled: ! selectExisting
            Component.onCompleted: textField.cursorPosition = textField.length
        }
        RowLayout {
            width: parent.width

            ToolButton {
                property bool hasParent: ! folderModel.folder.toString().endsWith(":///")

                enabled: hasParent
                text: "<"
                onClicked:
                    if (hasParent) {
                        folderModel.folder = folderModel.parentFolder
                        view.currentIndex = -1
                        textField.text = Logic.getFileFromUrl(folderModel.folder)
                        if (! textField.text.endsWith("/"))
                            textField.text += "/"
                        textField.cursorPosition = textField.length
                    }
            }
            ComboBox {
                Layout.fillWidth: true
                model: [ nameFilterText, qsTr("All files (*)") ]
                onCurrentIndexChanged:
                    if (currentIndex == 0) folderModel.nameFilters = [ root.nameFilter ]
                    else folderModel.nameFilters = [ "*" ]
            }
        }
        ListView {
            id: view

            height: Math.min(Screen.pixelDensity * 80, 0.4 * rootWindow.height)
            width: parent.width
            clip: true
            model: folderModel
            delegate: Button {
                width: view.width
                height: 2 * font.pixelSize
                flat: true
                onActiveFocusChanged: if (activeFocus) view.currentIndex = index
                contentItem: Text {
                    text: {
                        if (index < 0) return ""
                        if (folderModel.isFolder(index)) return "> " + fileName
                        return fileName
                    }
                    color: parent.activeFocus ? "blue" : "black"
                    horizontalAlignment: Text.AlignHLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                onClicked: {
                    view.currentIndex = index
                    if (folderModel.isFolder(index)) {
                        if (! folderModel.folder.toString().endsWith("/"))
                            folderModel.folder = folderModel.folder + "/"
                        folderModel.folder = folderModel.folder + fileName
                        view.currentIndex = -1
                        textField.text = Logic.getFileFromUrl(folderModel.folder) + "/"
                        textField.cursorPosition = textField.length
                    }
                    else {
                        textField.text = filePath
                        textField.cursorPosition = textField.length
                    }
                }
                onDoubleClicked: root.accept()
            }

            FolderListModel {
                id: folderModel

                folder: root.folder
                nameFilters: [ root.nameFilter ]
                showDirsFirst: true
            }
        }
    }
}
