#include "snapshotwallpapergenerator.h"
namespace SnapshotWallpaperGenerator {

Client::Client(MetaDataParser& mp, ThumbnailCreator& tc) :
    WallpaperDownload::Client(QString()),
    metaDataParser(mp),
    thumbnailCreator(tc)
{
    outstandingWallpapers = 0;
}

SearchResult Client::fetchPostsBlocking(const TvShow* show, int page) {
    SearchResult result;
    if (page != 1) {
        return result;
    }

    const QList<Episode*>& eps = show->episodeList().episodes;
    foreach (const Episode* ep, eps) {
        const MovieFile* mf = ep->bestFile(show->getReleaseGroupPreference());
        if (!mf) {
            continue;
        }
        MetaData data = metaDataParser.parse(mf->path);
        if (-1 == data.duration) {
            continue;
        }
        for (float i=1; i < 3.f; ++i) {
            // paranoid check for stupid compilers
            int second = std::min(data.duration-1, (int)(data.duration * (0.3 * i)));
            Entry e;
            e.id = mf->path;
            e.fileUrl = QString::number(second);
            e.score = second;
            result.entries.push_back(e);
        }
    }

    return result;
}

void Client::downloadResults(QDir directory, const QList<Entry>& entries, bool onlyTheBest) {
    int generated = 0;
    foreach (const Entry& entry, entries) {
        // TODO check if this is a good wallpaper source
        //if (onlyTheBest && !entry.isGoodWallpaper()) {
        //    continue;
        //}
        if (generated >= limit) {
            break;
        }
        ++generated;
        ++outstandingWallpapers;
        int s = QString(entry.fileUrl).toInt();
        ThumbnailCreationData* data = new ThumbnailCreationData();
        data->dir = directory;
        data->filename = QString("%1 %2.jpg").arg(QFileInfo(entry.id).fileName(), QString::number(s));
        data->client = this;
        ThumbCreationCallback* tcc = thumbnailCreator.generateJpeg(entry.id, s, -1, -1, data);
        connect(tcc, SIGNAL(jpegGenerated(QByteArray)), this, SLOT(wallpaperReady(QByteArray)));
        tcc->start();
    }

    //while (outstandingWallpapers) {
    //    QThread::msleep(10);
    //}
}

void Client::generated(QString file) {
    emit wallpaperDownloaded(file);
}

void Client::wallpaperReady(QByteArray data) {
    --outstandingWallpapers;
    ThumbCreationCallback* self = dynamic_cast<ThumbCreationCallback*>(sender());
    if (!self) {
        return;
    }
    ThumbnailCreationData* userData = static_cast<ThumbnailCreationData*>(self->data);
    QString path = userData->dir.absoluteFilePath(userData->filename);
    userData->dir.mkpath(".");
    QFile file(path);
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();
    userData->client->generated(path);
    delete userData;
}

} // namespace
