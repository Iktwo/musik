import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtMultimedia 5.1
import MusIk 1.0

Rectangle {
    property Audio audio: audioElement
    property alias song: songLabel.text

    function formatMilliseconds(ms) {
        var hours = Math.floor((((ms / 1000) / 60) / 60) % 60).toString()
        var minutes = Math.floor(((ms / 1000) / 60) % 60).toString()
        var seconds = Math.floor((ms / 1000) % 60).toString()

        var time = ""

        if (hours > 0)
            time += hours + ":"

        if (minutes < 10)
            time += "0" + minutes + ":"
        else
            time += minutes + ":"

        if (seconds < 10)
            time += "0" + seconds
        else
            time += seconds

        return time
    }

    color: "#88000000"
    height: column.height + 1 * ScreenValues.dp
    width: parent.width

    ColumnLayout {
        id: column

        Connections {
            target: audio
            onDurationChanged: progressBar.maximumValue = audio.duration
            onPositionChanged: progressBar.value = audio.position
        }

        anchors {
            left: parent.left
            right: parent.right
        }

        ProgressBar {
            id: progressBar

            maximumValue: audio.duration
            minimumValue: 0
            value: audio.position
            width: parent.width

            style: ProgressBarStyle {
                background: Rectangle {
                    color: "#fafafa"
                    implicitWidth: control.width
                    implicitHeight: 4 * ScreenValues.dp
                }
                progress: Rectangle {
                    color: "#9b59b6"
                }
            }

            MouseArea {
                anchors {
                    fill: parent
                    margins: -0.04 * ScreenValues.dpi
                }

                onClicked: {
                    if (audio.seekable)
                        audio.seek((mouseX / parent.width) * audio.duration)
                }
            }
        }

        Item {
            height: playBtn.height

            anchors {
                left: parent.left
                right: parent.right
                margins: 1 * ScreenValues.dp
            }

            RowLayout {
                anchors.fill: parent

                Item {
                    height: 1
                    Layout.preferredWidth: 4 * ScreenValues.dp
                }

                Label {
                    id: labelPosition

                    /// TODO: add animation where this flashes if paused

                    color: "#fafafa"
                    height: parent.height
                    verticalAlignment: "AlignVCenter"
                    text: formatMilliseconds(audio.position)
                    renderType: Text.NativeRendering

                    font {
                        pixelSize: 12 * ScreenValues.dp
                        family: theme.fontFamily
                        bold: audio.playbackState === Audio.PausedState && audio.status !== Audio.Stalled
                    }

                    SequentialAnimation {
                        running: audio.playbackState === Audio.PausedState && audio.status !== Audio.Stalled
                        loops: Animation.Infinite
                        alwaysRunToEnd: true
                        ColorAnimation { target: labelPosition; properties: "color"; to: "#9b59b6"; duration: 750 }
                        ColorAnimation { target: labelPosition; properties: "color"; to: "#fafafa"; duration: 750 }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
                    spacing: 8 * ScreenValues.dp

                    ImageButton {
                        id: playBtn

                        height: 48 * ScreenValues.dp
                        width: 48 * ScreenValues.dp

                        /// TODO: verify this logic
                        source: "qrc:/images/" + theme.getBestIconSize(Math.min(icon.height, icon.width)) + (audio.playbackState == Audio.PlayingState || (audio.status == Audio.Buffering || audio.status == Audio.Stalled) && audio.playbackState != Audio.PausedState ? "pause" : "play")

                        onClicked: {
                            if (audio.playbackState == Audio.PlayingState)
                                audio.pause();
                            else if (audio.source != "")
                                audio.play();
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Label {
                    color: "#fafafa"
                    text: formatMilliseconds(audio.duration)
                    horizontalAlignment: "AlignRight"
                    renderType: Text.NativeRendering

                    font {
                        pixelSize: 12 * ScreenValues.dp
                        family: theme.fontFamily
                    }
                }

                Item {
                    height: 1
                    Layout.preferredWidth: 4 * ScreenValues.dp
                }
            }
        }

        Label {
            id: songLabel

            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width - (16 * ScreenValues.dp)
            Layout.preferredWidth: parent.width - (16 * ScreenValues.dp)
            color: "#fafafa"
            renderType: Text.NativeRendering

            font {
                pixelSize: 12 * ScreenValues.dp
                family: theme.fontFamily
            }
        }
    }
}
