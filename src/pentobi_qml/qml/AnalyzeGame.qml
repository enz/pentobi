import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    id: root

    property var elements: analyzeGameModel.elements
    property var color: [
        theme.colorBlue,
        (gameModel.nuColors === 2 ? theme.colorGreen : theme.colorYellow),
        theme.colorRed,
        theme.colorGreen
    ]
    property real margin: makeCrispY(width / 60, y)
    property int markMoveNumber: analyzeGameModel.markMoveNumber
    property int maxMoves: gameDisplay.pieces0.length * gameModel.nuColors

    // Distance between moves on the x axis
    property real dist

    // Modify dy, such that y + dy is an odd multiple of 0.5 for crisp
    // horizontal lines with linewidth 1
    function makeCrispY(dy, y) {
        return dy - (y + dy - Math.floor(y + dy)) + 0.5
    }

    onElementsChanged: {
        analyzeGameModel.markCurrentMove(gameModel)
        canvas.requestPaint()
    }
    onMarkMoveNumberChanged: canvas.requestPaint()

    Canvas {
        id: canvas

        visible: elements.length > 0 || analyzeGameModel.isRunning
        anchors.fill: parent
        antialiasing: true
        onPaint: {
            var elements = analyzeGameModel.elements
            var nuMoves = elements.length
            var w = width
            var h = height
            var ctx = getContext("2d")

            ctx.fillStyle = theme.analyzeBackgroundColor
            ctx.fillRect(0, 0, w, h)

            ctx.save()
            ctx.translate(margin, margin)
            w -= 2 * margin
            h -= 2 * margin
            var d = w / (Math.max(maxMoves, 1))
            dist = d

            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(w, 0)
            ctx.moveTo(0, h)
            ctx.lineTo(w, h)
            ctx.strokeStyle = theme.analyzeLineColor
            ctx.stroke()

            ctx.beginPath()
            var middle =  makeCrispY(h / 2, root.y + margin)
            ctx.moveTo(0, middle)
            ctx.lineTo(w, middle)
            ctx.strokeStyle = theme.analyzeMiddleLineColor
            ctx.stroke()

            var i = root.markMoveNumber
            if (markMoveNumber >= 0 && markMoveNumber <= nuMoves) {
                ctx.beginPath()
                ctx.moveTo(i * d, 0)
                ctx.lineTo(i* d, h)
                ctx.strokeStyle = theme.analyzeLineColor
                ctx.stroke()
            }

            var value
            var radius = d / 2
            for (i = 0; i < nuMoves; ++i) {
                ctx.beginPath()
                ctx.fillStyle = color[elements[i].moveColor]
                ctx.arc(i * d, h - elements[i].value * h, radius, 0, 2 * Math.PI)
                ctx.fill()
            }

            ctx.restore()
        }
    }
    Text {
        visible: elements.length === 0 && ! analyzeGameModel.isRunning
        anchors.centerIn: parent
        color: theme.fontColorPosInfo
        text: qsTr("(No analysis)")
    }
    ProgressBar {
        visible: analyzeGameModel.isRunning
        anchors.centerIn: parent
        minimumValue: 0; maximumValue: 100
        value: analyzeGameModel.progress
    }
    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (dist === 0 || elements.length === 0
                    || analyzeGameModel.isRunning)
                return
            var moveNumber = Math.round((mouseX - margin) / dist)
            analyzeGameModel.gotoMove(gameModel, moveNumber)
        }
    }
}
