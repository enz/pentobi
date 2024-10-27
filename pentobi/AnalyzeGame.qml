//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var elements: analyzeGameModel.elements
    property var color: [ color0[0], color1[0], color2[0], color3[0] ]
    property int markMoveNumber: analyzeGameModel.markMoveNumber
    property Item theme

    property real margin
    // Distance between moves on the x axis
    property real dist
    property color colorBackground: theme.colorBackground
    property color colorLine: Qt.alpha(theme.colorText, 0.3)

    onElementsChanged: {
        analyzeGameModel.markCurrentMove(gameModel)
        canvas.requestPaint()
    }
    onMarkMoveNumberChanged: canvas.requestPaint()
    onThemeChanged: canvas.requestPaint()
    onColorBackgroundChanged: canvas.requestPaint()
    onColorLineChanged: canvas.requestPaint()

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
            // Use the whole width unless few moves have been played
            var nuBins = Math.ceil(Math.max(nuMoves, 50))
            var d = w / nuBins
            var ctx = getContext("2d")
            ctx.fillStyle = colorBackground
            ctx.fillRect(0, 0, w, h)
            ctx.strokeStyle = colorLine
            ctx.save()
            ctx.translate(d / 2, d / 2)
            w -= d
            h -= d
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(w, 0)
            ctx.moveTo(0, h)
            ctx.lineTo(w, h)
            ctx.stroke()
            ctx.beginPath()
            ctx.moveTo(0, h / 2)
            ctx.lineTo(w, h / 2)
            ctx.stroke()
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
            margin = d / 2
        }
    }
    Label {
        visible: elements.length === 0 && ! analyzeGameModel.isRunning
        anchors.centerIn: parent
        color: theme.colorText
        opacity: 0.8
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
