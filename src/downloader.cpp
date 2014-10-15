#include "downloader.h"

#include <QDebug>
#include <QtNetwork>
#include <QDir>
#include <QFile>

#include "uivalues.h"

#define COOKIE_CREATOR_URL "http://www.goear.com/listen/"
#define TARGET_URL "http://www.goear.com/"
#define SEARCH_URL "http://www.goear.com/search/"

QString Downloader::DownloadUrl("http://www.goear.com/action/sound/get/");

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    m_downloading(false),
    m_activeConnections(0)
{
    m_netAccess = new QNetworkAccessManager(this);
    connect(m_netAccess, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}

Downloader::~Downloader()
{
    delete m_netAccess;
}

void Downloader::downloadSong(const QString &name, const QString &url)
{
#ifdef Q_OS_ANDROID
    mAdm.downloadFile(url, name.simplified());
#else
    m_songsToDownload.insert(url, name);
    download(url);
    setDownloading(true);
#endif
}

void Downloader::getDownloadLink(const QString &code)
{
    //    qDebug() << Q_FUNC_INFO << " - " << code;
    //    QString newUrl = url;
    //    newUrl.replace(DownloadUrl, COOKIE_CREATOR_URL);

    download(COOKIE_CREATOR_URL + code);
    //    setDownloading(true);
}

void Downloader::download(const QString &urlString)
{
    QUrl url(urlString);
    QNetworkRequest request(url);

    if (!mCookies.isEmpty()) {
        QVariant var;
        var.setValue(mCookies);
        request.setHeader(QNetworkRequest::CookieHeader, var);
    }

    request.setRawHeader("User-Agent", " Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0");
    // m_nreply = m_netAccess->get(request);
    m_replies.append(m_netAccess->get(request));
    setActiveConnections(m_activeConnections + 1);
    // connect(m_nreply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgressChanged(qint64, qint64)));
}

void Downloader::search(const QString &term)
{
    // qDebug() << Q_FUNC_INFO << " " + term;
    download(SEARCH_URL + term);
}

void Downloader::downloadFinished(QNetworkReply *reply)
{
    // qDebug() << Q_FUNC_INFO << " downloaded " << reply->url().toString();

    setActiveConnections(m_activeConnections - 1);
    if (reply->url().toString().startsWith(COOKIE_CREATOR_URL)) {
        mCookies = m_netAccess->cookieJar()->cookiesForUrl(reply->url());

        download(reply->url().toString().replace(COOKIE_CREATOR_URL, DownloadUrl));
        for (int i = 0; i < m_replies.count(); ++i) {
            if (m_replies.at(i) == reply) {
                m_replies.removeAt(i);
                break;
            }
        }
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        setDownloading(false);
        emit searchEnded();

        qDebug() << Q_FUNC_INFO << " error downloading "
                 << reply->url().toString() << ":" << reply->errorString();

        if (reply->error() != QNetworkReply::OperationCanceledError)
            emit serverError();

        for (int i = 0; i < m_replies.count(); ++i) {
            if (m_replies.at(i) == reply) {
                m_replies.removeAt(i);
                break;
            }
        }
        reply->deleteLater();
        return;
    }


    // if (!reply->url().toString().startsWith("http://www.goear.com/action/sound/get/"))
    // qDebug() << reply->url().toString() << "Downloaded";

    QVariant redir = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (redir.isValid()) {
        QUrl url = redir.toUrl();
        QString name = m_songsToDownload.value(reply->url().toString()).toString();
        m_songsToDownload.remove(reply->url().toString());
        m_songsToDownload.insert(url.toString(), name);

        if (url.toString().endsWith(".mp3")) {
            emit decodedUrl(reply->url().toString().remove(DownloadUrl), url.toString());
            for (int i = 0; i < m_replies.count(); ++i) {
                if (m_replies.at(i) == reply) {
                    m_replies.removeAt(i);
                    break;
                }
            }
            reply->deleteLater();
            return;
        }

        qDebug() << reply->url().toString() << " was redirected to:" << url.toString();

        if (url.isRelative())
            url.setScheme(reply->url().scheme());

        download(url.toString());
        for (int i = 0; i < m_replies.count(); ++i) {
            if (m_replies.at(i) == reply) {
                m_replies.removeAt(i);
                break;
            }
        }
        reply->deleteLater();
        return;
    }

    QString mimeType(reply->header(QNetworkRequest::ContentTypeHeader).toString());

    if (mimeType.contains("text/html")) {
        QString songs(reply->readAll());

        int count = songs.count("listen/") / 3;

        if (count == 0)
            emit noResults();

        QString hasMore = "";
        QString searchTerm = "<ol class=\"pagination group\">";
        QString closingTerm = "</ol>";

        int termBegins = songs.indexOf(searchTerm);
        int termEnds = songs.indexOf(closingTerm, termBegins);

        if (termBegins != -1) {
            hasMore = songs.mid(termBegins + searchTerm.length(), termEnds - termBegins
                                - searchTerm.length());
        }

        QStringList list = hasMore.simplified().split("<li", QString::SkipEmptyParts);

        bool foundMore = false;
        for (int i = 0; i < list.length() - 1; ++i) {
            if (list.at(i).simplified().startsWith("class=\"active\">")) {
                QString number = reply->url().toString().mid(reply->url().toString().lastIndexOf("/") + 1);
                bool isNumber = false;
                int index = number.toInt(&isNumber);
                //                reply->url().toString().mid(reply->url().toString().length() - 1).toInt(isANumber);
                if (isNumber)
                    emit searchHasMoreResults(reply->url().toString().mid(0, reply->url().toString().lastIndexOf("/")) + "/" + QString::number(index + 1));
                else
                    emit searchHasMoreResults(reply->url().toString() + "/" + QString::number(++i));

                foundMore = true;
                break;
            }
        }

        if (!foundMore)
            emit searchHasNoMoreResults();

        searchTerm = "<ol class=\"board_list results_list\">";
        closingTerm = "</ol>";

        termBegins = songs.indexOf(searchTerm);
        termEnds = songs.indexOf(closingTerm, termBegins);

        songs = songs.mid(termBegins + searchTerm.length(),
                          termEnds - termBegins);

        songs = decodeHtml(songs);

        int addedSongs = 0;

        for (int i = 0; i < count; ++i) {
            searchTerm = "listen/";
            closingTerm = "/";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString code = songs.mid(termBegins, termEnds - termBegins);

            searchTerm = "src=\"http://www.goear.com/band/picture/";
            closingTerm = "\"";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString picture = songs.mid(termBegins, termEnds - termBegins);

            songs = songs.mid(termEnds);

            searchTerm = "\">";
            closingTerm = "</a></h4>";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString tmp = songs.mid(termBegins, termEnds - termBegins);
            QString artist = tmp.mid(0, tmp.indexOf(" -"));
            QString title = tmp.mid(tmp.indexOf("- ") + 1);

            if (artist == "")
                artist = tmp;

            if (title == "")
                title = tmp;


            searchTerm = "<li class=\"length\"";
            closingTerm = "</li>";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            tmp = songs.mid(termBegins, termEnds - termBegins);
            QString length = tmp.mid(tmp.indexOf(">") + 1);

            searchTerm = "title=\"Kbps\">";
            closingTerm = "<abbr";

            if (songs.indexOf(searchTerm) == -1)
                searchTerm = "title=\"Kbps\">";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString kbps = songs.mid(termBegins, termEnds - termBegins).remove("<abbr title=\"Kilobit por segundo\">").remove("kbps");

            searchTerm = "title=\"Plays\">";
            closingTerm = "</li>";

            if (songs.indexOf(searchTerm) == -1)
                searchTerm = "class=\"total_hits\" >";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString hits = songs.mid(termBegins, termEnds - termBegins);

            searchTerm = "<li class=\"description\">";
            closingTerm = "</li>";

            termBegins = songs.indexOf(searchTerm) + searchTerm.length();
            termEnds = songs.indexOf(closingTerm, termBegins);

            QString comment = songs.mid(termBegins, termEnds - termBegins);

            searchTerm = ">";
            closingTerm = "</";

            termBegins = comment.indexOf(searchTerm) + searchTerm.length();
            termEnds = comment.indexOf(closingTerm, termBegins);

            comment = comment.mid(termBegins, termEnds - termBegins);

            // qDebug() << "ADDDING SONG:";
            // qDebug() << "TITLE:" << title << "ARTIST:" << artist << "LENGTH:" << length
            //          << "COMMENT:" << comment << "KBPS:" << kbps << "CODE:" << code
            //          << "PICTURE:" << picture;

            songs = songs.mid(songs.indexOf("<li class=\"total_comments hide\""));

            if (kbps.toInt() > 127) {
                emit songFound(title, artist, length, comment, kbps.toInt(), code, picture, hits.replace(",", "").toLong());
                getDownloadLink(code);
                addedSongs++;
            }
        }

        if (addedSongs == 0)
            emit noResults();

    }  else if (mimeType == "audio/mpeg") {
        qDebug() << "Writing file";
        QString name(m_songsToDownload.value(reply->url().toString()).toString());
        m_songsToDownload.remove(reply->url().toString());

        name.replace("/", "-");

#ifdef Q_OS_BLACKBERRY
        name = "shared/music/" + name;
#endif

#ifdef Q_OS_ANDROID
        QDir musicDir("/sdcard/Music");
        if (!musicDir.exists())
            musicDir.mkdir("/sdcard/Music");

        name = "/sdcard/Music/" + name;
#endif

        bool permission;
        QFile file(name + reply->url().toString().mid(reply->url().toString().lastIndexOf(".")));
        permission = file.open(QIODevice::WriteOnly);

        if (!permission) {
            UIValues uiValues;
            uiValues.showMessage("Unable to create file, no permission");
        }

        file.write(reply->readAll());
        file.close();
    } else {
        qDebug() << reply->url().toString() << " type is:" << mimeType;
    }

    if (reply->url().toString().endsWith(".mp3"))
        setDownloading(false);

    if (reply->url().toString().startsWith(SEARCH_URL))
        emit searchEnded();

    for (int i = 0; i < m_replies.count(); ++i) {
        if (m_replies.at(i) == reply) {
            m_replies.removeAt(i);
            break;
        }
    }
    reply->deleteLater();
}

void Downloader::cancelSearch()
{
    for (int i = 0; i < m_replies.count(); i++) {
        if (i < m_replies.count() && m_replies.at(i) && m_replies.at(i)->isRunning())
            m_replies.at(i)->abort();
    }
}

int Downloader::activeConnections() const
{
    return m_activeConnections;
}

void Downloader::setActiveConnections(int activeConnections)
{
    if (m_activeConnections == activeConnections)
        return;

    m_activeConnections = activeConnections;
    emit activeConnectionsChanged();
}

QString Downloader::decodeHtml(const QString &html)
{
    QString decodedHtml(html);

    decodedHtml.replace("&amp;", "&");

    decodedHtml.replace("&aacute;", "á");
    decodedHtml.replace("&eacute;", "é");
    decodedHtml.replace("&iacute;", "í");
    decodedHtml.replace("&oacute;", "ó");
    decodedHtml.replace("&uacute;", "ú");

    decodedHtml.replace("&Aacute;", "Á");
    decodedHtml.replace("&Eacute;", "É");
    decodedHtml.replace("&Iacute;", "Í");
    decodedHtml.replace("&Oacute;", "Ó");
    decodedHtml.replace("&Uacute;", "Ú");

    decodedHtml.replace("&Ntilde;", "Ñ");
    decodedHtml.replace("&ntilde;", "ñ");

    decodedHtml.replace("&Euml;", "Ë");
    decodedHtml.replace("&euml;", "ë");
    decodedHtml.replace("&Uuml;", "Ü");
    decodedHtml.replace("&uuml;", "ü");

    decodedHtml.replace("&Ccedil;", "Ç");
    decodedHtml.replace("&ccedil;", "ç");

    return decodedHtml;
}

void Downloader::downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal == 0 || bytesTotal == -1)
        return;

    float i = bytesReceived / (double)bytesTotal;

    QNetworkReply *net = (QNetworkReply *)sender();
    // qDebug() << "Progress for: "<< net->url().toString() << " " << i;

    emit progressChanged(i, m_songsToDownload.value(net->url().toString()).toString());
}

bool Downloader::isDownloading() const
{
    return m_downloading;
}

void Downloader::setDownloading(bool downloading)
{
    if (m_downloading == downloading)
        return;

    m_downloading = downloading;
    emit downloadingChanged();
}
