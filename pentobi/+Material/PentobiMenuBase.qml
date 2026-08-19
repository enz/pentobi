//-----------------------------------------------------------------------------
/** @file pentobi/+Material/PentobiMenuBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

Menu {
    // Material style on Android highlights first item in menu even if opened
    // with touch (see also QTBUG-69541)
    onAboutToShow: currentIndex = -1
}
