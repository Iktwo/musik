import QtQuick 2.3
import QtQuick.Controls 1.1
import MusIk 1.0

FocusScope {
    id: root

    property alias color: container.color
    property alias title: titleLabel.text
    property alias titleLabel: titleLabel
    property alias titleColor: titleLabel.color
    property string iconSource
    property bool isScreenPortrait: false

    signal iconClicked()

    objectName: "titleBar"

    width: parent.width
    height: Math.ceil(ScreenValues.dp * (ui.isTablet ? 56 : (isScreenPortrait ? 48 : 40)))

    anchors.top: parent.top

    Rectangle {
        id: container

        anchors.fill: parent
        color: "#88000000"

        Item {
            id: iconImage

            anchors {
                left: parent.left; leftMargin: 4 * ScreenValues.dp
            }

            height: parent.height
            width: ScreenValues.dp * 48

            Image {
                anchors.centerIn: parent
                height: 32 * ScreenValues.dp
                width: 32 * ScreenValues.dp
                source: iconSource != "" ? iconSource : ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: iconClicked()
            }
        }


        Label {
            id: titleLabel

            anchors {
                left: iconSource != "" ? iconImage.right : parent.left; leftMargin: ScreenValues.dp * (iconSource != "" ? 0 : 8)
                verticalCenter: parent.verticalCenter
            }

            font.pixelSize: 18 * ScreenValues.dp

            color: "#ecf0f1"
        }
    }
}
