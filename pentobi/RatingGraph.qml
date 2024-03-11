//-----------------------------------------------------------------------------
/** @file pentobi/RatingGraph.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Canvas {
    property var history

    antialiasing: true
    onHistoryChanged: requestPaint()
    onPaint: {
        var w = width
        var h = height
        var ctx = getContext("2d")
        ctx.fillStyle = "white"
        ctx.fillRect(0, 0, w, h)
        if (history === null)
            return
        var n = history.length
        if (n === 0)
            return
        var margin = w / 30
        ctx.save()
        ctx.translate(margin, margin)
        w -= 2 * margin
        h -= 2 * margin
        var i
        var minY = Number.POSITIVE_INFINITY
        var maxY = Number.NEGATIVE_INFINITY
        var info
        for (i = 0; i < n; ++i) {
            minY = Math.min(minY, history[i])
            maxY = Math.max(maxY, history[i])
        }
        minY = Math.floor(minY / 100) * 100
        maxY = Math.ceil(maxY / 100) * 100
        if (maxY - minY < 100)
            maxY = minY + 100

        ctx.beginPath()
        var top =  0
        ctx.moveTo(0, top)
        ctx.lineTo(w, top)
        var bottom =  h
        ctx.moveTo(0, bottom)
        ctx.lineTo(w, bottom)
        ctx.strokeStyle = "gray"
        ctx.stroke()

        ctx.font = Math.ceil(0.15 * h) + "px sans-serif"
        ctx.fillStyle = "gray"
        ctx.textAlign = "right"
        ctx.fillText(minY, w, h - w / 60)
        ctx.textBaseline = "top"
        ctx.fillText(maxY, w, w / 60)

        ctx.beginPath()
        for (i = 0; i < n; ++i)
            ctx.lineTo(i * w / n, h - (history[i] - minY)  / (maxY - minY) * h)
        ctx.strokeStyle = "red"
        ctx.stroke()

        ctx.restore()
    }
}
