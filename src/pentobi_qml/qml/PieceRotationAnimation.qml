import QtQuick 2.0

/* Note: This type could be unified with PieceFlipAnimation if we use
   the same duration for both animations because RotationAnimation
   automatically targets all properties named "angle" or "rotation".
   But wait until QTBUG-22141 is fixed, right now the explicit naming
   of the property avoids faulty warning messages. */
RotationAnimation {
    duration: 300
    direction: RotationAnimation.Shortest
    property: "rotation"
}
