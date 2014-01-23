#include "moviefile.h"
#include <QFileInfo>
#include <QDebug>

MovieFile::MovieFile(const QString originalPath) {
    // set the path and resolve links
    QString path = originalPath;
    this->path = QFileInfo(path).canonicalFilePath();
    // if the resolved filepath does not exist just take the given path
    // and assume the drive will be mounted later
    if (this->path.isEmpty()) {
        this->path = path;
    }

    // start processing
    path = QFileInfo(this->path).completeBaseName();


    int spaces = path.count(' ');
    if (path.count('_') > spaces) {
        path.replace('_', ' ');
    }

    spaces = path.count(' ');
    if (path.count('.') > spaces) {
        path.replace('.', ' ');
    }

    int groupIndex = -2;
    while (groupIndex != -1) {
        QRegExp regexGroup("^(\\[.*\\])");
        regexGroup.setMinimal(true);
        groupIndex = regexGroup.indexIn(path);
        if (groupIndex != -1) {
            this->releaseGroup.append(regexGroup.cap(1));
            path.remove(0, regexGroup.cap(1).length());
        }
    }

    QRegExp regexHashId("(\\[[A-F0-9]{8}\\])");
    int hashIdIndex = regexHashId.indexIn(path);
    if (hashIdIndex != -1) {
        this->hashId = regexHashId.cap(1);
        path.remove(hashIdIndex, regexHashId.cap(1).length());
    }


    QRegExp techTags("((\\[.*\\])|(\\(.*\\)))");
    techTags.setMinimal(true);
    int techTagsIndex = techTags.indexIn(path);
    while (techTagsIndex != -1) {
        path.remove(techTagsIndex, techTags.cap(1).length());
        techTagsIndex = techTags.indexIn(path);
    }

    // TODO differ techtags in [] from release groups
    // hints: at the end, multiple [] like [720p][AAC]
    // comma separated [720p, AAC]
    // space separated [720p AAC]

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    // figure out when the name stops and something else (ep name / techtags) begins
    QRegExp regexName("(.*)"
                      "("
                      "( -)|"
                      "(-\\s?[0-9])|"
                      "\\[|"
                      "\\(|"
                      "(\\sOP[0-9])|"
                      "(\\sED[0-9])|"
                      "(\\sEP[0-9])|"
                      "(\\sSP[0-9])|"
                      "(\\sEpisode\\s?[0-9])|"
                      "(\\sPlay All)|"
                      "$"
                      ")");
    regexName.setMinimal(true);
    int nameIndex = regexName.indexIn(path);
    this->showName = regexName.cap(1).trimmed();

    qDebug() << showName;

    if (nameIndex != -1 || this->showName.length() <= 0) {
        path.remove(nameIndex, regexName.cap(1).length());
    } else {
        qDebug() << "could not parse name out of " << path;
    }

    QRegExp regexEpisode("("
        // number only with separator
        "-[0-9]|"
        "\\s[0-9]+((v|\\.)[0-9]+)?(\\s|$)|"
        "\\s[0-9]+(\\[v[0-9]+\\])(\\s|$)|"
        "\\s[0-9]+x[0-9]+|"
        "\\sEP\\s?[0-9]+|"
        "\\sEpisode\\s?[0-9]+|"

         "\\sED(\\s|$)|"
         "\\sOP(\\s|$)|"

         "\\sED\\s?[0-9]+|"
         "\\sOP\\s?[0-9]+[a-z]?|"
         "\\sSP\\s?[0-9]+|"
         "\\sNC.?OP\\s?([0-9]+)?|"
         "\\sNC.?ED\\s?([0-9]+)?|"
         "\\sEX\\s?([0-9]+)?|"
         "\\sPV\\s?([0-9]+)?|"

         "\\sOpening(\\s?[0-9]+)?|"
         "\\sPreview\\s?([0-9]+)?|"
         "\\sSpecials?\\s?-?\\s?([0-9]+)?|"
         "\\sPlay All\\s(.+)($|\\(|\\[)?|"
         "\\sEnding(\\s?[0-9]+)?"
        ")", Qt::CaseInsensitive);
    //regexEpisode.setMinimal(true);
    int episodeIndex = regexEpisode.indexIn(path);
    this->episodeNumber = regexEpisode.cap(1).trimmed();

    path.remove(episodeIndex, regexEpisode.cap(1).length());

    QRegExp regexSeason("((SE?)[0-9]+|(Season\\s?)[0-9]+)", Qt::CaseInsensitive);
    regexSeason.setMinimal(true);
    int seasonIndex = regexSeason.indexIn(path);
    if (seasonIndex != -1) {
        this->seasonName = regexSeason.cap(1);
        path.remove(seasonIndex, regexSeason.cap(1).length());
    }

    // check epNum after epName
    if (this->episodeNumber.isEmpty() || this->episodeNumber.isNull()) {
        //QRegExp regexNumberAfterShowName("(\\s[0-9]+\\s?)$");
        //int epIndex = regexNumberAfterShowName.indexIn(mShowName);
        int epIndex = regexEpisode.indexIn(this->showName);
        // TODO check all caps for the longest
        if (epIndex != -1) {
            this->episodeNumber = regexEpisode.cap(1).trimmed();
            this->showName.remove(epIndex, regexEpisode.cap(1).length());

            QRegExp regexEpName("(.*)");
            int epNameIndex = regexEpName.indexIn(this->showName, epIndex);
            if (epNameIndex != -1) {
                this->episodeName = regexEpName.cap(1).trimmed();
                this->showName.remove(epIndex, regexEpName.cap(1).length());
            }
            this->showName = this->showName.trimmed();
        }
    }

    // release group at the end (last word)
    if (this->releaseGroup.isEmpty()) {
        QRegExp releaseGroupAtEnd("([^\\s]+)($|\\s)");
        int occurance = path.indexOf(releaseGroupAtEnd);
        int latestOccurance = occurance;
        while (occurance != -1) {
            // ignore file version at the end like: " - THORA 1.0v2.1.vid"
            QRegExp releaseVersionRegex("([0-9\\.]+)?(v[0-9\\.])");
            if (-1 == releaseVersionRegex.indexIn(releaseGroupAtEnd.cap(1))) {
                latestOccurance = occurance;
            } else {
                path.remove(releaseVersionRegex);
            }
            occurance = path.indexOf(releaseGroupAtEnd, occurance + releaseGroupAtEnd.cap(0).length());
        }

        if (latestOccurance != -1) {
            path.indexOf(releaseGroupAtEnd, latestOccurance);
            this->releaseGroup = releaseGroupAtEnd.cap(1);
            path.remove(latestOccurance, releaseGroupAtEnd.cap(1).length());
        }
    }

    QRegExp epNameRegex("-\\s?(.*)$");
    if (episodeName.isEmpty() && -1 != epNameRegex.indexIn(path)) {
        episodeName = epNameRegex.cap(1);
        episodeName = episodeName.trimmed();
    }

    // TODO less redundant get an isSpecial function with \\s prefixes
    if (0 == showName.compare("ED", Qt::CaseInsensitive) ||
        0 == showName.compare("OP", Qt::CaseInsensitive) ||
        0 == showName.compare("Episode", Qt::CaseInsensitive) ||
        0 == showName.compare("EP", Qt::CaseInsensitive)) {

        const MovieFile parentDir(QFileInfo(originalPath).absolutePath());
        this->episodeNumber = this->showName;
        this->showName = parentDir.showName;

    }
}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv$|\\.ogv$|\\.mpeg$|\\.mp4$|\\.webm$|\\.avi$|\\.mp5$", Qt::CaseInsensitive));
}

QString MovieFile::xbmcEpisodeNumber() const {
    int num = numericEpisodeNumber();
    if (num == SPECIAL) {
        return QString("0x%1").arg(episodeNumber);
    }
    if (num != UNKNOWN) {
        return QString::number(num);
    }
    return episodeNumber;
}


QString MovieFile::fileExtension() const {
    return QFileInfo(path).completeSuffix();
}

bool MovieFile::exists() const {
    return QFile::exists(path);
}

QString MovieFile::xbmcEpisodeName() const {
    if (episodeName.length() > 0) {
        return episodeName;
    }
    return "Episode";
}

bool MovieFile::isSpecial(QString episodeNumberString) {
    QRegExp specialRegex("("
        "ED(\\s|$)|"
        "OP(\\s|$)|"

        "ED\\s?[0-9]+|"
        "OP\\s?[0-9]+[a-z]?|"
        "SP\\s?[0-9]+|"
        "NC.?OP\\s?([0-9]+)?|"
        "NC.?ED\\s?([0-9]+)?|"
        "EX\\s?([0-9]+)?|"
        "PV\\s?([0-9]+)?|"

        "Opening(\\s?[0-9]+)?|"
        "Preview\\s?([0-9]+)?|"
        "Specials?|"
        "Play All|"
        "Ending(\\s?[0-9]+)?"
        ")", Qt::CaseInsensitive);
    bool is = -1 != specialRegex.indexIn(episodeNumberString);
    //qDebug() << specialRegex.cap(1);
    return is;
}

bool MovieFile::isSpecial() const {
    return isSpecial(this->episodeNumber);
}

float MovieFile::numericEpisodeNumber() const {
    if (isSpecial()) {
        return SPECIAL;
    }
    QRegExp pureNumber("([0-9\\.]+x)?([0-9\\.]+)", Qt::CaseInsensitive);
    int index = pureNumber.indexIn(episodeNumber);
    if (index != -1) {
        // this is for files that don't have a space after the "showname -epnum" dash
        // Maybe I should move specials TO 0xFFFF and allow negative numbers
        float ep = pureNumber.cap(2).toFloat();
        return ep >= 0 ? ep : -1.f*ep;
    }
    return UNKNOWN;
}
