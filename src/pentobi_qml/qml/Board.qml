import QtQuick 2.0

Item {
    property string gameVariant
    property bool isTrigon: gameVariant.indexOf("trigon") >= 0
    property int columns: {
        switch (gameVariant) {
        case "duo":
        case "junior":
            return 14
        case "trigon":
        case "trigon_2":
            return 35
        case "trigon_3":
            return 31
        default:
            return 20
        }
    }
    property int rows: {
        switch (gameVariant) {
        case "duo":
        case "junior":
            return 14
        case "trigon":
        case "trigon_2":
            return 18
        case "trigon_3":
            return 16
        default:
            return 20
        }
    }
    property real _sideLength: isTrigon ?
                                   Math.min(width, Math.sqrt(3) * height) :
                                   Math.min(width, height)
    // Avoid fractional piece element sizes if the piece elements are squares
    property real gridElementWidth: isTrigon ? _sideLength / (columns + 1): Math.floor(_sideLength / columns)
    property real gridElementHeight: isTrigon ?  Math.sqrt(3) * gridElementWidth : gridElementWidth
    property real _boardWidth: isTrigon ? gridElementWidth * (columns + 1) : gridElementWidth * columns
    property real _boardHeight: gridElementHeight * rows
    property real _boardOffsetX: Math.floor((width - _boardWidth) / 2)
    property real _boardOffsetY: Math.floor((height - _boardHeight) / 2)

    function mapFromGameX(x) { return isTrigon ? _boardOffsetX + (x + 0.5) * gridElementWidth : _boardOffsetX + x * gridElementWidth}
    function mapFromGameY(y) { return _boardOffsetY + y * gridElementHeight }
    function mapToGame(x, y) {
        return isTrigon ?
                    Qt.point((x - _boardOffsetX - 0.5 * gridElementWidth) / gridElementWidth,
                             (y - _boardOffsetY) / gridElementHeight)
                  : Qt.point((x - _boardOffsetX) / gridElementWidth,
                             (y - _boardOffsetY) / gridElementHeight)
    }

    Image {
        x: _boardOffsetX; y: _boardOffsetY
        width: _boardWidth; height: _boardHeight
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
            default:
                return theme.getImage("board-classic")
            }
        }
        sourceSize { width: _boardWidth; height: _boardHeight }
        asynchronous: true
    }
}
