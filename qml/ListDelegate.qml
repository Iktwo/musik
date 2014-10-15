import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import MusIk 1.0

Item {
    id: root

    signal play()
    signal download()

    height: 84 * ScreenValues.dp
    width: parent.width

    RowLayout {
        anchors {
            fill: parent
            margins: 8 * ScreenValues.dp
        }

        Label {
            id: songName

            font {
                pixelSize: 18 * ScreenValues.dp
                family: theme.fontFamily
            }

            Layout.fillWidth: true
            color: "#fafafa"
            elide: Text.ElideRight
            wrapMode: "Wrap"
            text: model.name.trim()
            renderType: Text.NativeRendering
            maximumLineCount: 2
        }

        RowLayout {
            id: row

            anchors.right: parent.right

            spacing: 4 * ScreenValues.dp
            Layout.preferredWidth: spacing + (48 * ScreenValues.dp * 2)
            Layout.fillHeight: true

            Item {
                Layout.fillHeight: true
                Layout.preferredWidth: 48 * ScreenValues.dp

                ImageButton {
                    anchors.fill: parent
                    source: "qrc:/images/" + theme.getBestIconSize(Math.min(icon.height, icon.width)) + "play"
                    visible: model.url === "" ? false : true

                    onClicked: root.play()
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.preferredWidth: 48 * ScreenValues.dp

                ImageButton {
                    anchors.fill: parent
                    source: "qrc:/images/" + theme.getBestIconSize(Math.min(icon.height, icon.width)) + "download"
                    visible: model.url === "" ? false : true

                    onClicked: root.download()
                }
            }
        }
    }
}
