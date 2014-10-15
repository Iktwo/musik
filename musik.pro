QT += qml quick widgets svg network multimedia

android {
    QT += androidextras gui-private
    SOURCES += \
        src/androiddownloadmanager.cpp

    HEADERS += \
        src/androiddownloadmanager.h
}

TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/uivalues.cpp \
    src/screenvalues.cpp \
    src/downloader.cpp \
    src/musicstreamer.cpp \
    src/song.cpp

HEADERS += \
    src/uivalues.h \
    src/screenvalues.h \
    src/downloader.h \
    src/musicstreamer.h \
    src/song.h

OTHER_FILES += \
    qml/*.qml \
    android/AndroidManifest.xml \
    android/src/com/iktwo/musik/MusIk.java

RESOURCES += resources.qrc

include(deployment.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

