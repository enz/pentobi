import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("G&o"))

    MenuItem { action: actions.actionGoto }
    MenuItem { action: actions.actionBackToMainVar }
    MenuItem { action: actions.actionBeginningOfBranch }
    MenuSeparator { }
    MenuItem { action: actions.actionFindNextComment }
}
