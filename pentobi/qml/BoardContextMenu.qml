//-----------------------------------------------------------------------------
/** @file pentobi/qml/BoardContextMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "." as Pentobi

Pentobi.Menu {
    property int moveNumber

    relativeWidth: 15
    onOpened: {
        var annotation = gameModel.getMoveAnnotation(moveNumber)
        itemAnnotation.text =
                annotation === "" ?
                    qsTr("Move Annotation") :
                    //: The argument is the annotation symbol for the current move
                    qsTr("Move Annotation (%1)").arg(annotation)
    }

    Pentobi.MenuItem {
        enabled: moveNumber !== gameModel.moveNumber && ! isRated
        text: qsTr("Go to Move %1").arg(moveNumber)
        onTriggered: gameModel.gotoMove(moveNumber)
    }
    Pentobi.MenuItem {
        id: itemAnnotation

        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = moveNumber
            moveAnnotationDialog.open()
        }
    }
}
