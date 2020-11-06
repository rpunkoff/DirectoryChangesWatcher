import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0

Text {
    id: control

    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignLeft

    font.pointSize: Theme.labelFontSize
    fontSizeMode: Label.Fit

    color: Theme.textColor
}
