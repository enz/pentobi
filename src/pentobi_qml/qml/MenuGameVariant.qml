import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Game Variant"),
                       //: Mnemonic for menu Game Variant. Leave empty for no mnemonic.
                       qsTr("V"))

    ButtonGroup { id: groupGameVariant }
    Pentobi.Menu {
        title: addMnemonic(qsTr("Classic"),
                           //: Mnemonic for menu Classic. Leave empty for no mnemonic.
                           qsTr("C"))

        MenuItemGameVariant {
            gameVariant: "classic_2"
            text: addMnemonic(qsTr("Classic (%1 Players)").arg(2),
                              //: Mnemonic for menu item Classic/Trigon/Nexos (2 Players) and GambloQ/Callisto (2 Players, 2 Colors). Leave empty for no mnemonic.
                              qsTr("2"))
        }
        MenuItemGameVariant {
            gameVariant: "classic_3"
            text: addMnemonic(qsTr("Classic (%1 Players)").arg(3),
                              //: Mnemonic for menu item Classic/Trigon/GembloQ/Callisto (3 Players). Leave empty for no mnemonic.
                              qsTr("3"))
        }
        MenuItemGameVariant {
            gameVariant: "classic"
            text: addMnemonic(qsTr("Classic (%1 Players)").arg(4),
                              //: Mnemonic for menu item Classic/Trigon/Nexos/GembloQ/Callisto (4 Players). Leave empty for no mnemonic.
                              qsTr("4"))
        }
    }
    MenuItemGameVariant {
        gameVariant: "duo"
        text: addMnemonic(qsTr("Duo"),
                          //: Mnemonic for menu item Duo. Leave empty for no mnemonic.
                          qsTr("D"))
    }
    MenuItemGameVariant {
        gameVariant: "junior"
        text: addMnemonic(qsTr("Junior"),
                          //: Mnemonic for menu item Junior. Leave empty for no mnemonic.
                          qsTr("J"))
    }
    Pentobi.Menu {
        title: addMnemonic(qsTr("Trigon"),
                           //: Mnemonic for menu Trigon. Leave empty for no mnemonic.
                           qsTr("T"))

        MenuItemGameVariant {
            gameVariant: "trigon_2"
            text: addMnemonic(qsTr("Trigon (%1 Players)").arg(2), qsTr("2"))
        }
        MenuItemGameVariant {
            gameVariant: "trigon_3"
            text: addMnemonic(qsTr("Trigon (%1 Players)").arg(3), qsTr("3"))
        }
        MenuItemGameVariant {
            gameVariant: "trigon"
            text: addMnemonic(qsTr("Trigon (%1 Players)").arg(4), qsTr("4"))
        }
    }
    Pentobi.Menu {
        title: addMnemonic(qsTr("Nexos"),
                           //: Mnemonic for menu Nexos. Leave empty for no mnemonic.
                           qsTr("N"))

        MenuItemGameVariant {
            gameVariant: "nexos_2"
            text: addMnemonic(qsTr("Nexos (%1 Players)").arg(2), qsTr("2"))
        }
        MenuItemGameVariant {
            gameVariant: "nexos"
            text: addMnemonic(qsTr("Nexos (%1 Players)").arg(4), qsTr("4"))
        }
    }
    Pentobi.Menu {
        title: addMnemonic(qsTr("GembloQ"),
                           //: Mnemonic for menu GembloQ. Leave empty for no mnemonic.
                           qsTr("G"))

        MenuItemGameVariant {
            gameVariant: "gembloq_2"
            text: addMnemonic(qsTr("GembloQ (2 Players, 2 Colors)"), qsTr("2"))
        }
        MenuItemGameVariant {
            gameVariant: "gembloq_2_4"
            text: addMnemonic(qsTr("GembloQ (2 Players, 4 Colors)"),
                              //: Mnemonic for menu item GembloQ/Callisto (2 Players, 4 Colors). Leave empty for no mnemonic.
                              qsTr("P"))
        }
        MenuItemGameVariant {
            gameVariant: "gembloq_3"
            text: addMnemonic(qsTr("GembloQ (%1 Players)").arg(3), qsTr("3"))
        }
        MenuItemGameVariant {
            gameVariant: "gembloq"
            text: addMnemonic(qsTr("GembloQ (%1 Players)").arg(4), qsTr("4"))
        }
    }
    Pentobi.Menu {
        title: addMnemonic(qsTr("Callisto"),
                           //: Mnemonic for menu item Callisto. Leave empty for no mnemonic.
                           qsTr("A"))

        MenuItemGameVariant {
            gameVariant: "callisto_2"
            text: addMnemonic(qsTr("Callisto (2 Players, 2 Colors)"), qsTr("2"))
        }
        MenuItemGameVariant {
            gameVariant: "callisto_2_4"
            text: addMnemonic(qsTr("Callisto (2 Players, 4 Colors)"), qsTr("P"))
        }
        MenuItemGameVariant {
            gameVariant: "callisto_3"
            text: addMnemonic(qsTr("Callisto (%1 Players)").arg(3), qsTr("3"))
        }
        MenuItemGameVariant {
            gameVariant: "callisto"
            text: addMnemonic(qsTr("Callisto (%1 Players)").arg(4), qsTr("4"))
        }
    }
}
