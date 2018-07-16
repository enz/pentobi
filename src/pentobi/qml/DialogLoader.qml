import QtQuick 2.0

Loader {
    property Component component

    function get() {
        if (status === Loader.Null)
            sourceComponent = component
        return item
    }
    function open() { get().open() }
}
