import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

StatusBar
{
    RowLayout {
        Label {
            text: {
                if (gameDisplay.setupMode)
                    return qsTr("Setup mode")
                if (playerModel.isGenMoveRunning)
                    return qsTr("Computer is thinking...")
                if (analyzeGameModel.isRunning)
                    return qsTr("Analyzing game...")
                if (isRated)
                    return qsTr("Rated game")
                return ""
            }
        }
    }
}
