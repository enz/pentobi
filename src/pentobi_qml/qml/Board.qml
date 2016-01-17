import QtQuick 2.0

Item {
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
    property real gridElementWidth: {
        var sideLength
        if (isTrigon) sideLength = Math.min(width, Math.sqrt(3) * height)
        else sideLength = Math.min(width, height)
        if (isTrigon) return sideLength / (columns + 1)
        else if (isNexos) Math.floor(sideLength / (columns - 0.5))
        else return Math.floor(sideLength / columns)
    }
    property real gridElementHeight: {
        if (isTrigon) return Math.sqrt(3) * gridElementWidth
        else return gridElementWidth
    }
    property real imgWidth: {
        if (isTrigon) return gridElementWidth * (columns + 1)
        else if (isNexos) return gridElementWidth * (columns - 0.5)
        else return gridElementWidth * columns
    }
    property real imgHeight: {
        if (isNexos) return gridElementHeight * (rows - 0.5)
        else return gridElementHeight * rows
    }
    property real imgOffsetX: (width - imgWidth) / 2
    property real imgOffsetY: (height - imgHeight) / 2

    function mapFromGameX(x) {
        if (isTrigon) return imgOffsetX + (x + 0.5) * gridElementWidth
        else if (isNexos) return imgOffsetX + (x - 0.25) * gridElementWidth
        else return imgOffsetX + x * gridElementWidth
    }
    function mapFromGameY(y) {
        if (isNexos) return imgOffsetY + (y - 0.25) * gridElementHeight
        else return imgOffsetY + y * gridElementHeight
    }
    function mapToGame(x, y) {
        if (isTrigon)
            return Qt.point((x - imgOffsetX + 0.5 * gridElementWidth)
                            / gridElementWidth,
                            (y - imgOffsetY) / gridElementHeight)
        else if (isNexos)
            return Qt.point((x - imgOffsetX + 0.25 * gridElementWidth)
                            / gridElementWidth,
                            (y - imgOffsetY + 0.25 * gridElementHeight)
                            / gridElementHeight)
        else
            return Qt.point((x - imgOffsetX) / gridElementWidth,
                            (y - imgOffsetY) / gridElementHeight)
    }

    Image {
        x: imgOffsetX
        y: imgOffsetY
        width: imgWidth; height: imgHeight
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
        sourceSize { width: imgWidth; height: imgHeight }
        asynchronous: true
    }
}
