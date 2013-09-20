#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <N0Slib.h>
#include <QDir>
#include "moviefile.h"

class Season : public QObject
{
    Q_OBJECT
public:
    Season(QString name, QObject *parent = NULL);
    virtual ~Season();

    void exportXbmcLinks(QDir dir);
    void writeDetailed(nw::JsonWriter &jw);
    void writeAsElement(nw::JsonWriter& jw);
    void readAsElement(nw::JsonReader &jr);

    void addEpisode(MovieFile *file); ///< takes ownage
    void addEpisode(const MovieFile& file);
    void addEpisode(QString file);
    QString name() const;
    int numberOfEpisodes() const;
    int numberOfWatchedEpisodes() const;
    int highestWatchedEpisodeNumber() const;

    MovieFile* getEpisodeForPath(const QString &path);
    QString favouriteReleaseGroup();
    int highestDownloadedEpisodeNumber() const;
signals:
    void watchCountChanged(int oldCount, int newCount);

private slots:
    void watchedChanged(bool oldValue, bool newValue);

private:
    QList<MovieFile*> episodes;
    QString mName;
};

#endif // SEASON_H
