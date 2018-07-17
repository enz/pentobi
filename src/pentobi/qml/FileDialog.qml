import QtQuick 2.11
import QtQuick.Controls 2.4
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

    Item {
        implicitWidth: Math.min(font.pixelSize * 30, 0.85 * rootWindow.width)
        implicitHeight: columnLayout.implicitHeight

        Action {
            shortcut: "Alt+Left"
            onTriggered: backButton.onClicked()
        }
        ColumnLayout
        {
            id: columnLayout

            anchors.fill: parent

            TextField {
                id: nameField

                visible: ! selectExisting
                focus: ! isAndroid
                onAccepted: if (name.trim().length > 0) root.accept()
                Layout.fillWidth: true
                Component.onCompleted: nameField.cursorPosition = nameField.length
            }
            RowLayout {
                Layout.fillWidth: true

                ToolButton {
                    id: backButton

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
                    elide: Text.ElideLeft
                    Layout.fillWidth: true
                }
            }
            Frame {
                id: frame

                padding: 0.1 * font.pixelSize
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(font.pixelSize* 40, 0.4 * rootWindow.height)

                Rectangle {
                    anchors.fill: parent
                    color: frame.palette.base
                }
                ListView {
                    id: view

                    anchors.fill: parent
                    clip: true
                    model: folderModel
                    boundsBehavior: Flickable.StopAtBounds
                    highlight: Rectangle { color: frame.palette.highlight }
                    highlightMoveDuration: 0
                    delegate: AbstractButton {
                        width: view.width
                        height: 2 * font.pixelSize
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
                                color: view.currentIndex == index ?
                                           frame.palette.highlightedText :
                                           frame.palette.text
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
            ComboBox {
                model: [ nameFilterText, qsTr("All files (*)") ]
                onCurrentIndexChanged:
                    if (currentIndex == 0) folderModel.nameFilters = [ root.nameFilter ]
                    else folderModel.nameFilters = [ "*" ]
                // Default style in Qt 5.11 does not always set implictWidth of
                // ComboBox large enough to fit its content.
                Layout.preferredWidth: font.pixelSize * 18
                Layout.alignment: Qt.AlignRight
            }
        }
    }
}
