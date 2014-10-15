import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.1
import MusIk 1.0

ApplicationWindow {
    id: applicationWindow

    property var resolutions: [
        {"height": 480, "width": 320, "name": "HVGA", "ratio": "3:2"},
        {"height": 640, "width": 360, "name": "nHD", "ratio": "16:9"},
        {"height": 640, "width": 480, "name": "VGA", "ratio": "4:3"},
        {"height": 800, "width": 480, "name": "WVGA", "ratio": "5:3"},
        {"height": 800, "width": 600, "name": "SVGA", "ratio": "4:3"},
        {"height": 960, "width": 540, "name": "qHD", "ratio": "16:9"},
        {"height": 1280, "width": 720, "name": "720p", "ratio": "16:9"},
        {"height": 1280, "width": 800, "name": "WXGA", "ratio": "16:10"},
        {"height": 1920, "width": 1080, "name": "1080p", "ratio": "16:9"}
    ]

    property int currentResolution: 3
    property bool isScreenPortrait: height >= width

    visible: true
    width: resolutions[currentResolution]["width"]
    height: resolutions[currentResolution]["height"]
    title: "MusIk"

    QtObject {
        id: selectedArtist

        property string artist: ""
        property string picture: ""
    }

    QtObject {
        id: currentSong

        property string name: ""
        property string artist: ""
    }

    FontLoader { id: fontI; source: "qrc:/fonts/resources/fonts/Muli-Italic.ttf" }
    FontLoader { id: fontL; source: "qrc:/fonts/resources/fonts/Muli-Light.ttf" }
    FontLoader { id: fontLI; source: "qrc:/fonts/resources/fonts/Muli-LightItalic.ttf" }
    FontLoader { id: fontN; source: "qrc:/fonts/resources/fonts/Muli-Regular.ttf" }

    Theme {
        id: theme

        fontFamily: fontN.name
    }

    Audio {
        id: audioElement

        autoLoad: true
        autoPlay: true
    }

    Connections {
        target: musicStreamer
        /// TODO: handle errors that end with "server replied: Not Found"
        onServerError: ui.showMessage("Error, do you have internet connection? Please try again..")
    }

    Image {
        anchors.fill: parent
        antialiasing: true
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/images/bg-" + Math.round((Math.random())) + (height > width ?  "" : "-landscape")
    }

    ListModel {
        id: dataModel

        ListElement { artist: "Adele"; picture: "ad" }
        ListElement { artist: "Artic Monkeys"; picture: "am" }
        ListElement { artist: "Avicii"; picture: "av" }
        ListElement { artist: "Coldplay"; picture: "cp" }
        ListElement { artist: "Calvin Harris"; picture: "ch" }
        ListElement { artist: "Daft Punk"; picture: "dp" }
        ListElement { artist: "Franz Ferdinand"; picture: "ff" }
        ListElement { artist: "Imagine Dragons"; picture: "id" }
        ListElement { artist: "Lana Del Rey"; picture: "lr" }
        ListElement { artist: "Lorde"; picture: "ld" }
        ListElement { artist: "Radiohead"; picture: "rh" }
        ListElement { artist: "The Killers"; picture: "ki" }
        ListElement { artist: "The Ting Tings"; picture: "tt" }
        ListElement { artist: "The White Stripes"; picture: "ws" }
        ListElement { artist: "Yeah Yeah Yeahs"; picture: "yy" }
        ListElement { artist: "Zoe"; picture: "ze" }
    }

    StackView {
        id: stackView

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: playbackControls.top
        }

        initialItem: mainPage
        focus: true

        Behavior on height { NumberAnimation { easing.type: Easing.InOutQuad } }

        Keys.onBackPressed: {
            if (stackView.depth > 1) {
                if (stackView.currentItem === musicPage)
                    musicStreamer.cancelSearch()

                stackView.pop()
            } else {
                event.accepted = false
            }
        }
    }

    PlaybackControls {
        id: playbackControls

        anchors {
            bottom: parent.bottom; bottomMargin: audioElement.source != "" ? 0 : -playbackControls.height
        }

        song: currentSong.name + (currentSong.artist === "" ? "" : " - " + currentSong.artist)
        Behavior on y { NumberAnimation { easing.type: Easing.InOutQuad } }
    }

    Component {
        id: mainPage

        MainPage { }
    }

    Component {
        id: musicPage

        MusicPage { }
    }
}
