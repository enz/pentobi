import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import Qt.labs.folderlistmodel 2.1
import "." as Pentobi
import "Main.js" as Logic
import "Controls.js" as PentobiControls

Pentobi.Dialog {
    id: root

    property bool selectExisting: true
    property alias name: nameField.text
    property url folder
    property url fileUrl
    property string nameFilterText
    property string nameFilter

    // We don't use standardButtons because on Android, QtCreator does not
    // automatically include the qtbase translations and Dialog in Qt 5.11
    // has no mnemonics for the buttons.
    footer: DialogButtonBox {
        Button {
            enabled: name.trim().length > 0
            text: selectExisting ?
                      PentobiControls.addMnemonic(
                          qsTr("Open"),
                          //: Mnemonic for button Open. Leave empty for no mnemonic.
                          qsTr("O")) :
                      PentobiControls.addMnemonic(
                          qsTr("Save"),
                          //: Mnemonic for button Save. Leave empty for no mnemonic.
                          qsTr("S"))
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: PentobiControls.addMnemonic(
                      qsTr("Cancel"),
                      //: Mnemonic for button Cancel. Leave empty for no mnemonic.
                      qsTr("C"))
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
    onVisibleChanged:
        if (visible) {
            if (! isAndroid) {
                var pos = name.lastIndexOf(".")
                if (pos < 0)
                    nameField.selectAll()
                else
                    nameField.select(0, pos)
            }
            view.currentIndex = -1
        }
    onAccepted: {
        folder = folderModel.folder
        fileUrl = folder + "/" + name
    }

    Column {
        width: Math.min(nameField.height * 20, 0.9 * rootWindow.width)
        spacing: Math.round(Screen.pixelDensity * 1.5)

        TextField {
            id: nameField

            focus: ! selectExisting && ! isAndroid
            width: parent.width
            enabled: ! selectExisting
            onAccepted: if (name.trim().length > 0) root.accept()
            Component.onCompleted: nameField.cursorPosition = nameField.length
        }
        RowLayout {
            width: parent.width

            ToolButton {
                property bool hasParent: ! folderModel.folder.toString().endsWith(":///")

                enabled: hasParent
                onClicked:
                    if (hasParent) {
                        folderModel.folder = folderModel.parentFolder
                        view.currentIndex = -1
                    }
                icon {
                    source: "icons/filedialog-parent.svg"
                    width: 16; height: 16
                }
            }
            Label {
                text: Logic.getFileFromUrl(folderModel.folder)
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }
        ComboBox {
            width: parent.width
            model: [ nameFilterText, qsTr("All files (*)") ]
            onCurrentIndexChanged:
                if (currentIndex == 0) folderModel.nameFilters = [ root.nameFilter ]
                else folderModel.nameFilters = [ "*" ]
        }
        ListView {
            id: view

            height: Math.min(Screen.pixelDensity * 80, 0.4 * rootWindow.height)
            width: parent.width
            clip: true
            model: folderModel
            boundsBehavior: Flickable.StopAtBounds
            highlight: Rectangle { color: "#ddd" }
            highlightMoveDuration: 0
            delegate: Button {
                width: view.width
                height: 2 * font.pixelSize
                flat: true
                onActiveFocusChanged: if (activeFocus) view.currentIndex = index
                contentItem: Row {
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        visible: folderModel.isFolder(index)
                        width: 0.8 * font.pixelSize; height: width
                        source: "icons/filedialog-folder.svg"
                        sourceSize { width: width; height: height }
                    }
                    Text {
                        text: index < 0 ? "" : fileName
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 0.2 * font.pixelSize
                        color: "black"
                        horizontalAlignment: Text.AlignHLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
                onClicked: {
                    view.currentIndex = index
                    if (folderModel.isFolder(index)) {
                        if (! folderModel.folder.toString().endsWith("/"))
                            folderModel.folder = folderModel.folder + "/"
                        folderModel.folder = folderModel.folder + fileName
                        view.currentIndex = -1
                    }
                    else
                        name = fileName
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
