import QtQuick 2.12
import QtQuick.Controls 2.12
import Controls.Base 1.0 as Base
import Theme 1.0

Base.AppDelegate {
    id: root

    property alias textAnchors: _label.anchors
    property alias elideMode: _label.elide
    property alias textVisible: _label.visible

    height: 25

    Base.AppLabel {
        id: _label
        anchors.fill: parent

        horizontalAlignment: root.horizontalAlignment

        text: root.itemData === undefined ? "" : root.itemData
    }

    color: Theme.panelsColor
    opacity: itemSelected ? Theme.secondaryOpacity : Theme.emphasisOpacity
}
