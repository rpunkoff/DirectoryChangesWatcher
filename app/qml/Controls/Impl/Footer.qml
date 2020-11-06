import QtQuick 2.12
import QtQuick.Controls 2.12
import Controls.Base 1.0 as Base
import Theme 1.0

Base.AppPanel {
    property alias labelText: _lbl.text
    property alias text: _content.text

    Base.AppLabel {
        id:_lbl

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: Theme.mediumOffset
    }

    Base.AppLabel {
        id: _content
        font.bold: true

        anchors.left: _lbl.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: Theme.smallOffset
    }
}
