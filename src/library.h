#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <qhttpconnection.h>
#include <QFileSystemWatcher>
#include "tvshow.h"
#include "malclient.h"
#include "malapidotcomclient.h"
#include "moebooruclient.h"
#include "gelbooruclient.h"
#include "metadataparser.h"
#include "libraryfilter.h"
#include "searchdirectory.h"
#include "franchise.h"
#include "onlinesync.h"

class BaseConfig;
class DirectoryScannerThread;

class Library : public QObject
{
    Q_OBJECT
public:

    enum searchStatus {
        notStarted,
        inProgress,
        done
    };

    explicit Library(QString path, QObject *parent = 0);
    virtual ~Library();
    void initOnlineSync(const BaseConfig& malConfigFilepath);

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

    TvShow& tvShow(const QString name);
    TvShow* existingTvShow(const QString name);
    const LibraryFilter& filter() const;

    //void xbmcLinkExport(QDir outputDir);
    void write();
    void readAll();

    QDir getDirectory() const;

    void startSearch();
    void startSearch(const QList<SearchDirectory> dirs);
    Library::searchStatus getSearchStatus() const;
    bool getWallpaperDownloadRunning() const;

    const QList<SearchDirectory>& getSearchDirectories() const;
    bool addSearchDirectory(SearchDirectory dir);
    void addWallpaperDownloader(WallpaperDownload::Client* client);
    SearchDirectory* getSearchDirectory(QString path);
    bool removeSearchDirectory(QString path);

    void addToFrenchise(const TvShow *show);

    MetaDataParser *getMetaDataParser() const;
    void setMetaDataParser(MetaDataParser *value);
signals:
    void showAdded(TvShow* show);
    void searchFinished();
    void wallpaperDownloadersFinished();
    void wallpaperDownloaded(QString);
    void beforeWatchCountChanged(int newValue, int oldValue);
    
public slots:
    void importTvShowEpisode(QString episodePath);
    void startWallpaperDownloaders();
    void fetchMetaData();
    void generateFrenchises();

    void fileChangedInSearchDirectory(QString);
    void onVideoPlaybackEndedNormally(TvShow* show);

private slots:
    void fetchingFinished();
    void wallpaperDownloaderFinished();

    void onBodyForRemoveFiles(QHttpResponse* resp, const QByteArray& body);
private:
    QDir directory;
    QList<TvShow*> tvShows;
    QList<Franchise*> franchises;
    QList<SearchDirectory> searchDirectories;

public:
// put this public, because the server needs, it.
// TODO better remove this from library and move it into main and pass it here as reference
OnlineSync onlineSync;
private:
    LibraryFilter mFilter;
    MetaDataParser* metaDataParser;

    QList<WallpaperDownload::Client*> wallpaperDownloaders;
    QList<WallpaperDownload::FetchThread*> runningWallpaperDownloaders; // TODO only access the clients
    DirectoryScannerThread* searchThread;
    QFileSystemWatcher* fileSystemWatcher;
};

#endif // LIBRARY_H
