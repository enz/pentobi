import QtQuick 2.0

Item {
    id: root

    property string gameVariant
    property bool isTrigon: gameVariant.lastIndexOf("trigon") === 0
    property bool isNexos: gameVariant.lastIndexOf("nexos") === 0
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
        if (isTrigon) return sideLength / (columns + 1)
        else if (isNexos) Math.floor(sideLength / (columns - 0.5))
        else return Math.floor(sideLength / columns)
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

    function mapFromGameX(x) {
        if (isTrigon) return image.x + (x + 0.5) * gridWidth
        else if (isNexos) return image.x + (x - 0.25) * gridWidth
        else return image.x + x * gridWidth
    }
    function mapFromGameY(y) {
        if (isNexos) return image.y + (y - 0.25) * gridHeight
        else return image.y + y * gridHeight
    }
    function mapToGame(x, y) {
        if (isTrigon)
            return Qt.point((x - image.x - 0.5 * gridWidth) / gridWidth,
                            (y - image.y) / gridHeight)
        else if (isNexos)
            return Qt.point((x - image.x + 0.25 * gridWidth) / gridWidth,
                            (y - image.y + 0.25 * gridHeight)
                            / gridHeight)
        else
            return Qt.point((x - image.x) / gridWidth,
                            (y - image.y) / gridHeight)
    }
    function getCenterYTrigon(pos) {
        var isDownward = ((pos.x % 2 == 0) != (pos.y % 2 == 0))
        return (isDownward ? 1 : 2) / 3 * gridHeight
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
            case "duo":
            case "junior":
                return theme.getImage("board-duo")
            case "nexos":
            case "nexos_2":
                return theme.getImage("board-nexos")
            case "callisto":
                return theme.getImage("board-callisto")
            case "callisto_2":
                return theme.getImage("board-callisto-2")
            case "callisto_3":
                return theme.getImage("board-callisto-3")
            default:
                return theme.getImage("board-classic")
            }
        }
        sourceSize { width: width; height: height }
    }
    Repeater {
        model: gameModel.startingPoints0

        Rectangle {
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
            color: gameModel.gameVariant == "duo"
                   || gameModel.gameVariant == "junior"
                   || gameModel.gameVariant == "callisto_2" ?
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
            color: theme.colorStartingPoint
            width: startingPointSize; height: width
            radius: width / 2
            x: mapFromGameX(modelData.x) + (gridWidth - width) / 2
            y: mapFromGameY(modelData.y) + getCenterYTrigon(modelData)
               - height / 2
        }
    }
}
