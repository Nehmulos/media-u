#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <QList>
#include "tvshow.h"
#include <qhttpconnection.h>
#include <utility>

class LibraryFilter
{
public:
    LibraryFilter(QList<TvShow*>& shows, QDir libraryDir);

    QList<TvShow*> all();
    QList<TvShow*> airing();
    QList<TvShow*> withWallpaper();
    QList<TvShow*> recentlyWatched();

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);
    MovieFile* getEpisodeForPath(const QString& path);
    TvShow* getShowForRemoteId(int remoteId);
    TvShow* getRandomShow();
    QString getRandomWallpaper();

    QDir getLibraryDir() const;

    QList<TvShow *> statusCompleted();
    QList<TvShow *> statusWatching();
    QList<TvShow *> statusWaitingForNewEpisodes();
    QList<TvShow *> statusOnHold();
    QList<TvShow *> statusDropped();
    QList<TvShow *> statusPlanToWatch();
    void sendLists(QHttpResponse *resp, QList<std::pair<QString, QList<TvShow *> > > lists);
private:
    QList<TvShow*> filter(bool (*filterFunc)(const TvShow &, const LibraryFilter &, const void *), const void* userData = NULL);

    static bool filterAll(const TvShow&, const LibraryFilter&, const void *userData);
    static bool filterAiring(const TvShow & show, const LibraryFilter&, const void* userData);
    static bool filterHasWallpaper(const TvShow& show, const LibraryFilter& filter, const void* userData);
    static bool filterRecentlyWatched(const TvShow& show, const LibraryFilter&, const void *userData);
    static bool filterStatus(const TvShow& show, const LibraryFilter&, const void* userData);

    TvShow* getRandomShow(const QList<TvShow *> &shows);

    QList<TvShow*>& tvShows;
    QDir libraryDir;
};

#endif // LIBRARYFILTER_H
