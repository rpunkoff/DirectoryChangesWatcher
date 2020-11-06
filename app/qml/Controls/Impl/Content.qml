import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as QuickControlsOne
import Controls.Base 1.0 as Base
import Theme 1.0
import DirectoryChangesWatcher 1.0

Rectangle {
    id: root

    opacity: Theme.backgroundOpacity
    color: Theme.backgroundColor

    property alias model: _tableView.model

    property bool checked

    signal started()

    Base.AppTableView {
        id: _tableView

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: _panel.top

        rowDelegate: Base.AppDelegate { }

        frameVisible: false

        style: TableViewDarkStyle { }

        QuickControlsOne.TableViewColumn {
            role: "name"
            width: _tableView.width / 7 * 5
            delegate: TableViewEditableCellDelegate {
                id: _delegateName
//                textAnchors.left: _delegateName.left
//                textAnchors.leftMargin: Theme.mediumOffset

                elideMode: styleData.elideMode
                canEdit: model === null ? false : !model.isDirectory
                itemSelected: styleData.selected
                itemData: styleData.value

                onRenamed: {
                    _tableView.selection.clear()
                    FileRenamer.rename(oldName, newName)
                }
            }
        }

        QuickControlsOne.TableViewColumn {
            role: "date"
            width:  _tableView.width / 7
            delegate: TableViewCellDelegate {
                id: _delegateDate
                textAnchors.right: _delegateDate.right
                textAnchors.rightMargin: Theme.mediumOffset
                itemSelected: styleData.selected
                itemData: styleData.value
                elideMode: Qt.ElideLeft
                horizontalAlignment: Qt.AlignRight
            }
        }

        QuickControlsOne.TableViewColumn {
            role: "size"
            width: _tableView.__verticalScrollBar.visible ? _tableView.width / 7 - _tableView.__verticalScrollBar.width :
                                                            _tableView.width / 7
            delegate: TableViewCellDelegate {
                id: _delegateSize
                textAnchors.right: _delegateSize.right
                textAnchors.rightMargin: Theme.mediumOffset
                itemSelected: styleData.selected
                itemData: styleData.value
                elideMode: Qt.ElideLeft
                horizontalAlignment: Qt.AlignRight
            }
        }
    }

    Base.AppPanel {
        id: _panel

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: "transparent"

        Base.AppButton {
            id: _btn

            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top

            anchors.rightMargin: Theme.mediumOffset
            anchors.topMargin: Theme.tinyOffset
            anchors.bottomMargin: Theme.tinyOffset

            text: root.checked ? "Stop" : "Watch"

            onClicked: started()
        }
    }
}
