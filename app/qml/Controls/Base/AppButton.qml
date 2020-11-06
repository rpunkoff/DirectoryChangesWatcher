import QtQuick 2.12
import QtQuick.Controls 2.12 as QuickControls
import Theme 1.0

QuickControls.Button {
    id: control

    font.pointSize: Theme.labelFontSize

    contentItem: Text {
            text: control.text
            font: control.font
            opacity: control.enabled ? Theme.backgroundOpacity : Theme.emphasisOpacity
            color: control.down ? "#f1f1f1" : Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

    background: AppRectangle {
        implicitWidth: 100
        implicitHeight: 25
        opacity: control.enabled ? Theme.backgroundOpacity : Theme.secondaryOpacity
        border.color: control.down ? Theme.backgroundColor : Theme.panelsColor
        color:  control.down ? Theme.panelsColor : Theme.backgroundColor
    }
}
