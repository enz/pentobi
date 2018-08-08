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
    property int markMoveNumber: analyzeGameModel.markMoveNumber
    property QtObject theme

    property real margin
    // Distance between moves on the x axis
    property real dist

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
            var m = 0.015 * width
            var ctx = getContext("2d")
            ctx.fillStyle = theme.colorBackground
            ctx.fillRect(0, 0, w, h)
            ctx.strokeStyle = theme.colorCommentBorder
            ctx.beginPath()
            ctx.moveTo(0, m)
            ctx.lineTo(w, m)
            ctx.moveTo(0, h - m)
            ctx.lineTo(w, h - m)
            ctx.stroke()
            ctx.beginPath()
            ctx.moveTo(0, h / 2)
            ctx.lineTo(w, h / 2)
            ctx.stroke()
            ctx.save()
            w -= 2 * m
            h -= 2 * m
            // Use the whole width unless few moves have been played
            var nuBins = Math.ceil(Math.max(nuMoves, 60))
            var d = w / nuBins
            ctx.translate(m, m)
            var n = markMoveNumber
            if (n >= 0 && n <= nuMoves) {
                ctx.beginPath()
                ctx.moveTo(n * d, 0)
                ctx.lineTo(n * d, h)
                ctx.stroke()
            }
            var radius = d / 2
            var i
            for (i = 0; i < nuMoves; ++i) {
                ctx.beginPath()
                ctx.fillStyle = color[elements[i].moveColor]
                ctx.arc(i * d, h - elements[i].value * h,
                        radius, 0, 2 * Math.PI)
                ctx.fill()
            }
            ctx.restore()
            dist = d
            margin = m
        }
    }
    Label {
        visible: elements.length === 0 && ! analyzeGameModel.isRunning
        anchors.centerIn: parent
        color: theme.colorText
        opacity: theme.opacitySubduedText
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
