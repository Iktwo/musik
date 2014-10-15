#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "uivalues.h"
#include "screenvalues.h"
#include "musicstreamer.h"

static QObject *screen_values_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    ScreenValues *screenValues = new ScreenValues();
    return screenValues;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QCoreApplication::setOrganizationName("Iktwo");
    QCoreApplication::setOrganizationDomain("iktwo.com");
    QCoreApplication::setApplicationName("MusIk");

    qmlRegisterSingletonType<ScreenValues>("MusIk", 1, 0, "ScreenValues", screen_values_provider);

    UIValues uiValues;
    engine.rootContext()->setContextProperty("ui", &uiValues);

    MusicStreamer musicStreamer;
    engine.rootContext()->setContextProperty("musicStreamer", &musicStreamer);

    engine.load(QUrl(QStringLiteral("qrc:/qml/qml/main.qml")));

    return app.exec();
}
