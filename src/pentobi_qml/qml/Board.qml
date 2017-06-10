import QtQuick 2.0

Item {
    id: root

    property string gameVariant: gameModel.gameVariant
    property bool showCoordinates
    property bool isTrigon: startsWith(gameVariant, "trigon")
    property bool isNexos: startsWith(gameVariant, "nexos")
    property bool isCallisto: startsWith(gameVariant, "callisto")
    property bool isGembloQ: startsWith(gameVariant, "gembloq")
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
    property int rows: {
        switch (gameVariant) {
        case "duo":
        case "junior":
            return 14
        case "callisto_2":
            return 16
        case "trigon":
        case "trigon_2":
            return 18
        case "trigon_3":
            return 16
        case "nexos":
        case "nexos_2":
            return 25
        case "gembloq":
        case "gembloq_2_4":
            return 28
        case "gembloq_2":
            return 22
        case "gembloq_3":
            return 26
        default:
            return 20
        }
    }
    // Avoid fractional piece element sizes if the piece elements are squares
    property real gridWidth: {
        var sideLength
        if (isTrigon) sideLength = Math.min(width, Math.sqrt(3) * height)
        else sideLength = Math.min(width, height)
        var n = columns
        if (showCoordinates) n += (isTrigon ? 3 : 2)
        if (isTrigon) return sideLength / (n + 1)
        if (isNexos) return Math.floor(sideLength / (n - 0.5))
        if (isGembloQ) return Math.floor(2 * sideLength / n) / 2
        return Math.floor(sideLength / n)
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

    // Helper function for Qt <5.8, which doesn't support String.startsWith()
    function startsWith(s, s1) { return s.substring(0, s1.length) === s1 }

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
                return theme.getImage("board-tile-nexos")
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
            default:
                return theme.getImage("board-tile-classic")
            }
        }
        sourceSize {
            width: {
                if (isTrigon || isCallisto || isGembloQ) return width
                if (isNexos) return 2 * gridWidth
                return gridWidth
            }
            height: {
                if (isTrigon || isCallisto || isGembloQ) return height
                if (isNexos) return 2 * gridHeight
                return gridHeight
            }
        }
        // Tiling board elements is faster than loading a whole board image,
        // but tiling is only possible for rectangular boards. Note that the
        // aspect ratio of the Trigon SVG files is 1:1 to avoid irrational
        // coordinates for the triangles but Image.sourceSize always maintains
        // aspect ratio, so we need Image.Stretch, not Image.Tile.
        fillMode: isTrigon ? Image.Stretch : Image.Tile
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
        onClicked: {
            var pos = mapToGame(Qt.point(mouseX, mouseY))
            pos.x = Math.floor(pos.x)
            pos.y = Math.floor(pos.y)
            root.clicked(pos)
        }
    }
}
