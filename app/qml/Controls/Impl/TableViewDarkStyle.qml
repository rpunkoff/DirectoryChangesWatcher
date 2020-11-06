import QtQuick 2.12
import QtQuick.Controls.Styles 1.4
import Theme 1.0

TableViewStyle {
        frame: Item { }

        backgroundColor: "transparent"

        corner: Item { }

        handle: Item {
            implicitWidth: Theme.mediumSpacing
            implicitHeight: Theme.bigSpacing
            Rectangle {
                opacity: styleData.pressed ? Theme.secondaryOpacity : Theme.defaultOpacity
                color: Theme.scrollColor
                anchors.fill: parent
                radius: 5
                anchors.topMargin: 3
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.bottomMargin: 3
            }
        }

        scrollBarBackground: Item {
            implicitWidth: Theme.mediumSpacing
            implicitHeight: Theme.bigSpacing
        }

        incrementControl: Item { }
        decrementControl: Item { }
    }
