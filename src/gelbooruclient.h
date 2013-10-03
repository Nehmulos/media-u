#ifndef GELBOORUCLIENT_H
#define GELBOORUCLIENT_H


#include <curl/curl.h>
#include <QString>
#include <QThread>
#include <QDir>
#include <sstream>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "filedownloadthread.h"
#include "wallpaperdownloadclient.h"

namespace Gelbooru {
using WallpaperDownload::FetchThread;
using WallpaperDownload::Rating;
using WallpaperDownload::SearchResult;
using WallpaperDownload::Entry;

class Client : public WallpaperDownload::Client
{
public:
    Client(QString baseUrl = "http://gelbooru.com", int limit = 10, Rating ratingFilter = WallpaperDownload::ratingSafe);

    SearchResult fetchPostsBlocking(QString tagName, int page = 1);
    Entry parseEntry(nw::Describer *de);
    SearchResult parseSearchResult(std::stringstream &ss, int limit);
protected:
    CURL* curlClient(QString tag, CurlResult& userdata, const unsigned int page = 1);
};

}

#endif // GELBOORUCLIENT_H
