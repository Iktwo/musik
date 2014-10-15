#ifndef SONG_H
#define SONG_H

#include <QObject>

class Song : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString length READ length NOTIFY lengthChanged)
    Q_PROPERTY(QString comment READ comment NOTIFY commentChanged)
    Q_PROPERTY(QString code READ code NOTIFY codeChanged)
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)
    Q_PROPERTY(int kbps READ kbps NOTIFY kbpsChanged)
    Q_PROPERTY(QString picture READ picture NOTIFY pictureChanged)
    Q_PROPERTY(long long hits READ hits WRITE setHits NOTIFY hitsChanged)

public:
    Song(QObject *parent = 0);

    Song(const QString &name, const QString &artist, const QString &length, const QString &comment,
         int kbps, const QString &code, const QString &picture, long long hits, QObject *parent = 0);

    QString name() const;
    QString artist() const;
    QString length() const;
    QString comment() const;
    int kbps() const;
    QString code() const;
    QString url() const;

    void setName(const QString &name);
    void setArtist(const QString &artist);
    void setLength(const QString &length);
    void setComment(const QString &comment);
    void setCode(const QString &code);
    void setUrl(const QString &url);
    void setKbps(int kbps);

    QString picture() const;
    void setPicture(const QString &picture);

    long long hits() const;
    void setHits(long long hits);

signals:
    void nameChanged();
    void artistChanged();
    void lengthChanged();
    void commentChanged();
    void codeChanged();
    void urlChanged();
    void kbpsChanged();
    void pictureChanged();
    void hitsChanged();

private:
    QString m_name;
    QString m_artist;
    QString m_length;
    QString m_comment;
    int m_kbps;
    QString m_url;
    QString m_code;
    QString m_picture;
    long long m_hits;
};

#endif // SONG_H
