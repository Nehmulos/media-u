#include "season.h"
#include <QDebug>
#include "nwutils.h"

Season::Season(QString name) {
    mName = name;
}

Season::~Season()
{
    foreach (MovieFile* file, episodes) {
        if (file) {
            delete file;
        }
    }
}

void Season::readAsElement(nw::JsonReader &jr) {
    NwUtils::describe(jr, "name", mName);
    jr.describeValueArray("episodes", episodes.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        std::string episodePath;
        jr.describeValue(episodePath);
        addEpisode(QString(episodePath.data()));
    }
}

void Season::writeAsElement(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "name", mName);
    jw.describeValueArray("episodes", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        const MovieFile* episode = episodes.at(i);
        std::string episodePath = episode->path().toStdString();
        jw.describeValue(episodePath);
    }
}

void Season::addEpisode(const MovieFile& file) {
    addEpisode(file.path());
}

void Season::addEpisode(QString file) {
    for (int i=0; i < episodes.length(); ++i) {
        const MovieFile* f = episodes.at(i);
        if (f->path() == file) {
            return;
        }
    }

    episodes.append(new MovieFile(file));
    qDebug() << episodes.back()->releaseGroup() << episodes.back()->episodeNumber() << episodes.back()->showName();
}

QString Season::name() const {
    return mName;
}

int Season::numberOfEpisodes() const
{
    return episodes.length();
}
