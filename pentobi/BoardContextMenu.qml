//-----------------------------------------------------------------------------
/** @file pentobi/BoardContextMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    property int moveNumber

    relativeWidth: 15
    onAboutToShow: {
        var annotation = gameModel.getMoveAnnotation(moveNumber)
        itemAnnotation.text =
                annotation === "" ?
                    qsTr("Move Annotation") :
                    //: The argument is the annotation symbol for the current move
                    qsTr("Move Annotation (%1)").arg(annotation)
    }

    PentobiMenuItem {
        enabled: moveNumber !== gameModel.moveNumber && ! isRated
        text: qsTr("Go to Move %1").arg(moveNumber)
        onTriggered: gameModel.gotoMove(moveNumber)
    }
    PentobiMenuItem {
        id: itemAnnotation

        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = moveNumber
            moveAnnotationDialog.open()
        }
    }
}
