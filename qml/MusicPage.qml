import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import MusIk 1.0

Page {
    id: root

    titleBar: TitleBar {
        title: selectedArtist.artist
        titleLabel.font.family: fontN.name
        iconSource: "qrc:/images/" + theme.getBestIconSize(height) + "back"
        isScreenPortrait: applicationWindow.isScreenPortrait

        onIconClicked: {
            musicStreamer.cancelSearch()
            stackView.pop()
        }
    }

    onActivated: timerSearchDelay.restart()

    Timer {
        id: timerSearchDelay

        interval: 650
        onTriggered: {
            if (stackView.currentItem == root)
                musicStreamer.search(selectedArtist.artist)
        }
    }

    Image {
        id: image

        anchors.fill: parent

        antialiasing: true
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/images/" + selectedArtist.picture
        visible: false

        Rectangle {
            anchors.fill: parent
            opacity: 0.2
            color: "#ffffff"
        }
    }

    FastBlur {
        anchors.fill: image
        source: image
        radius: 32

        Component {
            id: busyFooter
            Item {
                id: busyFooterContainer
                width: listResults.width
                height: musicStreamer.activeConnections > 0 && listResults.count > 0 ? 48 * ScreenValues.dp : 0

                BusyIndicator {
                    anchors.centerIn: parent
                    height: parent.height - 8 * ScreenValues.dp
                    width: height
                    running: parent.height > 0
                    style: BusyIndicatorStyle {
                        indicator: Image {
                            id: busyIndicator
                            visible: control.running
                            height: control.height
                            width: control.width
                            source: "qrc:/images/" + theme.getBestIconSize(height) + "busy_dark"
                            antialiasing: true
                            RotationAnimator {
                                target: busyIndicator
                                running: control.running
                                loops: Animation.Infinite
                                duration: 2000
                                from: 0; to: 360
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            anchors.fill: parent

            color: "#88000000"
        }

        ScrollView {
            anchors.fill: parent

            flickableItem.interactive: true
            focus: true

            ListView {
                id: listResults

                anchors.fill: parent
                clip: true
                footer: busyFooter
                model: musicStreamer
                delegate: ListDelegate {
                    onPlay: {
                        currentSong.name = model.name
                        currentSong.artist = model.comment

                        audioElement.source = model.url
                    }

                    onDownload: {
                        musicStreamer.downloadSong(model.name, model.url)
                    }
                }

                onContentYChanged: {
                    if (contentHeight != 0) {
                        // if (!musicStreamer.searching && ((contentY + height) / contentHeight) > 0.85)
                        if (musicStreamer.activeConnections === 0 && atYEnd)
                            musicStreamer.fetchMoreResulst()
                    }
                }
            }
        }

        Item {
            anchors.fill: parent

            opacity: busyIndicatorComponent.running ? 1 : 0

            BusyIndicator {
                id: busyIndicatorComponent
                anchors.centerIn: parent
                running: (musicStreamer.searching || musicStreamer.activeConnections > 0 || timerSearchDelay.running) && listResults.count === 0
                height: (applicationWindow.height > applicationWindow.width ? applicationWindow.width : applicationWindow.height) * 0.4
                width: height

                style: BusyIndicatorStyle {
                    indicator: Image {
                        id: busyIndicator
                        visible: control.running
                        source: "qrc:/images/" + theme.getBestIconSize(height) + "busy"
                        antialiasing: true
                        RotationAnimator {
                            target: busyIndicator
                            running: control.running
                            loops: Animation.Infinite
                            duration: 2000
                            from: 0; to: 360
                        }
                    }
                }
            }
        }
    }
}
