import QtQuick 2.0

Loader {
    property Component component

    function open() {
        if (status === Loader.Null)
            sourceComponent = component
        item.open()
    }
}
