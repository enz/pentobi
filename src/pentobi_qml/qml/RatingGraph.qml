import QtQuick 2.0
import QtQuick.Window 2.0

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
        var minX = Number.POSITIVE_INFINITY
        var maxX = Number.NEGATIVE_INFINITY
        var minY = Number.POSITIVE_INFINITY
        var maxY = Number.NEGATIVE_INFINITY
        var info
        for (i = 0; i < n; ++i) {
            info = ratingModel.history[i]
            minX = Math.min(minX, info.number)
            maxX = Math.max(maxX, info.number)
            minY = Math.min(minY, info.rating)
            maxY = Math.max(maxY, info.rating)
        }
        maxX = minX + Math.ceil((maxX - minX) * 1.2)
        minY = Math.floor(minY / 100) * 100
        maxY = Math.ceil(maxY / 100) * 100
        if (maxY - minY < 100)
            maxY = minY + 100

        ctx.beginPath()
        ctx.lineWidth = Math.max(1, 0.2 * Screen.pixelDensity)
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
        for (i = 0; i < n; ++i) {
            info = ratingModel.history[i]
            ctx.lineTo((info.number - minX)  / (maxX - minX) * w,
                       h - (info.rating - minY)  / (maxY - minY) * h)
        }
        ctx.strokeStyle = "red"
        ctx.stroke()

        ctx.restore()
    }
}
