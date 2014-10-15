#include "androiddownloadmanager.h"

#include <QAndroidJniObject>

AndroidDownloadManager::AndroidDownloadManager(QObject *parent) :
    QObject(parent)
{
}

void AndroidDownloadManager::downloadFile(const QString &url, const QString &fileName)
{
    QAndroidJniObject jUrl = QAndroidJniObject::fromString(url);
    QAndroidJniObject jFileName = QAndroidJniObject::fromString(fileName);

    QAndroidJniObject::callStaticMethod<void>("com/iktwo/musik/MusIk",
                                              "download",
                                              "(Ljava/lang/String;Ljava/lang/String;)V",
                                              jUrl.object<jstring>(),
                                              jFileName.object<jstring>());
}
