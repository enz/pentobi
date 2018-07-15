import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("G&o")

    Pentobi.MenuItem { action: actions.actionGoto }
    Pentobi.MenuItem { action: actions.actionBackToMainVar }
    Pentobi.MenuItem { action: actions.actionBeginningOfBranch }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionFindNextComment }
}
