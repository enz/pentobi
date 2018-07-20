import QtQuick.Controls 2.3

Menu {
    property int moveNumber

    property string _annotation;

    onOpened: _annotation = gameModel.getMoveAnnotation(moveNumber)

    MenuItem {
        enabled: moveNumber !== gameModel.moveNumber
        text: qsTr("Go to Move %1").arg(moveNumber)
        onTriggered: gameModel.gotoMove(moveNumber)
    }
    MenuItem {
        text: _annotation === "" ?
                  qsTr("Move Annotation") :
                  //: The argument is the annotation symbol for the current move
                  qsTr("Move Annotation (%1)").arg(_annotation)
        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = moveNumber
            moveAnnotationDialog.open()
        }
    }
}
