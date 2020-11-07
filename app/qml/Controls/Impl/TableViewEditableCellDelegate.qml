import QtQuick 2.12
import QtQuick.Controls 2.12
import Controls.Base 1.0 as Base
import Theme 1.0

Base.AppDelegate {
    id: root

    signal renamed(string oldName, string newName)

    property var elideMode : Qt.ElideLeft
    property bool canEdit

    height: 25

    color: Theme.panelsColor
    opacity: itemSelected ? Theme.secondaryOpacity : Theme.emphasisOpacity

    Loader {
        id: _loader
        anchors.fill: parent
        anchors.margins: Theme.smallOffset

        sourceComponent: root.canEdit ? root.itemSelected ? _editor : _viewer : _viewer

        Component {
            id: _viewer

            Base.AppLabel {
                id: _label
                anchors.fill: parent

                horizontalAlignment: root.horizontalAlignment

                text: root.itemData === undefined ? "" : root.itemData

                color: root.itemSelected ? Theme.themeInvertedColor : Theme.textColor

                elide: root.elideMode
            }
        }

        Component {
            id: _editor

            TextInput {
                id: _textinput
                color: Theme.textColor
                anchors.leftMargin: 15
                property string prefix
                property string visibleText
                text: visibleText
                font.pointSize: 11
                font.bold: true
                font.italic: true

                Component.onCompleted: {
                    //get the full path to file
                    var txt = root.itemData
                    //looking for the last "/" charachter
                    var lastIndex = txt.lastIndexOf('/')
                    if(lastIndex > -1) {
                        //get the full path to dir
                        _textinput.prefix = txt.substring(0, lastIndex + 1)
                        //get the file name
                        _textinput.visibleText = txt.substring(lastIndex + 1, txt.length)
                    }else {
                        //get the file name
                        _textinput.visibleText = txt
                    }
                }

                function rename() {
                    var txt = _textinput.prefix + _textinput.text
                    if(txt !== root.itemData) {
                        root.renamed(root.itemData, txt);
                    }
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        rename();
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: _textinput.forceActiveFocus()
                }
            }
        }
    }
}
