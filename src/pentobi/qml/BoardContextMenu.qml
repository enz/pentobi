//-----------------------------------------------------------------------------
/** @file pentobi/qml/BoardContextMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "." as Pentobi

Pentobi.Menu {
    property int moveNumber

    property string _annotation;

    dynamicWidth: true
    onOpened: _annotation = gameModel.getMoveAnnotation(moveNumber)

    Pentobi.MenuItem {
        width: parent.width
        enabled: moveNumber !== gameModel.moveNumber && ! isRated
        text: qsTr("Go to Move %1").arg(moveNumber)
        onTriggered: gameModel.gotoMove(moveNumber)
    }
    Pentobi.MenuItem {
        width: parent.width
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
