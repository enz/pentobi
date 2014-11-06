import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import "." as Pentobi
import "Main.js" as Logic

ToolBar {
    property bool _isAndroid: Qt.platform.os === "android"

    RowLayout {
        anchors.fill: parent

        Item { Layout.preferredWidth: _isAndroid ? 0.1 * parent.height : 0 }
        // App icon (only on Android)
        Image {
            property real size: parent.height

            visible: _isAndroid
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            // Similar as in Pentobi.ToolButton but pentobi.svg is 48x48
            height: size < 26 || size > 100 ? 0.76 * size : size < 50 ? 24 : 48
            width: height
            sourceSize.height: height
            sourceSize.width: width
            source: _isAndroid ? "icons/pentobi.svg" : ""

        }
        Item { Layout.fillWidth: _isAndroid }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-newgame.svg"
            onClicked: Logic.newGame(true)
        }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-computer-colors.svg"
            onClicked: Logic.showComputerColorDialog()
        }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-play.svg"
            onClicked: Logic.computerPlay()
        }
        Item { Layout.fillWidth: ! _isAndroid }
    }
}
