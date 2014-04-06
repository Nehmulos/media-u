#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QProcess>
#include <qhttpconnection.h>
#include <QObject>
#include "metadataparser.h"
#include "thumbnailcreator.h"
#include "library.h"
#include "config.h"
#include "shortclipcreator.h"

class VideoProgress {
public:
    float seconds;
    MetaData metaData;
    TvShow* tvShow;
    QString path;

    QString toSecondsAndPathJson();
    QString toJson();
};

class VideoPlayer : public QObject
{
   Q_OBJECT // TODO build breaks here when trying to inherit qobject
public:
    explicit VideoPlayer(Library &library, const SnapshotConfig& snapshotConfig, QObject *parent = NULL);
    virtual ~VideoPlayer();
    bool playFile(QString filepath);

    void togglePause();
    void pause();
    void unPause();

    virtual void stop() = 0;

    virtual void jumpTo(int second);
    virtual void backwards(const int seconds = 5) = 0;
    virtual void forwards(const int seconds = 5) = 0;

    virtual float incrementVolume() = 0; ///< returns the new sound level
    virtual float decrementVolume() = 0; ///< returns the new sound level

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

    const MetaDataParser *getMetaDataParser() const;
    void setMetaDataParser(const MetaDataParser *value);

    const ThumbnailCreator *getThumbnailCreator() const;
    void setThumbnailCreator(const ThumbnailCreator *value);

    QStringList getPlaylist() const;
    void setPlaylist(const QStringList &value);

    void resetPlayingStatus();
    VideoProgress getNowPlaying() const;

    void createGif(QHttpResponse* resp, float startSecond, float endSecond);
signals:
    void paused();
    void unpaused();
    void jumped(int second);

    void playbackStarted();
    void playbackEnded();
    void playbackEndedNormally();
    void playbackCanceled();

    void exactProgressReady(float);

protected:
    virtual bool playFileImpl(QString filepath, const TvShowPlayerSettings& settings) = 0;
    virtual void pauseImpl() = 0;
    virtual void unPauseImpl() = 0;
    virtual bool customHandleApiRequest() { return false; }
    virtual void getExactProgress() { emit exactProgressReady(nowPlaying.seconds); }

    Library& library;
    const SnapshotConfig& snapshotConfig;
    const MetaDataParser* metaDataParser;
    const ThumbnailCreator* thumbnailCreator;
    QProcess process;
    bool pauseStatus;
    VideoProgress nowPlaying;

    QStringList playlist;

    // snapshots
    QMap<QString, QString> unhandledSnapshots;

    QString imageName(QString templateString, QString extension) const;
    QString snapshotOutputPath() const;
    QString shortClipOutputPath(float start, float end) const;
    void convertSnapshots();
    bool convertSnapshot(const QString snapshotPath, const QString outputPath);

private:
    const ShortClipCreator::Config* initShortClipConfig(float startSecond, float endSecond) const;

public slots:
    void onThumbnailCreated(const QByteArray img);
    void receivedPlaylist(QHttpResponse* resp, const QByteArray& body);
    void onGifReady(bool success);
    void onExactProgressReady(float);

private slots:
    void onPlaybackEndedNormally();
    void onPlaybackCanceled();
};

#endif // VIDEOPLAYER_H
