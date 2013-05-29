#include "moviefile.h"
#include <QFileInfo>
#include <iostream>
#include <stdlib.h>
#include <QDebug>

MovieFile::MovieFile(QString path, QObject *parent) :
    QObject(parent)
{
    setPath(path);
    watched = false;
}

QString MovieFile::path() const {
    return mPath;
}

void MovieFile::setPath(QString path) {
    mPath = path;
    path = QFileInfo(path).fileName();

    if (!path.contains(' ')) {
        path.replace('_', ' ');
        path.replace('.', ' ');
    }

    int groupIndex = -2;
    while (groupIndex != -1) {
        QRegExp regexGroup("^(\\[.*\\])");
        regexGroup.setMinimal(true);
        groupIndex = regexGroup.indexIn(path);
        if (groupIndex != -1) {
            mReleaseGroup.append(regexGroup.cap(1));
            path.remove(0, regexGroup.cap(1).length());
        }
    }

    QRegExp regexHashId("(\\[[A-F0-9]{8}\\])");
    int hashIdIndex = regexHashId.indexIn(path);
    if (hashIdIndex != -1) {
        mHashId = regexHashId.cap(1);
        path.remove(hashIdIndex, regexHashId.cap(1).length());
    }


    // TODO differ techtags in [] from release groups
    // hints: at the end, multiple [] like [720p][AAC]
    // comma separated [720p, AAC]
    // space separated [720p AAC]

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    if (groupIndex >= -1 && groupIndex <= 1) {
        QRegExp regexName("(.*)(( -)|\\[|\\(|(OP[0-9])|(ED[0-9])|(EP[0-9])|(SP[0-9])|(Episode\\s?[0-9])|$)");
        regexName.setMinimal(true);
        int nameIndex = regexName.indexIn(path);
        mShowName = regexName.cap(1).trimmed();

        if (nameIndex != -1 || mShowName.length() <= 0) {
            path.remove(nameIndex, regexName.cap(1).length());
        } else {
            qDebug() << "could not parse name out of " << path;
        }

    // showname[Group] - 01v2 (techtags)[12345ABC].webm
    } else if (groupIndex > 0) {
        QRegExp regexName("^(.+)\\[");

    // [Group] showname - 01v2 (techtags)[12345ABC].webm    
    } else {
        QRegExp regexName("^(.+)\\[");
    }

    QRegExp regexEpisode("("
        "\\s[0-9]+((v|\\.)[0-9]+)?\\s|"
        "ED[0-9]+|"
        "OP[0-9]+[a-z]?|"
        "SP[0-9]+|"
        "EP[0-9]+|"
        "NC.?OP([0-9]+)?|"
        "NC.?ED([0-9]+)?|"
        "EX([0-9]+)?|"
        "Episode\\s?[0-9]+|"
        "Opening(\\s?[0-9]+)?|"
        "Ending(\\s?[0-9]+)?"
        ")", Qt::CaseInsensitive);
    //regexEpisode.setMinimal(true);
    int episodeIndex = regexEpisode.indexIn(path);
    mEpisodeNumber = regexEpisode.cap(1).trimmed();

    path.remove(episodeIndex, regexEpisode.cap(1).length());

    QRegExp regexSeason("((SE?)[0-9]+|(Season\\s?)[0-9]+)", Qt::CaseInsensitive);
    regexSeason.setMinimal(true);
    int seasonIndex = regexSeason.indexIn(path);
    if (seasonIndex != -1) {
        mSeasonName = regexSeason.cap(1);
        path.remove(seasonIndex, regexSeason.cap(1).length());
    }

    // TODO check for shows with a [0-9]+ ending and just 1 episode to avoid some false positives
    if (mEpisodeNumber.isEmpty() || mEpisodeNumber.isNull()) {
        QRegExp regexNumberAfterShowName("(\\s[0-9]+\\s?)$");
        int epIndex = regexNumberAfterShowName.indexIn(mShowName);
        if (epIndex != -1) {
            mEpisodeNumber = regexNumberAfterShowName.cap(1).trimmed();
            mShowName.remove(epIndex, regexNumberAfterShowName.cap(1).length());
        }
    }
}

void MovieFile::writeAsElement(nw::JsonWriter &jw) const
{
    std::string path = mPath.toStdString(),
                epNum = mEpisodeNumber.toStdString();
    jw.describe("path", path);
    //jw.describe("number", epNum);
    //jw.describe();
}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv$|\\.ogv$|\\.mpeg$|\\.mp4$|\\.webm$|\\.avi$", Qt::CaseInsensitive));
}

QString MovieFile::releaseGroup() const {
    return mReleaseGroup;
}

QString MovieFile::episodeName() const {
    return mEpisodeName;
}

QString MovieFile::showName() const {
    return mShowName;
}

QString MovieFile::seasonName() const {
    return mSeasonName;
}

QString MovieFile::episodeNumber() const {
    return mEpisodeNumber;
}

QStringList MovieFile::techTags() const {
    return mTechTags;
}
QString MovieFile::hashId() const {
    return mHashId;
}
