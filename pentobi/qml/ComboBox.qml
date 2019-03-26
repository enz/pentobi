//-----------------------------------------------------------------------------
/** @file pentobi/qml/ComboBox.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.4

/** Custom ComboBox.
    Explicitely set text color to work around QTBUG-74678 (ComboBox with
    default style uses white text on light background for highlighted items,
    last tested with Qt 5.12.2 and 5.13.0-beta1). Using custom colors also
    ensures that popup menu uses the same colors as our custom MenuItem. */
ComboBox {
    id: root

    delegate: ItemDelegate {
        width: root.width
        text: modelData
        highlighted: ListView.isCurrentItem
        contentItem: Label {
            text: parent.text
            // See comment at our custom MenuItem.contentItem/background
            color: {
                if (parent.highlighted)
                    return isDesktop ? palette.highlightedText
                                     : palette.buttonText
                return palette.text
            }
        }
        background: Rectangle {
            // See comment at our custom MenuItem.contentItem/background
            color: {
                if (! parent.highlighted)
                    return "transparent"
                return isDesktop ? palette.highlight : palette.midlight
            }
        }
    }
}
