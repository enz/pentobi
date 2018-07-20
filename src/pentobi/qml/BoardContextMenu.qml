import QtQuick.Controls 2.3

Menu {
    property int moveNumber

    MenuItem {
        text: qsTr("Go to Move %1").arg(moveNumber)
        onTriggered: gameModel.gotoMove(moveNumber)
    }
    MenuItem {
        text: qsTr("Move Annotation")
        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = moveNumber
            moveAnnotationDialog.open()
        }
    }
}
