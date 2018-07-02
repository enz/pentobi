import QtQuick 2.3

/** Image used in piece elements.
    Using mipmap produces better results when scaling the pieces, but because
    of QTBUG-57845, we don't use it on the desktop. Instead antialiasing is
    true by default on the desktop (overridden for some game variants, where
    antialiasing always produces better or always produces worse results) */
Image {
    mipmap: isAndroid
    antialiasing: ! isAndroid
}
