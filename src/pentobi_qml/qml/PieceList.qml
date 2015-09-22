import QtQuick 2.0

Row {
    id: root

    property real pieceAreaSize
    property var pieces
    property int nuPiecesLeft
    property alias rows: grid.rows

    signal piecePicked(var piece)

    Image {
        source: theme.getImage("flick")
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXBeginning ? 0 : 1
        width: 0.03 * root.width; height: root.height
    }
    Flickable {
        id: flickable

        property int _visibleColumns: root.pieceAreaSize == 0 ?
                                          0 :
                                          Math.floor(width / root.pieceAreaSize)
        property bool _allPiecesLeftFitInVisible:
            nuPiecesLeft <= rows * _visibleColumns

        width: 0.94 * root.width
        height: parent.height
        contentWidth: grid.columns * root.pieceAreaSize
        clip: true

        Grid {
            id: grid

            flow: Grid.LeftToRight
            columns: flickable._allPiecesLeftFitInVisible || rows == 0 ?
                         flickable._visibleColumns :
                         Math.ceil(nuPiecesLeft / rows)
            onColumnsChanged: flickable.contentX = 0

            Repeater {
                model: pieces

                MouseArea {
                    width: pieceAreaSize
                    height: pieceAreaSize
                    onClicked: piecePicked(modelData)
                    Component.onCompleted: modelData.parentPieceArea = this
                }
            }
        }
    }
    Image {
        width: 0.03 * root.width; height: root.height
        source: theme.getImage("flick")
        mirror: true
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXEnd ? 0 : 1
    }
}
