import QtQuick 2.0
import QtQuick.Window 2.2

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
        if (isNexos) return 0.3 * gridHeight
        if (isGembloQ) return 0.45 * gridHeight
        return 0.35 * gridHeight
    }
    property int coordinateFontSize: {
        if (isTrigon) return 0.4 * gridHeight
        if (isGembloQ) return 0.35 * gridHeight
        return 0.6 * gridHeight
    }

    signal clicked(point pos)
    signal rightClicked(point pos)

    function mapFromGameX(x) {
        if (isTrigon) return image.x + (x + 0.5) * gridWidth
        if (isNexos) return image.x + (x - 0.25) * gridWidth
        return image.x + x * gridWidth
    }
    function mapFromGameY(y) {
        if (isNexos) return image.y + (y - 0.25) * gridHeight
        return image.y + y * gridHeight
    }
    function mapToGame(pos) {
        if (isTrigon)
            return Qt.point((pos.x - image.x - 0.5 * gridWidth) / gridWidth,
                            (pos.y - image.y) / gridHeight)
        if (isNexos)
            return Qt.point((pos.x - image.x + 0.25 * gridWidth) / gridWidth,
                            (pos.y - image.y + 0.25 * gridHeight) / gridHeight)
        return Qt.point((pos.x - image.x) / gridWidth,
                        (pos.y - image.y) / gridHeight)
    }
    // Needs all arguments for dependencies
    function getStartingPointX(x, gridWidth, pointSize, isGembloQ) {
        var sx = mapFromGameX(x) + (gridWidth - pointSize) / 2
        if (isGembloQ) {
            if (x % 2 == 0) sx -= gridWidth / 2
            else sx += gridWidth / 2
        }
        return sx
    }
    // Needs all arguments for dependencies
    function getStartingPointY(y, gridHeight, pointSize, isGembloQ) {
        var sy = mapFromGameY(y) + (gridHeight - pointSize) / 2
        if (isGembloQ) {
            if (y % 2 == 0) sy += gridHeight / 2
            else sy -= gridHeight / 2
        }
        return sy
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

    Image {
        id: image

        width: {
            if (isTrigon) return gridWidth * (columns + 1)
            if (isNexos) return gridWidth * (columns - 0.5)
            return gridWidth * columns
        }
        height: {
            if (isNexos) return gridHeight * (rows - 0.5)
            return gridHeight * rows
        }
        anchors.centerIn: root
        source: {
            switch (gameVariant) {
            case "trigon":
            case "trigon_2":
                return theme.getImage("board-trigon")
            case "trigon_3":
                return theme.getImage("board-trigon-3")
            case "nexos":
            case "nexos_2":
                return theme.getImage("board-nexos")
            case "callisto":
            case "callisto_2_4":
                return theme.getImage("board-callisto")
            case "callisto_2":
                return theme.getImage("board-callisto-2")
            case "callisto_3":
                return theme.getImage("board-callisto-3")
            case "gembloq":
            case "gembloq_2_4":
                return theme.getImage("board-gembloq")
            case "gembloq_2":
                return theme.getImage("board-gembloq-2")
            case "gembloq_3":
                return theme.getImage("board-gembloq-3")
            case "duo":
            case "junior":
                return theme.getImage("board-duo")
            default:
                return theme.getImage("board-classic")
            }
        }
        sourceSize { width: width; height: height }
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
        cache: false
    }
    Repeater {
        model: gameModel.startingPoints0

        Rectangle {
            visible: image.status == Image.Ready
            color: gameVariant === "duo" ?
                       theme.colorPurple : gameVariant === "junior" ?
                           theme.colorGreen : theme.colorBlue
            width: startingPointSize; height: width
            radius: width / 2
            x: getStartingPointX(modelData.x, gridWidth, width, isGembloQ)
            y: getStartingPointY(modelData.y, gridHeight, height, isGembloQ)
        }
    }
    Repeater {
        model: gameModel.startingPoints1

        Rectangle {
            visible: image.status == Image.Ready
            color: gameVariant === "duo" || gameVariant === "junior" ?
                       theme.colorOrange : gameModel.nuColors === 2 ?
                           theme.colorGreen : theme.colorYellow
            width: startingPointSize; height: width
            radius: width / 2
            x: getStartingPointX(modelData.x, gridWidth, width, isGembloQ)
            y: getStartingPointY(modelData.y, gridHeight, height, isGembloQ)
        }
    }
    Repeater {
        model: gameModel.startingPoints2

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorRed
            width: startingPointSize; height: width
            radius: width / 2
            x: getStartingPointX(modelData.x, gridWidth, width, isGembloQ)
            y: getStartingPointY(modelData.y, gridHeight, height, isGembloQ)
        }
    }
    Repeater {
        model: gameModel.startingPoints3

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorGreen
            width: startingPointSize; height: width
            radius: width / 2
            x: getStartingPointX(modelData.x, gridWidth, width, isGembloQ)
            y: getStartingPointY(modelData.y, gridHeight, height, isGembloQ)
        }
    }
    Repeater {
        model: gameModel.startingPointsAll

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorStartingPoint
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + getCenterYTrigon(modelData)
               - height / 2
        }
    }
    Repeater {
        model: showCoordinates ? columns : 0

        Text {
            text: getColumnCoord(index)
            color: theme.fontColorCoordinates
            font.pixelSize: coordinateFontSize
            x: mapFromGameX(index) + (gridWidth - width) / 2
            y: mapFromGameY(-1) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? columns : 0

        Text {
            text: getColumnCoord(index)
            color: theme.fontColorCoordinates
            font.pixelSize: coordinateFontSize
            x: mapFromGameX(index) + (gridWidth - width) / 2
            y: mapFromGameY(rows) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? rows : 0

        Text {
            text: index + 1
            color: theme.fontColorCoordinates
            font.pixelSize: coordinateFontSize
            x: mapFromGameX(isTrigon ? -1.5 : -1) + (gridWidth - width) / 2
            y: mapFromGameY(rows - index - 1) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? rows : 0

        Text {
            text: index + 1
            color: theme.fontColorCoordinates
            font.pixelSize: coordinateFontSize
            x: mapFromGameX(isTrigon ? columns + 0.5 : columns) + (gridWidth - width) / 2
            y: mapFromGameY(rows - index - 1) + (gridHeight - height) / 2
        }
    }
    MouseArea {
        anchors.fill: root
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressAndHold: {
            var pos = mapToGame(Qt.point(mouseX, mouseY))
            pos.x = Math.floor(pos.x)
            pos.y = Math.floor(pos.y)
            root.rightClicked(pos)
        }
        onClicked: {
            var pos = mapToGame(Qt.point(mouseX, mouseY))
            pos.x = Math.floor(pos.x)
            pos.y = Math.floor(pos.y)
            if (mouse.button & Qt.RightButton)
                root.rightClicked(pos)
            else
                root.clicked(pos)
        }
    }
}
