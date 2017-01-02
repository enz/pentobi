import QtQuick 2.0

Item {
    id: root

    property string gameVariant
    property bool showCoordinates
    property bool isTrigon: gameVariant.startsWith("trigon")
    property bool isNexos: gameVariant.startsWith("nexos")
    property bool isCallisto: gameVariant.startsWith("callisto")
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
        else if (isNexos) Math.floor(sideLength / (n - 0.5))
        else return Math.floor(sideLength / n)
    }
    property real gridHeight: {
        if (isTrigon) return Math.sqrt(3) * gridWidth
        else return gridWidth
    }
    property real startingPointSize: {
        if (isTrigon) return 0.27 * gridHeight
        if (isNexos) return 0.3 * gridHeight
        return 0.35 * gridHeight
    }

    signal clicked(point pos)

    function mapFromGameX(x) {
        if (isTrigon) return image.x + (x + 0.5) * gridWidth
        else if (isNexos) return image.x + (x - 0.25) * gridWidth
        else return image.x + x * gridWidth
    }
    function mapFromGameY(y) {
        if (isNexos) return image.y + (y - 0.25) * gridHeight
        else return image.y + y * gridHeight
    }
    function mapToGame(pos) {
        if (isTrigon)
            return Qt.point((pos.x - image.x - 0.5 * gridWidth) / gridWidth,
                            (pos.y - image.y) / gridHeight)
        else if (isNexos)
            return Qt.point((pos.x - image.x + 0.25 * gridWidth) / gridWidth,
                            (pos.y - image.y + 0.25 * gridHeight) / gridHeight)
        else
            return Qt.point((pos.x - image.x) / gridWidth,
                            (pos.y - image.y) / gridHeight)
    }
    function getCenterYTrigon(pos) {

        var isDownward = ((pos.x % 2 == 0) != (pos.y % 2 == 0))
        if (gameVariant === "trigon_3")
            isDownward = ! isDownward
        return (isDownward ? 1 : 2) / 3 * gridHeight
    }
    function getColumnCoord(x) {
        if (x > 25)
            return "A" + String.fromCharCode("A".charCodeAt(0) + (x - 26))
        return String.fromCharCode("A".charCodeAt(0) + x)
    }

    Image {
        id: image

        width: {
            if (isTrigon) return gridWidth * (columns + 1)
            else if (isNexos) return gridWidth * (columns - 0.5)
            else return gridWidth * columns
        }
        height: {
            if (isNexos) return gridHeight * (rows - 0.5)
            else return gridHeight * rows
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
                return theme.getImage("board-callisto")
            case "callisto_2":
                return theme.getImage("board-callisto-2")
            case "callisto_3":
                return theme.getImage("board-callisto-3")
            default:
                return theme.getImage("board-tile-classic")
            }
        }
        sourceSize {
            width: {
                if (isTrigon || isCallisto) return width
                if (isNexos) return 2 * gridWidth
                return gridWidth
            }
            height: {
                if (isTrigon || isCallisto) return height
                if (isNexos) return 2 * gridHeight
                return gridHeight
            }
        }
        // Tiling board elements is much faster than loading a whole board
        // image, but tiling is not possible if the board is not rectangular
        // (Trigon, Callisto). Note that the aspect ratio of the Trigon SVG
        // files is 1:1 to avoid irrational coordinates for the triangles but
        // Image.sourceSize always maintains the image's aspect ratio, so we
        // need to use Image.Stretch here, not Image.Tile.
        fillMode: isTrigon ? Image.Stretch : Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
        cache: false
    }
    Repeater {
        model: gameModel.startingPoints0

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorBlue
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: gameModel.startingPoints1

        Rectangle {
            visible: image.status == Image.Ready
            color: gameModel.gameVariant === "duo"
                   || gameModel.gameVariant === "junior"
                   || gameModel.gameVariant === "callisto_2" ?
                       theme.colorGreen : theme.colorYellow
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: gameModel.startingPoints2

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorRed
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: gameModel.startingPoints3

        Rectangle {
            visible: image.status == Image.Ready
            color: theme.colorGreen
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + (gridHeight - height) / 2
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
            font.pixelSize: (isTrigon ? 0.4 : 0.6) * gridHeight
            x: mapFromGameX(index) + (gridWidth - width) / 2
            y: mapFromGameY(-1) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? columns : 0

        Text {
            text: getColumnCoord(index)
            color: theme.fontColorCoordinates
            font.pixelSize: (isTrigon ? 0.4 : 0.6) * gridHeight
            x: mapFromGameX(index) + (gridWidth - width) / 2
            y: mapFromGameY(rows) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? rows : 0

        Text {
            text: index + 1
            color: theme.fontColorCoordinates
            font.pixelSize: (isTrigon ? 0.4 : 0.6) * gridHeight
            x: mapFromGameX(isTrigon ? -1.5 : -1) + (gridWidth - width) / 2
            y: mapFromGameY(rows - index - 1) + (gridHeight - height) / 2
        }
    }
    Repeater {
        model: showCoordinates ? rows : 0

        Text {
            text: index + 1
            color: theme.fontColorCoordinates
            font.pixelSize: (isTrigon ? 0.4 : 0.6) * gridHeight
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
