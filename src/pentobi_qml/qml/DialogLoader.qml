import QtQuick 2.0

Loader {
    property url url

    function open() {
        if (status === Loader.Null)
            source = url
        item.open()
    }
}
