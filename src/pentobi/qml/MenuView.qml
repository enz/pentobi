import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("View"),
                       //: Mnemonic for menu View. Leave empty for no mnemonic.
                       qsTr("V"))

    MenuMoveMarking { }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Coordinates"),
                          //: Mnemonic for menu item Coordinates. Leave empty for no mnemonic.
                          qsTr("C"))
        checkable: true
        checked: gameDisplay.showCoordinates
        onTriggered: {
            checked = ! gameDisplay.showCoordinates // Workaround for QTBUG-69401
            gameDisplay.showCoordinates = checked
        }
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Show Variations"),
                          //: Mnemonic for menu item Show Variations. Leave empty for no mnemonic.
                          qsTr("S"))
        checkable: true
        checked: gameModel.showVariations
        onTriggered: {
            checked = ! gameModel.showVariations // Workaround for QTBUG-69401
            gameModel.showVariations = checked
        }
    }
    MenuSeparator { }
    MenuTheme { }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Animate Pieces"),
                          //: Mnemonic for menu item Animate Pieces. Leave empty for no mnemonic.
                          qsTr("A"))
        checkable: true
        checked: gameDisplay.enableAnimations
        onTriggered: {
            checked = ! gameDisplay.enableAnimations // Workaround for QTBUG-69401
            gameDisplay.enableAnimations = checked
        }
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionFullscreen
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Fullscreen. Leave empty for no mnemonic.
                          qsTr("F"))
    }
}
