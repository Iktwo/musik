import QtQuick 2.3
import QtQuick.Controls 1.2
import MusIk 1.0

Page {
    id: root

    property bool busy: false

    titleBar: TitleBar {
        title: "MusIk"
        titleLabel.font.family: fontN.name
        isScreenPortrait: applicationWindow.isScreenPortrait
    }

    Connections {
        target: stackView
        onCurrentItemChanged: root.busy = false
    }

    ScrollView {
        anchors.fill: parent

        flickableItem.interactive: true
        focus: true

        GridView {
            anchors.fill: parent

            model: dataModel
            snapMode: GridView.SnapToRow

            cacheBuffer: applicationWindow.height * 4
            cellWidth: parent.width / 2
            cellHeight: parent.height / 2.4

            focus: true

            delegate: Item {
                id: itemDelegate

                height: GridView.view.cellHeight
                width: GridView.view.cellWidth

                Image {
                    anchors.centerIn: parent

                    antialiasing: true
                    asynchronous: true

                    height: parent.height > parent.width ? parent.width * 0.95 : parent.height * 0.95
                    width: parent.height > parent.width ? parent.width * 0.95 : parent.height * 0.95

                    source: "qrc:/images/" + model.picture + "-small"

                    Rectangle {
                        anchors.bottom: parent.bottom

                        width: parent.width
                        color: "#88000000"
                        height: Math.min(labelArtist.height, parent.height)

                        Label {
                            id: labelArtist

                            anchors {
                                left: parent.left; leftMargin: 4 * ScreenValues.dp
                                right: parent.right; rightMargin: 4 * ScreenValues.dp
                            }

                            font {
                                weight: Font.Light
                                family: fontN.name
                                pixelSize: 20 * ScreenValues.dp
                            }

                            wrapMode: "Wrap"
                            maximumLineCount: 2
                            elide: "ElideRight"
                            text: model.artist
                            color: "#ecf0f1"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            if (!root.busy) {
                                root.busy = true

                                musicStreamer.cancelSearch()

                                selectedArtist.artist = model.artist
                                selectedArtist.picture = model.picture

                                stackView.push(musicPage)
                            }
                        }
                    }
                }
            }
        }
    }
}
