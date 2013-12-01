#ifndef NYAARSS_H
#define NYAARSS_H

#include "torrentrss.h"
#include "nwutils.h"

namespace NyaaRss {

class Entry : public TorrentRss::Entry {
public:
    void parse(nw::Describer* de);

    enum Type {
        notSet,
        invalid,
        rawAnime,        //"Raw Anime"
        nonEnglishAnime, //"Non-English-translated Anime"
        englishAnime,    //"English-translated Anime"
        other
    };

    static const QString rawAnimeStr;
    static const QString nonEnglishAnimeStr;
    static const QString englishAnimeStr;

protected:
    Type type;
};

class FeedResult : public TorrentRss::FeedResult {
    void parse(CurlResult &result);
};

class Feed : public TorrentRss::Feed {
public:
    Feed(QString url);
    TorrentRss::FeedResult* createFeedResult();
};

class Client : public TorrentRss::Client {
public:
    Client(TorrentClient &torrentClient, Library &library, QObject *parent = NULL);

    void addFeed(TvShow* show);
};

    QDateTime parseDate(QString dateString);

}

#endif // NYAARSS_H
