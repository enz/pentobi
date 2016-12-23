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
        // Don't use autoRepeat: true, there is a bug in Qt 5.8 that makes
        // autorepeat not work correctly if enabled becomes false while the
        // button is pressed.
        autoRepeat: enabled
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: "icons/pentobi-forward.svg"
        Layout.fillWidth: true
        onClicked: gameModel.goForward()
        // See comment in backward button
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
