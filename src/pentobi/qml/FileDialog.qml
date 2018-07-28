//-----------------------------------------------------------------------------
/** @file pentobi/qml/FileDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import Qt.labs.folderlistmodel 2.11
import "." as Pentobi
import "Main.js" as Logic
import "Controls.js" as PentobiControls

Pentobi.Dialog {
    id: root

    property bool selectExisting: true
    property alias name: nameField.text
    property url folder
    property url fileUrl
    property var nameFilterLabels
    property var nameFilters
    readonly property url defaultFolder: androidUtils.getDefaultFolder()

    function selectNameField() {
        if (! isAndroid) {
            var pos = name.lastIndexOf(".")
            if (pos < 0)
                nameField.selectAll()
            else
                nameField.select(0, pos)
        }
        view.currentIndex = -1
    }

    function selectNameFilter(index) {
        comboBoxNameFilter.currentIndex = index
    }

    signal nameFilterChanged(int index)

    property url _lastFolder

    function _checkAccept() {
        if (name.trim().length === 0)
            return
        folder = folderModel.folder
        fileUrl = folder + "/" + name
        if (! selectExisting
                && gameModel.checkFileExists(Logic.getFileFromUrl(fileUrl))) {
            Logic.showQuestion(qsTr("Overwrite file?"), accept)
            return
        }
        accept()
    }

    footer: Pentobi.DialogButtonBox {
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
            onClicked: _checkAccept()
        }
        ButtonCancel { }
    }
    onOpened: {
        if (isAndroid && ! folder.toString().startsWith(defaultFolder.toString()))
            folder = defaultFolder
        selectNameField()
    }

    Item {
        implicitWidth: Math.min(font.pixelSize * 30, 0.9 * rootWindow.width)
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
                selectByMouse: true
                Layout.fillWidth: true
                Component.onCompleted: nameField.cursorPosition = nameField.length
                onAccepted: _checkAccept()
                onTextEdited: view.currentIndex = -1
            }
            RowLayout {
                Layout.fillWidth: true

                ToolButton {
                    id: backButton

                    property bool hasParent:
                        ! folderModel.folder.toString().endsWith(":///")
                        && ! (isAndroid && folderModel.folder === defaultFolder)

                    opacity: hasParent ? 1 : 0.5
                    onClicked:
                        if (hasParent) {
                            _lastFolder = folderModel.folder
                            folderModel.folder = folderModel.parentFolder
                            if (selectExisting)
                                name = ""
                        }
                    icon {
                        source: "icons/filedialog-parent.svg"
                        // Icon size is 16x16
                        width: font.pixelSize < 20 ? 16 : font.pixelSize
                        height: font.pixelSize < 20 ? 16 : font.pixelSize
                        color: frame.palette.buttonText
                    }
                }
                Label {
                    text: {
                        if (isAndroid
                                && folderModel.folder.toString().startsWith(defaultFolder.toString()))
                            return folderModel.folder.toString().substr(defaultFolder.toString().length + 1)
                        Logic.getFileFromUrl(folderModel.folder)
                    }
                    elide: Text.ElideLeft
                    Layout.fillWidth: true
                }
                ToolButton {
                    visible: ! selectExisting
                    icon {
                        source: "icons/filedialog-newfolder.svg"
                        // Icon size is 16x16
                        width: font.pixelSize < 20 ? 16 : font.pixelSize
                        height: font.pixelSize < 20 ? 16 : font.pixelSize
                        color: frame.palette.buttonText
                    }
                    onClicked: {
                        var dialog = newFolderDialog.get()
                        dialog.folder = folderModel.folder
                        dialog.open()
                    }
                }
            }
            Frame {
                id: frame

                padding: 0.1 * font.pixelSize
                focusPolicy: Qt.TabFocus
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(font.pixelSize * 20, 0.3 * rootWindow.height)
                background: Rectangle {
                    color: frame.palette.base
                    border.color: frame.activeFocus ? frame.palette.highlight : frame.palette.mid
                    radius: 2
                }
                ScrollView {
                    anchors.fill: parent
                    contentHeight: view.implicitHeight
                    contentWidth: width

                    ListView {
                        id: view

                        clip: true
                        model: folderModel
                        boundsBehavior: Flickable.StopAtBounds
                        highlight: Rectangle {
                            // Should logically use palette.highlight, but in
                            // most styles other than the desktop style Fusion,
                            // palette.highlight is a too flashy color.
                            color: isDesktop ? palette.highlight : palette.midlight
                        }
                        highlightMoveDuration: 0
                        focus: true
                        onActiveFocusChanged:
                            if (activeFocus && currentIndex < 0 && count)
                                currentIndex = 0
                        delegate: AbstractButton {
                            width: view.width
                            height: 2 * font.pixelSize
                            focusPolicy: Qt.NoFocus
                            contentItem: Row {
                                spacing: 0.3 * font.pixelSize
                                leftPadding: 0.2 * font.pixelSize

                                Image {
                                    anchors.verticalCenter: parent.verticalCenter
                                    visible: folderModel.isFolder(index)
                                    // Icon size is 16x16
                                    width: font.pixelSize < 20 ? 16 : font.pixelSize
                                    height: width
                                    source: "icons/filedialog-folder.svg"
                                    sourceSize { width: width; height: height }
                                }
                                Label {
                                    width: parent.width - parent.spacing - parent.leftPadding
                                    text: index < 0 ? "" : fileName
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: view.currentIndex == index ?
                                               // See comment at highlight
                                               (isDesktop ? frame.palette.highlightedText
                                                          : frame.palette.buttonText) :
                                               frame.palette.text
                                    horizontalAlignment: Text.AlignHLeft
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideMiddle
                                }
                            }
                            onClicked: {
                                if (folderModel.isFolder(index)) {
                                    delayedOpenFolderTimer.folderName = fileName
                                    delayedOpenFolderTimer.restart()
                                }
                                else {
                                    name = fileName
                                    if (! selectExisting)
                                        selectNameField()
                                }
                                view.currentIndex = index
                            }
                            onDoubleClicked:
                                if (! folderModel.isFolder(index))
                                    _checkAccept()
                        }
                        FolderListModel {
                            id: folderModel

                            folder: root.folder
                            nameFilters: [ root.nameFilter ]
                            showDirsFirst: true
                            onStatusChanged:
                                if (status === FolderListModel.Ready) {
                                    var i = folderModel.indexOf(_lastFolder)
                                    if (i >= 0)
                                        view.currentIndex = i
                                    else
                                        view.currentIndex = -1
                                }
                        }
                        // Open folder with small delay such that the folder name
                        // is visibly highlighted when clicked before it opens. We
                        // can't set view.currentIndex in onPressed, otherwise the
                        // item is unwantedly highlighted when flicking the list.
                        Timer {
                            id: delayedOpenFolderTimer

                            property string folderName

                            interval: 100

                            onTriggered: {
                                if (! folderModel.folder.toString().endsWith("/"))
                                    folderModel.folder = folderModel.folder + "/"
                                _lastFolder = ""
                                folderModel.folder = folderModel.folder + folderName
                                if (selectExisting)
                                    name = ""
                            }
                        }
                    }
                }
            }
            ComboBox {
                id: comboBoxNameFilter

                model: {
                    var result = nameFilterLabels
                    nameFilterLabels.push(qsTr("All files"))
                    return result
                }
                onCurrentIndexChanged: {
                    if (currentIndex < root.nameFilters.length)
                        folderModel.nameFilters = root.nameFilters[currentIndex]
                    else
                        folderModel.nameFilters = [ "*" ]
                    nameFilterChanged(currentIndex)
                }
                Layout.preferredWidth: Math.min(font.pixelSize * 14, 0.9 * rootWindow.width)
                Layout.alignment: Qt.AlignRight
            }
        }
    }
}
