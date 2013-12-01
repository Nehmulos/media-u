#include "nyaarss.h"

namespace NyaaRss {

Feed::Feed(QString url) : TorrentRss::Feed(url)
{
}

TorrentRss::FeedResult* Feed::createFeedResult() {
    return new FeedResult();
}

void FeedResult::parse(CurlResult& result) {
    nw::XmlReader xr(result.data);
    xr.push("rss");
    xr.push("channel");
    xr.describeArray("", "item", -1);
    for (int i=0; xr.enterNextElement(i); ++i) {
        Entry* entry = new Entry();
        entry->parse(&xr);
        this->entires.push_back(entry);
    }
    xr.close();
}



void Entry::parse(nw::Describer* de) {
    QString dateString;
    QString typeStr;
    NwUtils::describe(*de, "title", this->name);
    NwUtils::describe(*de, "link", this->url);
    NwUtils::describe(*de, "pubDate", dateString);
    NwUtils::describe(*de, "category", typeStr);

    if (typeStr == Entry::rawAnimeStr) type = rawAnime;
    else if (typeStr == Entry::nonEnglishAnimeStr) type = nonEnglishAnime;
    else if (typeStr == Entry::englishAnimeStr) type = englishAnime;
    else type = other;

    this->date = parseDate(dateString);
}

 const QString Entry::rawAnimeStr = "Raw Anime";
 const QString Entry::nonEnglishAnimeStr = "Non-English-translated Anime";
 const QString Entry::englishAnimeStr = "English-translated Anime";



// I could not find a way to solve this with the default format
QDateTime parseDate(QString string) {
    // example: Sat, 30 Nov 2013 16:41:27 +0000
    QRegExp regex("..., ([0-9]+) ([a-zA-Z]+) ([0-9]+ [0-9:]+)");
    int found = regex.indexIn(string);
    if (-1 != found) {
        static const QStringList months =
                QStringList() << "Jan" << "Feb" << "Mar" << "Apr" <<
                                 "May" << "Jun" << "Jul" << "Aug" <<
                                 "Sep" << "Oct" << "Nov" << "Dez";
        int month = months.indexOf(regex.cap(2));
        if (-1 == month) {
            return QDateTime();
        }
        month += 1;
        QString monthString = QString("%1").arg(month, 2, 10, QChar('0'));
        QString simplified = QString("%1 %2 %3").arg(regex.cap(1), monthString, regex.cap(3));
        return QDateTime::fromString(simplified, "dd MM yyyy hh:mm:ss");
    }
    return QDateTime();
}

Client::Client(TorrentClient& torrentClient, Library& library, QObject* parent) :
    TorrentRss::Client(torrentClient, library, parent)
{
}

void Client::addFeed(TvShow* tvShow) {
    int nextEpisode = tvShow->episodeList().highestDownloadedEpisodeNumber() + 1;
    QString query = QString("%1 %2").arg(QString::number(nextEpisode), tvShow->name());
    QString url = QString("http://www.nyaa.se/?page=rss&term=%1").arg(query);
    this->feeds.push_back(new Feed(url));
}

} // < namespace
