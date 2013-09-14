#include "videoplayer.h"
#include <server.h>

VideoPlayer::VideoPlayer(Library& library, QObject* parent) : QObject(parent), library(library)
{
    paused = false;
    connect(this, SIGNAL(playbackEndedNormally()), this, SLOT(onPlaybackEndedNormally()));
    connect(this, SIGNAL(playbackCanceled()), this, SLOT(onPlaybackCanceled()));
}

VideoPlayer::~VideoPlayer() {
    this->process.kill();
}

void VideoPlayer::togglePause() {
    if (paused) {
        unPause();
    } else {
        pause();
    }
}

void VideoPlayer::jumpTo(int second) {
    int difference = second - this->progress;
    if (difference > 0) {
        forwards(difference);
    } else if (difference < 0) {
        backwards(-difference);
    }
}

bool VideoPlayer::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/player/play") && !req->url().query().isEmpty()) {
        std::stringstream ss;
        ss << req->url().query(QUrl::FullyDecoded).toStdString();
        QString episode;
        nw::JsonReader jr(ss);
        NwUtils::describe(jr, "filename", episode);
        jr.close();

        int error = this->playFile(episode);
        if (this->process.state() == QProcess::Running) {
            Server::simpleWrite(resp, 200, QString("{\"status\":\"playback started\"}"));
        } else {
            Server::simpleWrite(resp, 500, QString("{\"status\":\"could not start playback\", \"error\":%1}").arg(error));
        }
    } else if (req->path() == "/api/player/setPlaylist") {
        std::stringstream ss;
        // TODO switch to a http-server that can parse request bodies
        //ss << req->body().data();
        //qDebug() << req->body();
        ss << req->url().query(QUrl::FullyDecoded).toStdString();
        nw::JsonReader jr(ss);
        jr.describeValueArray("episodes", -1);
        QStringList episodes;
        for (int i=0; jr.enterNextElement(i); ++i) {
            std::string ep;
            jr.describeValue(ep);
            episodes.append(ep.c_str());
        }
        jr.close();

        this->setPlaylist(episodes);

        if (!episodes.isEmpty()) {
            QString firstEpisode = this->playlist.takeFirst();
            int error = this->playFile(firstEpisode);
            if (this->process.state() == QProcess::Running) {
                Server::simpleWrite(resp, 200, QString("{\"status\":\"playback started\"}"));
            } else {
                Server::simpleWrite(resp, 500, QString("{\"status\":\"could not start playback\", \"error\":%1}").arg(error));
            }
        } else {
            Server::simpleWrite(resp, 500, QString("{\"status\":\"playlist is empty\", \"error\":-1}"));
        }
    } else if (req->path() == "/api/player/stop") {
        this->stop();
        Server::simpleWrite(resp, 200, "{\"status\":\"stopped\"}");
    } else if (req->path() == "/api/player/togglePause") {
        this->togglePause();
        QString status = paused ? "paused" : "unPaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status));
    } else if (req->path() == "/api/player/unPause") {
        this->unPause();
        Server::simpleWrite(resp, 200, "{\"status\":\"unPaused\"}");
    } else if (req->path() == "/api/player/pause") {
        this->pause();
        Server::simpleWrite(resp, 200, "{\"status\":\"paused\"}");
    } else if (req->path().startsWith("/api/player/jumpTo")) {
        bool ok;
        int second = req->url().query().toInt(&ok);
        if (ok) {
            this->jumpTo(second);
            Server::simpleWrite(resp, 200, "{\"status\":\"done\"}");
        } else {
            QByteArray errorData;
            Server::simpleWriteBytes(resp, 404, errorData);
        }
    } else if (req->path() == "/api/player/backwards") {
        this->backwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}");
    } else if (req->path() == "/api/player/forwards") {
        this->forwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}");
    } else if (req->path() == "/api/player/incrementVolume") {
        float volume = this->incrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume));
    } else if (req->path() == "/api/player/decrementVolume") {
        float volume = this->decrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume));
    } else if (req->path() == "/api/player/pauseStatus") {
        QString status = paused ? "paused" : "unPaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status));
    } else if (req->path() == "/api/player/metaData") {
        MetaData m = this->metaDataParser->parse(this->playingFile);
        Server::simpleWrite(resp, 200, m.toJson());
    } else if (req->path().startsWith("/api/player/thumbnail")) {
        bool ok;
        int second = req->url().query().toInt(&ok);
        if (ok) {
            ThumbCreationCallback* tcc = thumbnailCreator->generateJpeg(this->playingFile, second, 100, 70, resp);
            connect(tcc, SIGNAL(jpegGenerated(QByteArray)), this, SLOT(onThumbnailCreated(QByteArray)));
        } else {
            QByteArray errorData;
            Server::simpleWriteBytes(resp, 404, errorData);
        }
    } else if (req->path() == "/api/player/progress") {
        Server::simpleWrite(resp, 200, QString("{\"status\":\"unknown\",\"progress\":%1}").arg(progress));
    } else {
        return customHandleApiRequest();
    }
    return true;
}

const MetaDataParser *VideoPlayer::getMetaDataParser() const
{
    return metaDataParser;
}

void VideoPlayer::setMetaDataParser(const MetaDataParser *value)
{
    metaDataParser = value;
}

const ThumbnailCreator *VideoPlayer::getThumbnailCreator() const
{
    return thumbnailCreator;
}

void VideoPlayer::setThumbnailCreator(const ThumbnailCreator *value)
{
    thumbnailCreator = value;
}

void VideoPlayer::onThumbnailCreated(const QByteArray img) {
    ThumbCreationCallback* tcc = dynamic_cast<ThumbCreationCallback*>(this->sender());
    QHttpResponse* resp = static_cast<QHttpResponse*>(tcc->data);
    Server::simpleWriteBytes(resp, 200, img);
    tcc->deleteLater();
}

void VideoPlayer::onPlaybackEndedNormally() {

    MovieFile* episode = this->library.filter().getEpisodeForPath(playingFile);
    if (episode) {
        episode->setWatched(true);
    }

    this->resetPlayingStatus();
    if (this->playlist.length() > 0) {
        this->playFile(this->playlist.first());
        this->playlist.removeFirst();
    }
}

void VideoPlayer::onPlaybackCanceled() {
    this->playlist.clear();
    this->resetPlayingStatus();
}

void VideoPlayer::resetPlayingStatus() {
    this->progress = -1;
    this->playingFile = QString();
    this->paused = true;
}

QStringList VideoPlayer::getPlaylist() const {
    return playlist;
}

void VideoPlayer::setPlaylist(const QStringList &value) {
    playlist = value;
}