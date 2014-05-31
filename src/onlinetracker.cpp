#include "onlinetracker.h"

OnlineTracker::OnlineTracker(OnlineCredentials& credentials, QObject *parent) :
    QObject(parent),
    credentials(credentials)
{
}

bool OnlineTracker::updateRemote(TvShow* show) {
    OnlineTracker::EntryList* entries = satisfyingEntries();
    if (!entries) {
        return false;
    }

    OnlineTracker::UpdateResult result = this->updateRemoteImpl(show, *entries);
    switch (result) {
    case OnlineTracker::success:
        show->setLastOnlineTrackerUpdate(this->identifierKey(), QDateTime::currentDateTimeUtc());
        return true;
    case OnlineTracker::alreadySameAsLocal: {
        const OnlineTracker::Entry* entry = entries->get(this->identifierKey(), show);
        if (entry) {
            show->setLastOnlineTrackerUpdate(this->identifierKey(), entry->lastUpdate);
        }
        return true;
    }
    case OnlineTracker::skipDueToNoChanges:
        return true;
    case OnlineTracker::invalid:
    case OnlineTracker::failedDueToMissingData:
    case OnlineTracker::failedDueToNetwork:
    default:
        return false;
    }
}


OnlineTracker::EntryList*OnlineTracker::satisfyingEntries() {
    if (!cachedEntries || cachedEntries->tooOld()) {
        this->cachedEntries = fetchRemote();
    }
    return this->cachedEntries;
}

OnlineTracker::EntryList::EntryList() :
    fetchTime(QDateTime::currentDateTimeUtc())
{
}

bool OnlineTracker::EntryList::tooOld() const {
    return this->fetchTime.addSecs(60 * 5) < QDateTime::currentDateTimeUtc();
}
