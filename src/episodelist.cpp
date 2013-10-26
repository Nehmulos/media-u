#include "episodelist.h"
#include <QDebug>
#include "nwutils.h"

EpisodeList::EpisodeList(QString name, QObject* parent) : QObject(parent) {
    mName = name;
}

EpisodeList::~EpisodeList() {
}

void EpisodeList::exportXbmcLinks(QDir dir) {
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (int i=0; i < episodes.length(); ++i) {
        const MovieFile* f = episodes.at(i);
        QString linkName = QString("%1. %2.%3").arg(f->xbmcEpisodeNumber(), f->xbmcEpisodeName(), f->fileExtension());
        QFile::link(f->path(), dir.absoluteFilePath(linkName));
    }
}

void EpisodeList::readAsElement(nw::JsonReader &jr) {
    NwUtils::describe(jr, "name", mName);
    jr.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        MovieFile* episode = new MovieFile(&jr, this);
        addEpisode(episode);
    }
}

void EpisodeList::writeAsElement(nw::JsonWriter &jw) {
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MovieFile* episode = episodes[i];
        episode->describe(&jw);
    }
}

void EpisodeList::writeDetailed(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "name", mName);
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MovieFile* episode = episodes[i];
        episode->writeDetailed(jw);
    }
}

void EpisodeList::addEpisode(MovieFile* episode) {
    if (episode->path().isEmpty() || NULL != getEpisodeForPath(episode->path())) {
        delete episode;
    } else {
        episodes.append(episode);
        episode->setParent(this);
        connect(episode, SIGNAL(watchedChanged(bool,bool)), this, SLOT(watchedChanged(bool,bool)));
    }
}

void EpisodeList::addEpisode(QString file) {
    if (file.isEmpty() || NULL != getEpisodeForPath(file)) {
        return;
    }

    addEpisode(new MovieFile(file, this));
    qDebug() << episodes.back()->releaseGroup() << episodes.back()->episodeNumber() << episodes.back()->showName();
}

QString EpisodeList::name() const {
    return mName;
}

int EpisodeList::numberOfEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial()) {
            ++count;
        }
    }
    return count;
}

int EpisodeList::numberOfWatchedEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial() && episodes.at(i)->getWatched()) {
            ++count;
        }
    }
    return count;
}


int EpisodeList::highestDownloadedEpisodeNumber() const
{
    int highest = -1;
    for (int i=0; i < episodes.length(); ++i) {
        int num = episodes.at(i)->numericEpisodeNumber();
        highest = num > highest ? num : highest;
    }
    return highest;
}

int EpisodeList::highestWatchedEpisodeNumber() const
{
    int highest = -1;
    for (int i=0; i < episodes.length(); ++i) {
        if (episodes.at(i)->getWatched()) {
            int num = episodes.at(i)->numericEpisodeNumber();
            highest = num > highest ? num : highest;
        }
    }
    return highest;
}

QString EpisodeList::mostDownloadedReleaseGroup() const {
    QMap<QString, int> groups;
    for (int i=0; i < episodes.length(); ++i) {
        QString group = episodes.at(i)->releaseGroup();
        groups[group]++;
    }
    std::pair<QString, int> highest;
    for (QMap<QString, int>::iterator it=groups.begin(); it != groups.end(); ++it) {
        int num = it.value();
        if (num > highest.second) {
            highest.second = num;
            highest.first = it.key();
        }
    }
    return highest.first;
}

bool epNumLess(const MovieFile* a, const MovieFile* b) {
    return a->numericEpisodeNumber() < b->numericEpisodeNumber();
}

QString EpisodeList::favouriteReleaseGroup() const {
    QList<MovieFile* > episodesByNumber = episodes;
    qSort(episodesByNumber.begin(), episodesByNumber.end(), epNumLess);

    if (episodesByNumber.at(0)->releaseGroup().compare(
            episodesByNumber.at(1)->releaseGroup(),
            Qt::CaseInsensitive
        )
    ) {
        return episodesByNumber.at(0)->releaseGroup();
    }
    return mostDownloadedReleaseGroup();
}

MovieFile* EpisodeList::getEpisodeForPath(const QString& path) {
    for (int i=0; i < episodes.length(); ++i) {
        MovieFile* f = episodes[i];
        if (f->path() == path) {
            return f;
        }
    }
    return NULL;
}

void EpisodeList::watchedChanged(bool oldValue, bool newValue) {
    int count = numberOfWatchedEpisodes();
    if (!oldValue && newValue) {
        emit watchCountChanged(count-1, count);
    } else if (oldValue && !newValue) {
        emit watchCountChanged(count+1, count);
    } else {
        emit watchCountChanged(count, count);
    }
}
