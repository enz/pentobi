//-----------------------------------------------------------------------------
/** @file pentobi/qml/PentobiComboBox.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.4

/** Custom ComboBox.
    Using custom colors ensures that popup menu uses the same colors as
    PentobiMenuItem. */
ComboBox {
    id: root

    delegate: ItemDelegate {
        width: root.width
        text: modelData
        highlighted: ListView.isCurrentItem
        contentItem: Label {
            text: parent.text
            // See comment at PentobiMenuItem.contentItem/background
            color: {
                if (parent.highlighted)
                    return isDesktop ? palette.highlightedText
                                     : palette.buttonText
                return palette.text
            }
        }
        background: Rectangle {
            // See comment at PentobiMenuItem.contentItem/background
            color: {
                if (! parent.highlighted)
                    return "transparent"
                return isDesktop ? palette.highlight : palette.midlight
            }
        }
    }
}
