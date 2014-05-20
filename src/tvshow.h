#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <map>
#include <QDate>
#include <QDir>
#include "episodelist.h"
#include <N0Slib.h>
#include <qhttpconnection.h>

//TODO fix Franchises by asking for service-identifier + remoteId
//continue writing code in this file, I guess

class TvShowPlayerSettings {
public:
    TvShowPlayerSettings();
    int subtileTrack;
    int audioTrack;
    int videoTrack;
    void describe(nw::Describer *de);
};

class TvShow;
class Library;
class RelatedTvShow {
public:
    RelatedTvShow(const QString identifier, int id);
    TvShow* get(Library &library) const;

    bool matches(const TvShow& show) const;
    bool isDummy() const;
    bool operator ==(const RelatedTvShow& other) const;
    void describe(nw::Describer* de);
    void parseForManga(nw::Describer* de);
    void parseForAnime(nw::Describer* de);
    static RelatedTvShow makeDummy();
    static void parseFromList(nw::Describer* de, QString arrayName, QList<RelatedTvShow> &list, const bool anime);

private:
    std::set<const QString, int> remoteIds;
    QString title;
};

class TvShow : public QObject
{
    Q_OBJECT
public:

    enum WatchStatus {
        automatic,
        completed,
        watching,
        waitingForNewEpisodes,
        onHold,
        dropped,
        planToWatch
    };

    class OnlineSyncData {
    public:
        OnlineSyncData();
        void describe(nw::Describer& de);
        int getRemoteId() const;
        void setRemoteId(int value);
        QDateTime getLastOnlineTrackerUpdate() const;
        void setLastOnlineTrackerUpdate(const QDateTime& value);
    private:
        int remoteId;
        QDateTime lastOnlineTrackerUpdate;
    };

    TvShow(QString name, QObject* parent = NULL);

    EpisodeList &episodeList();

    void read(QDir &dir);
    void write(nw::JsonWriter &jw);

    void importVideoFile(const VideoFile *episode);
    void downloadImage(const QString url, QDir libraryDirectory);

    bool isAiring() const;
    QString coverPath(QDir libaryPath) const;

    void handleApiRequest(int urlPrefixOffset, QHttpRequest *req, QHttpResponse *resp);

    // TODO reimplement export for episode list
    // or remove, since it didn't work very good to begin with
    //void exportXbmcLinks(QDir dir);

    QString name() const;
    QStringList getSynonyms() const;
    QString getAiringStatus() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    int getTotalEpisodes() const;
    QString getSynopsis() const;
    QString getShowType() const;
    int getRemoteId(const QString trackerIdentifierKey) const;
    bool matchesNameOrSynonym(QString str) const;
    bool matchesRemote(const QString trackerIdentifierKey, int id) const;

    void addPrequels(QList<RelatedTvShow> relations);
    void addSideStories(QList<RelatedTvShow> relations);
    void addSequels(QList<RelatedTvShow> relations);
    void syncRelations(Library &library);
    bool hasRelationTo(const TvShow *show) const;

    void addSynonyms(const QStringList& values);
    void setSynonyms(const QStringList &value);
    void setAiringStatus(const QString &value);
    void setStartDate(const QDate &value);
    void setEndDate(const QDate &value);
    void setTotalEpisodes(int value);
    void setShowType(const QString &value);
    void setSynopsis(const QString &value);
    void setRemoteId(const QString remoteIdentifier, const int &value);

    QDir directory(QDir libraryDirectory) const;
    QDir wallpaperDirectory(QDir libraryDirectory) const;
    int numberOfWallpapers(QDir libraryDirectory) const;
    QString randomWallpaper(QDir libraryDirectory) const;
    QStringList wallpapers(QDir libraryDirectory) const;
    QString favouriteReleaseGroup() const;

    Episode* getEpisodeForPath(const QString &path);

    TvShowPlayerSettings playerSettings;
    QDateTime lastWatchedDate() const;

    WatchStatus getStatus() const;
    void setStatus(TvShow::WatchStatus status);
    bool isCompleted() const;
    const EpisodeList& episodeList() const;
    static QString watchStatusToString(TvShow::WatchStatus status);
    static TvShow::WatchStatus watchStatusFromString(QString statusString);

    void writeAsListingItem(nw::Describer *de) const;
    QStringList getReleaseGroupPreference() const;
    void setReleaseGroupPreference(QStringList value);
    int getRewatchMarker() const;
    void setRewatchMarker(int marker, bool updateTracker);
    int getRewatchCount() const;
    void setRewatchCount(int count, bool updateTracker);
    QDateTime getLastLocalUpdate() const;
    QDateTime getLastOnlineTrackerUpdate(const QString trackerKey) const;
    void setLastOnlineTrackerUpdate(const QString trackerKey, const QDateTime& value);

signals:

private slots:
    void receivedPlayerSettings(QHttpResponse* resp, const QByteArray& body);
    void receivedReleaseGroupPreference(QHttpResponse* resp, const QByteArray& body);
    void onWatchCountChanged(int, int);

private:

    QString mName;
    EpisodeList episodes;
    WatchStatus customStatus;

    QList<RelatedTvShow> prequels;
    QList<RelatedTvShow> sideStories;
    QList<RelatedTvShow> sequels;
    QStringList releaseGroupPreference;
    int rewatchMarker;
    int rewatchCount;

    std::map<const QString, OnlineSyncData> onlineSyncData;
    QDateTime lastLocalUpdate;
    QStringList synonyms;
    QString showType;
    QString airingStatus;
    QDate startDate;
    QDate endDate;
    int totalEpisodes;
    QString synopsis;
};

#endif // TVSHOW_H
