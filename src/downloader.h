#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QVariantMap>

#ifdef Q_OS_ANDROID
#include "androiddownloadmanager.h"
#endif

//! Downloader class.

/*!
Downloader class provides functions to download files using http.
*/

class QNetworkCookie;
class QNetworkReply;
class QNetworkAccessManager;
class Downloader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool downloading READ isDownloading NOTIFY downloadingChanged)
    Q_PROPERTY(int activeConnections READ activeConnections NOTIFY activeConnectionsChanged)

public:
    explicit Downloader(QObject *parent = 0);
    ~Downloader();

    static QString DownloadUrl;
    static QString TargetDir;

    bool isDownloading() const;

    int activeConnections() const;
    void setActiveConnections(int activeConnections);

public slots:
    void downloadSong(const QString &name, const QString &url);
    void getDownloadLink(const QString &code);
    void download(const QString &urlString);
    void search(const QString &term);
    void downloadFinished(QNetworkReply *reply);
    void cancelSearch();

signals:
    void songFound(const QString &title, const QString &artist, const QString &length,
                   const QString &comment, int kbps, const QString &code,
                   const QString &picture, long long hits);

    void decodedUrl(const QString &code, const QString &url);
    void searchEnded();
    void songDownloaded(const QString &url);
    void serverError();
    void searchHasMoreResults(const QString &url);
    void searchHasNoMoreResults();
    void downloadingChanged();
    void progressChanged(float progress, const QString &name);
    void activeConnectionsChanged();
    void noResults();

private:
    QNetworkAccessManager *m_netAccess;
    QMap<QString, QString> m_subdirs;
    QVariantMap m_songsToDownload;
    QList<QNetworkReply *> m_replies;
    bool m_downloading;
    QList<QNetworkCookie> mCookies;
    int m_activeConnections;

    void setDownloading(bool downloading);

private slots:
    QString decodeHtml(const QString &html);
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

#ifdef Q_OS_ANDROID
    AndroidDownloadManager mAdm;
#endif
};

#endif // DOWNLOADER_H
