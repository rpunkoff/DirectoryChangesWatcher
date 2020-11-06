import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0

AppRectangle {
    id: root

    property variant itemData
    property bool itemSelected: false
    property var horizontalAlignment: Qt.AlignLeft

    height: Theme.panelHeight
}
