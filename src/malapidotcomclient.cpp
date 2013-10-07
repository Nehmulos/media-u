#include "malapidotcomclient.h"
#include <QDebug>

namespace MalApiDotCom {


Client::Client()
{
}

SearchResult Client::search(QString anime) {
    CurlResult userData;
    QString url = QString("http://mal-api.com/anime/search?q=%1").arg(anime);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        SearchResult result(userData, anime);
        return result;
    }
    return SearchResult();
}

CURL* Client::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}


void RelatedEntry::parseForManga(nw::Describer* de) {
    NwUtils::describe(*de, "manga_id", id);
    NwUtils::describe(*de, "title", title);
}

void RelatedEntry::parseForAnime(nw::Describer* de) {
    NwUtils::describe(*de, "anime_id", id);
    NwUtils::describe(*de, "title", title);
}

void RelatedEntry::parseFromList(nw::Describer *de, QString arrayName, QList<RelatedEntry>& list, const bool anime) {
    list.empty();
    de->describeArray(arrayName.toUtf8().data(), "", 0);
    for (int i=0; de->enterNextElement(i); ++i) {
        RelatedEntry entry;
        if (anime) {
            entry.parseForAnime(de);
        } else {
            entry.parseForManga(de);
        }
        list.append(entry);
    }
}



void Thread::run()
{
}

SearchResult::SearchResult() {}

SearchResult::SearchResult(CurlResult &response, QString searchedAnime) :
    searchedAnime(searchedAnime)
{
    nw::JsonReader jr(response.data);
    this->describe(&jr);
    jr.close();
}

void SearchResult::describe(nw::Describer* const de) {
    de->describeArray("Entries", "Entry", 0);
    for (int i=0; de->enterNextElement(i); ++i) {
        entries.push_back(Entry(de));
    }
}

Entry::Entry(nw::Describer *de) {
    describe(de);
}

void Entry::describe(nw::Describer *de) {
    NwUtils::describe(*de, "id", id);

    NwUtils::describe(*de, "title", title);
    //other_titles - A hash/dictionary containing other titles this anime has.
    NwUtils::describeValueArray(*de, "synonyms",synonyms);
    NwUtils::describeValueArray(*de, "englishTitles",englishTitles);
    NwUtils::describeValueArray(*de, "japaneseTitles", japaneseTitles);

    NwUtils::describe(*de, "image_url", image_url);
    NwUtils::describe(*de, "type", type);
    NwUtils::describe(*de, "episodes", episodes);
    NwUtils::describe(*de, "status", status);
    NwUtils::describe(*de, "start_date", start_date);
    NwUtils::describe(*de, "end_date", end_date);
    NwUtils::describe(*de, "classification", classification);
    NwUtils::describe(*de, "synopsis", synopsis);
    NwUtils::describeValueArray(*de, "genres", genres);
    NwUtils::describeValueArray(*de, "tags", tags);
    RelatedEntry::parseFromList(de, "manga_adaptations", manga_adaptations, false);
    RelatedEntry::parseFromList(de, "prequels", prequels, true);
    RelatedEntry::parseFromList(de, "sequels", sequels, true);
    RelatedEntry::parseFromList(de, "side_stories", side_stories, true);
    parent_story.parseForAnime(de);
    RelatedEntry::parseFromList(de, "character_anime", character_anime, true);
    RelatedEntry::parseFromList(de, "spin_offs", spin_offs, true);
    RelatedEntry::parseFromList(de, "summaries", summaries, true);
    RelatedEntry::parseFromList(de, "alternative_versions", alternative_versions, true);
}



} // namespace
