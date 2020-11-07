import QtQuick 2.12
import QtQuick.Controls 2.12
import Controls.Base 1.0 as Base
import QtQuick.Dialogs 1.2
import Theme 1.0
import DirectoryChangesWatcher 1.0

Base.AppPanel {
    id: root

    property alias labelText: _lbl.text
    property alias placeholderText: _rect.placeholderText
    property alias pathText : _pathLbl.fullText

    signal selected(string path)

    function openDialog() {
        _fileDialog.open();
    }

    Base.AppLabel {
        id: _lbl

        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.left

        anchors.leftMargin: Theme.mediumOffset
    }

    Base.AppRectangle {
        id: _rect

        anchors.left: _lbl.right
        anchors.right: _browseBtn.left
        anchors.top: root.top
        anchors.bottom: root.bottom

        anchors.leftMargin: Theme.smallOffset
        anchors.rightMargin: Theme.smallOffset
        anchors.topMargin: Theme.smallOffset
        anchors.bottomMargin: Theme.smallOffset

        property string placeholderText

        Base.AppLabel {
               id: _pathLbl

               anchors.top: _rect.top
               anchors.bottom: _rect.bottom
               anchors.left: _rect.left
               anchors.right: _rect.right

               anchors.leftMargin: Theme.smallOffset
               anchors.rightMargin: Theme.smallOffset

               property string fullText

               text: _txtMetrics.elidedText.length === 0 ? _rect.placeholderText : _txtMetrics.elidedText
               opacity: _txtMetrics.elidedText.length === 0 ? Theme.emphasisOpacity : Theme.defaultOpacity
               font.italic: fullText.length == 0
           }

        TextMetrics {
            id: _txtMetrics
            font: _pathLbl.font
            elide: Text.ElideMiddle
            text: _pathLbl.fullText
            elideWidth: _pathLbl.width - 4
        }

        FileDialog {
            id: _fileDialog
            title: "Select a directory to observe"
            selectMultiple: false
            selectFolder: true
            folder: "."
            onAccepted: {
                var path = UrlFormatter.format(_fileDialog.fileUrl.toString())
                if(_pathLbl.text !== path) {
                    _pathLbl.fullText = path
                      selected(_pathLbl.fullText)
                }
            }
        }
    }

    Base.AppButton {
        id:_browseBtn
        text: "..."
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.right: root.right

        anchors.rightMargin: Theme.mediumOffset
        anchors.topMargin: Theme.tinyOffset
        anchors.bottomMargin: Theme.tinyOffset

        onClicked: _fileDialog.open();
    }
}
