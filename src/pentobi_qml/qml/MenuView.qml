import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("&View"))

    Pentobi.Menu {
        title: Logic.removeShortcut(qsTr("&Move Marking"))

        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("Last With &Dot"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking !== "last_number"
                     && gameDisplay.moveMarking !== "all_number"
                     && gameDisplay.moveMarking !== "none"
            onTriggered: gameDisplay.moveMarking = "last_dot"
        }
        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("Last With &Number"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "last_number"
            onTriggered: gameDisplay.moveMarking = "last_number"
        }
        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("&All With Number"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "all_number"
            onTriggered: gameDisplay.moveMarking = "all_number"
        }
        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("N&one"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "none"
            onTriggered: gameDisplay.moveMarking = "none"
        }
    }
    Pentobi.MenuItem {
        text: Logic.removeShortcut(qsTr("&Coordinates"))
        checkable: true
        checked: gameDisplay.showCoordinates
        onTriggered: {
            // Set checked explicitely because of QTBUG-69401
            checked = ! gameDisplay.showCoordinates
            gameDisplay.showCoordinates = checked
        }
    }
    Pentobi.MenuItem {
        text: Logic.removeShortcut(qsTr("Show &Variations"))
        checkable: true
        checked: gameModel.showVariations
        onTriggered: {
            // Set checked explicitely because of QTBUG-69401
            checked = ! gameModel.showVariations
            gameModel.showVariations = checked
        }
    }
    MenuSeparator { }
    Pentobi.Menu {
        title: Logic.removeShortcut(qsTr("&Theme"))

        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("&Light"))
            checkable: true
            autoExclusive: true
            checked: themeName === "light"
            onTriggered: themeName = "light"
        }
        Pentobi.MenuItem {
            text: Logic.removeShortcut(qsTr("&Dark"))
            checkable: true
            autoExclusive: true
            checked: themeName === "dark"
            onTriggered: themeName = "dark"
        }
        Pentobi.MenuItem {
            //: Name of light theme optimized for colorblindness.
            text: Logic.removeShortcut(qsTr("&Colorblind Light"))
            checkable: true
            autoExclusive: true
            checked: themeName === "colorblind-light"
            onTriggered: themeName = "colorblind-light"
        }
        Pentobi.MenuItem {
            //: Name of dark theme optimized for colorblindness.
            text: Logic.removeShortcut(qsTr("Colorblind D&ark"))
            checkable: true
            autoExclusive: true
            checked: themeName === "colorblind-dark"
            onTriggered: themeName = "colorblind-dark"
        }
    }
    Pentobi.Menu {
        title: Logic.removeShortcut(qsTr("&Layout"))
        enabled: ! isAndroid
        Pentobi.MenuItem {
            //: Name of window layout optimized for desktop.
            text: Logic.removeShortcut(qsTr("&Desktop"))
            checkable: true
            autoExclusive: true
            checked: isDesktop
            onTriggered: {
                gameDisplay.destroyPieces()
                isDesktop = true
                gameDisplay.createPieces()
            }
        }
        Pentobi.MenuItem {
            //: Name of window layout optimized for smartphones.
            text: Logic.removeShortcut(qsTr("&Mobile"))
            checkable: true
            autoExclusive: true
            checked: ! isDesktop
            onTriggered: {
                gameDisplay.destroyPieces()
                isDesktop = false
                gameDisplay.createPieces()
            }
        }
    }
    Pentobi.MenuItem {
        text: Logic.removeShortcut(qsTr("&Animate Pieces"))
        checkable: true
        checked: gameDisplay.enableAnimations
        onTriggered: {
            // Set checked explicitely because of QTBUG-69401
            checked = ! gameDisplay.enableAnimations
            gameDisplay.enableAnimations = checked
        }
    }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionFullscreen }
}
