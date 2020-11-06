import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import DirectoryChangesWatcher 1.0
import Controls.Impl 1.0 as Impl

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    minimumWidth: 640
    minimumHeight: 480
    title: qsTr("Directory changes watcher")

    FilesModel {
        id: _model
    }

    DirectoryChangesController {
        id: _controller
        onNotify: {
            _model.handle(fInfo)
        }
        onFailed: {
            console.log("Error: ", errorString);
            stop();
        }
    }

    Component.onDestruction: {
        if(_controller.running) {
            _controller.stop();
            _model.clear();
        }
    }

    header: Impl.Header {
        id: _header

        anchors.left: parent.left
        anchors.right: parent.right

        labelText: "Watch in:"
        placeholderText: "Select a directory for watching..."

        enabled: !_content.checked
    }

    footer: Impl.Footer {
        id: _footer
        anchors.left: parent.left
        anchors.right: parent.right

        labelText: "Files Count:"
        text: _model.count
    }

    MessageDialog {
        id: _messageDialog
        title: window.title
        text: "No directory selected."
    }

    Impl.Content {
        id: _content

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: _header.bottom
        anchors.bottom: footer.top

        model: _model

        checked: _controller.running

        onStarted: {
            if(!_content.checked) {
                if(_header.pathText.length === 0) {
                    _messageDialog.open();
                } else {
                    _model.clear();
                    _controller.run(header.pathText)
                }
            } else {
                _controller.stop();
            }
        }
    }
}
