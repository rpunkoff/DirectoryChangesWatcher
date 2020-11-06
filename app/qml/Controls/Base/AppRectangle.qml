import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0

Rectangle {
    id: control

    color: Theme.backgroundColor
    opacity: Theme.emphasisOpacity

    border.color: Theme.backgroundColor
    border.width: Theme.defaultBorderWidth
    radius: Theme.defaultBorderRadius
}
