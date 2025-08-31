//-----------------------------------------------------------------------------
/** @file pentobi/Board.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Window

Item {
    id: root

    property string gameVariant: gameModel.gameVariant
    property bool showCoordinates
    property bool isTrigon: gameVariant.startsWith("trigon")
    property bool isNexos: gameVariant.startsWith("nexos")
    property bool isCallisto: gameVariant.startsWith("callisto")
    property bool isGembloQ: gameVariant.startsWith("gembloq")
    property int columns: {
        switch (gameVariant) {
        case "duo":
        case "junior":
            return 14
        case "callisto_2":
            return 16
        case "trigon":
        case "trigon_2":
            return 35
        case "trigon_3":
            return 31
        case "nexos":
        case "nexos_2":
            return 25
        case "gembloq":
        case "gembloq_2_4":
            return 56
        case "gembloq_2":
            return 44
        case "gembloq_3":
            return 52
        default:
            return 20
        }
    }
    property int rows:
        isTrigon ? (columns + 1) / 2 : isGembloQ ? columns / 2 : columns

    property real gridWidth: {
        // Avoid fractional piece element sizes if the piece elements are squares
        var sideLength
        if (isTrigon) sideLength = Math.min(width, Math.sqrt(3) * height)
        else sideLength = Math.min(width, height)
        var n = columns
        if (showCoordinates) n += (isTrigon ? 3 : 2)
        if (isTrigon) return sideLength / (n + 1)
        if (isNexos) return Math.floor(sideLength * Screen.devicePixelRatio / (n - 0.5)) / Screen.devicePixelRatio
        if (isGembloQ) return Math.floor(2 * sideLength * Screen.devicePixelRatio / n) / 2 / Screen.devicePixelRatio
        return Math.floor(sideLength * Screen.devicePixelRatio / n) / Screen.devicePixelRatio
    }
    property real gridHeight: {
        if (isTrigon) return Math.sqrt(3) * gridWidth
        if (isGembloQ) return 2 * gridWidth
        return gridWidth
    }
    property real startingPointSize: {
        if (isTrigon) return 0.27 * gridHeight
        if (isGembloQ) return 0.45 * gridHeight
        return 0.35 * gridHeight
    }
    property int coordinateFontSize: {
        if (isTrigon) return 0.4 * gridHeight
        if (isGembloQ) return 0.35 * gridHeight
        return 0.5 * gridHeight
    }
    property alias grabImageTarget: grabImageTarget

    signal clicked(point pos)
    signal rightClicked(point pos)

    function mapFromGameX(x) {
        if (isTrigon) return image.x + grabImageTarget.x + (x + 0.5) * gridWidth
        if (isNexos) return image.x + grabImageTarget.x + (x - 0.25) * gridWidth
        return image.x + grabImageTarget.x + x * gridWidth
    }
    function mapFromGameY(y) {
        if (isNexos) return image.y + grabImageTarget.y + (y - 0.25) * gridHeight
        return image.y + grabImageTarget.y + y * gridHeight
    }
    function mapToGame(pos) {
        if (isTrigon)
            return Qt.point((pos.x - grabImageTarget.x - image.x - 0.5 * gridWidth) / gridWidth,
                            (pos.y - grabImageTarget.y - image.y) / gridHeight)
        if (isNexos)
            return Qt.point((pos.x - grabImageTarget.x - image.x + 0.25 * gridWidth) / gridWidth,
                            (pos.y - grabImageTarget.y - image.y + 0.25 * gridHeight) / gridHeight)
        return Qt.point((pos.x - grabImageTarget.x - image.x) / gridWidth,
                        (pos.y - grabImageTarget.y - image.y) / gridHeight)
    }
    // Needs all arguments for dependencies
    function getStartingPointX(x, gridWidth, pointSize) {
        return mapFromGameX(x) - grabImageTarget.x + (gridWidth - pointSize) / 2
    }
    // Needs all arguments for dependencies
    function getStartingPointY(y, gridHeight, pointSize) {
        return mapFromGameY(y) - grabImageTarget.y + (gridHeight - pointSize) / 2
    }
    function getCenterYTrigon(pos) {

        var isDownward = (pos.x % 2 == 0) != (pos.y % 2 == 0)
        if (gameVariant === "trigon_3")
            isDownward = ! isDownward
        return (isDownward ? 1 : 2) / 3 * gridHeight
    }
    function getColumnCoord(x) {
        if (x > 25)
            return String.fromCharCode("A".charCodeAt(0) + x / 26 - 1)
                    + String.fromCharCode("A".charCodeAt(0) + (x % 26))
        return String.fromCharCode("A".charCodeAt(0) + x)
    }

    Item {
        id: grabImageTarget

        anchors.centerIn: parent
        width: {
            if (! root.showCoordinates)
                return image.width
            if (root.isTrigon)
                return image.width + 3 * root.gridWidth
            return image.width + 2 * root.gridWidth
        }
        height: {
            if (! root.showCoordinates)
                return image.height
            return image.height + 2 * root.gridHeight
        }

        Image {
            id: image

            width: {
                if (root.isTrigon) return root.gridWidth * (root.columns + 1)
                if (root.isNexos) return root.gridWidth * (root.columns - 0.5)
                return root.gridWidth * root.columns
            }
            height: {
                if (root.isNexos) return root.gridHeight * (root.rows - 0.5)
                return root.gridHeight * root.rows
            }
            anchors.centerIn: parent
            source: width > 0 && height > 0 ?
                        "image://pentobi/board/" + root.gameVariant + "/"
                        + theme.colorBoard[0] + "/" + theme.colorBoard[1] + "/"
                        + theme.colorBoard[2] + "/" + theme.colorBoard[3] + "/"
                        + theme.colorBoard[4] + "/" + theme.colorBoard[5] :
                        ""
            sourceSize { width: width; height: height }
            cache: false
        }
        Repeater {
            model: gameModel.startingPoints0

            Rectangle {
                color: color0[0]
                width: startingPointSize; height: width
                radius: width / 2
                x: getStartingPointX(modelData.x, gridWidth, width)
                y: getStartingPointY(modelData.y, gridHeight, height)
            }
        }
        Repeater {
            model: gameModel.startingPoints1

            Rectangle {
                color: color1[0]
                width: startingPointSize; height: width
                radius: width / 2
                x: getStartingPointX(modelData.x, gridWidth, width)
                y: getStartingPointY(modelData.y, gridHeight, height)
            }
        }
        Repeater {
            model: gameModel.startingPoints2

            Rectangle {
                color: color2[0]
                width: startingPointSize; height: width
                radius: width / 2
                x: getStartingPointX(modelData.x, gridWidth, width)
                y: getStartingPointY(modelData.y, gridHeight, height)
            }
        }
        Repeater {
            model: gameModel.startingPoints3

            Rectangle {
                color: color3[0]
                width: startingPointSize; height: width
                radius: width / 2
                x: getStartingPointX(modelData.x, gridWidth, width)
                y: getStartingPointY(modelData.y, gridHeight, height)
            }
        }
        Repeater {
            model: gameModel.startingPointsAny

            Rectangle {
                color: theme.colorStartingPoint
                width: startingPointSize; height: width
                radius: width / 2
                x: mapFromGameX(modelData.x) - grabImageTarget.x
                   + (gridWidth - width) / 2
                y: mapFromGameY(modelData.y) - grabImageTarget.y
                   + getCenterYTrigon(modelData) - height / 2
            }
        }
        Repeater {
            model: root.showCoordinates ? root.columns : 0

            Label {
                text: getColumnCoord(index)
                opacity: 0.55 - 0.1 * theme.colorBackground.hslLightness
                font.pixelSize: coordinateFontSize
                x: mapFromGameX(index) - grabImageTarget.x
                   + (gridWidth - width) / 2
                y: mapFromGameY(-1) - grabImageTarget.y
                   + (gridHeight - height) / 2
            }
        }
        Repeater {
            model: root.showCoordinates ? root.columns : 0

            Label {
                text: getColumnCoord(index)
                opacity: 0.55 - 0.1 * theme.colorBackground.hslLightness
                font.pixelSize: coordinateFontSize
                x: mapFromGameX(index) - grabImageTarget.x
                   + (gridWidth - width) / 2
                y: mapFromGameY(rows) - grabImageTarget.y
                   + (gridHeight - height) / 2
            }
        }
        Repeater {
            model: root.showCoordinates ? root.rows : 0

            Label {
                text: index + 1
                opacity: 0.55 - 0.1 * theme.colorBackground.hslLightness
                font.pixelSize: coordinateFontSize
                x: mapFromGameX(isTrigon ? -1.5 : -1) - grabImageTarget.x
                   + (gridWidth - width) / 2
                y: mapFromGameY(rows - index - 1) - grabImageTarget.y
                   + (gridHeight - height) / 2
            }
        }
        Repeater {
            model: root.showCoordinates ? root.rows : 0

            Label {
                text: index + 1
                opacity: 0.55 - 0.1 * theme.colorBackground.hslLightness
                font.pixelSize: coordinateFontSize
                x: mapFromGameX(isTrigon ? columns + 0.5 : columns)
                    - grabImageTarget.x + (gridWidth - width) / 2
                y: mapFromGameY(rows - index - 1) - grabImageTarget.y
                   + (gridHeight - height) / 2
            }
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressAndHold: {
                var pos = root.mapToGame(Qt.point(mouseX + grabImageTarget.x,
                                                  mouseY + grabImageTarget.y))
                pos.x = Math.floor(pos.x)
                pos.y = Math.floor(pos.y)
                root.rightClicked(pos)
            }
            onClicked: mouse => {
                var pos = root.mapToGame(Qt.point(mouseX + grabImageTarget.x,
                                                  mouseY + grabImageTarget.y))
                pos.x = Math.floor(pos.x)
                pos.y = Math.floor(pos.y)
                if (mouse.button & Qt.RightButton)
                    root.rightClicked(pos)
                else
                    root.clicked(pos)
            }
        }
    }
}
