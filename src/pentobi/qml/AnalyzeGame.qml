//-----------------------------------------------------------------------------
/** @file pentobi/qml/AnalyzeGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Window 2.0

Item {
    id: root

    property var elements: analyzeGameModel.elements
    property var color: [
        (gameModel.gameVariant === "duo" ?
             theme.colorPurple : gameModel.gameVariant === "junior" ?
                 theme.colorGreen : theme.colorBlue),
        (gameModel.gameVariant === "duo" || gameModel.gameVariant === "junior" ?
             theme.colorOrange : gameModel.nuColors === 2 ?
                 theme.colorGreen : theme.colorYellow),
        theme.colorRed,
        theme.colorGreen
    ]
    property real margin: makeCrispY(width / 60, y)
    property int markMoveNumber: analyzeGameModel.markMoveNumber
    property QtObject theme

    // Distance between moves on the x axis
    property real dist

    // Modify dy, such that y + dy is an odd multiple of 0.5 for crisp
    // horizontal lines with linewidth 1
    function makeCrispY(dy, y) {
        return dy - ((y + dy) - Math.floor((y + dy) * Screen.devicePixelRatio) / Screen.devicePixelRatio) + 0.5
    }

    onElementsChanged: {
        analyzeGameModel.markCurrentMove(gameModel)
        canvas.requestPaint()
    }
    onMarkMoveNumberChanged: canvas.requestPaint()
    onThemeChanged: canvas.requestPaint()

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
            // Use the whole width unless few moves have been played
            var nuBins = Math.ceil(Math.max(nuMoves, 60))
            var d = w / nuBins
            dist = d

            ctx.beginPath()
            ctx.lineWidth = Math.max(1, 0.2 * Screen.pixelDensity)
            var top =  makeCrispY(0, root.y + margin)
            ctx.moveTo(0, top)
            ctx.lineTo(w, top)
            var bottom =  makeCrispY(h, root.y + margin)
            ctx.moveTo(0, bottom)
            ctx.lineTo(w, bottom)
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
