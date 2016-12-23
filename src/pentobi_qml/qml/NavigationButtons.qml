import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import "." as Pentobi

RowLayout
{
    Pentobi.Button {
        enabled: gameModel.canGoBackward
        imageSource: "icons/pentobi-beginning.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goBeginning()
    }
    Pentobi.Button {
        enabled: gameModel.canGoBackward
        imageSource: "icons/pentobi-backward.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goBackward()
        // Don't use autoRepeat: true, there is a bug in Qt 5.8-rc on Android
        // that makes the button produce an infinite number of clicks if it
        // becomes disabled while pressed (QTBUG-57797).
        autoRepeat: enabled
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: "icons/pentobi-forward.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goForward()
        // Don't use autoRepeat: true, see comment in backward button.
        autoRepeat: enabled
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: "icons/pentobi-end.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goEnd()
    }
    Pentobi.Button {
        enabled: gameModel.hasPrevVar
        imageSource: "icons/pentobi-previous-variation.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goPrevVar()
    }
    Pentobi.Button {
        enabled: gameModel.hasNextVar
        imageSource: "icons/pentobi-next-variation.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goNextVar()
    }
}
