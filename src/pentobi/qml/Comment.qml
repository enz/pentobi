//-----------------------------------------------------------------------------
/** @file pentobi/qml/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

Item {
    function dropFocus() { if (loader.item) loader.item.dropFocus() }

    function _createTextArea(forceActiveFocus) {
        if (loader.item)
            return
        loader.sourceComponent = loaderComponent
        if (forceActiveFocus)
            loader.item.forceActiveFocus()
    }

    // Placeholder to delay more expensive creation of TextArea
    AbstractButton {
        visible: ! loader.item
        anchors.fill: parent
        background: Rectangle {
            color: theme.colorBackground
            border.color: theme.colorCommentBorder
        }
        onActiveFocusChanged: _createTextArea(true)
        onClicked: _createTextArea(true)
    }
    Connections {
        target: gameModel
        onCommentChanged: if (gameModel.comment !== "") _createTextArea(false)
    }
    Loader {
        id: loader

        anchors.fill: parent

        Component {
            id: loaderComponent

            ScrollView {
                function forceActiveFocus() { textArea.forceActiveFocus() }
                function dropFocus() { textArea.focus = false }

                clip: true

                TextArea {
                    id: textArea

                    text: gameModel.comment
                    color: theme.colorText
                    selectionColor: theme.colorSelection
                    selectedTextColor: theme.colorSelectedText
                    selectByMouse: isDesktop
                    wrapMode: TextEdit.Wrap
                    onTextChanged: gameModel.comment = text
                    background: Rectangle {
                        color:
                            textArea.text !== "" || textArea.activeFocus ?
                                theme.colorCommentBase : theme.colorBackground
                        radius: 2
                        border.color:
                            textArea.activeFocus ? theme.colorCommentFocus
                                                 : theme.colorCommentBorder
                    }
                    Keys.onPressed:
                        if (event.key === Qt.Key_Tab)
                        {
                            focus = false
                            event.accepted = true
                        }
                }
            }
        }
    }
}
