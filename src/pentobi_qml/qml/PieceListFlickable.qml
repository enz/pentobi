import QtQuick 2.0

Row {
    id: root

    property real pieceAreaSize
    property int color
    property var pieces
    property int nuPiecesLeft
    property int rows: 1
    property bool allPiecesFitInVisible

    signal piecePicked(var piece)

    Loader {
        id: flickLeftLoader

        visible: ! allPiecesFitInVisible
        width: 0.03 * root.width; height: root.height
    }
    Component {
        id: flickLeftComponent

        Image {
            source: theme.getImage("flick-left")
            sourceSize { width: width; height: height }
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            opacity: flickable.atXBeginning ? 0 : 1
        }
    }
    Flickable {
        id: flickable

        property int _visibleColumns: root.pieceAreaSize == 0 ?
                                          0 :
                                          Math.floor(width / root.pieceAreaSize)
        property bool _allPiecesLeftFitInVisible:
            nuPiecesLeft <= rows * _visibleColumns

        width: (allPiecesFitInVisible ? 1 : 0.94) * root.width
        height: parent.height
        contentWidth: pieceList.columns * root.pieceAreaSize
        clip: ! allPiecesFitInVisible
        onAtXBeginningChanged:
            if (! allPiecesFitInVisible && ! atXBeginning
                    && flickLeftLoader.status == Loader.Null)
                flickLeftLoader.sourceComponent = flickLeftComponent
        onAtXEndChanged:
            if (! allPiecesFitInVisible && ! atXEnd
                    && flickRightLoader.status == Loader.Null)
                flickRightLoader.sourceComponent = flickRightComponent

        PieceList {
            id: pieceList

            rows: root.rows
            columns: flickable._allPiecesLeftFitInVisible || rows == 0 ?
                         flickable._visibleColumns :
                         Math.ceil(nuPiecesLeft / rows)
            pieceAreaSize: root.pieceAreaSize
            pieces: root.pieces
            onPiecePicked: root.piecePicked(piece)
            onColumnsChanged: flickable.contentX = 0
        }
    }

    Loader {
        id: flickRightLoader

        visible: ! allPiecesFitInVisible
        width: 0.03 * root.width; height: root.height
    }
    Component {
        id: flickRightComponent

        Image {
            visible: ! allPiecesFitInVisible
            width: 0.03 * root.width; height: root.height
            source: theme.getImage("flick-left")
            mirror: true
            sourceSize { width: width; height: height }
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            opacity: flickable.atXEnd ? 0 : 1
        }
    }
}
