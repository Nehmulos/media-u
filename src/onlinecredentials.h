#ifndef ONLINECREDENTIALS_H
#define ONLINECREDENTIALS_H

#include "curlresult.h"
#include <QString>
#include <QTime>

class OnlineCredentials
{
public:
    class TimeLock {
    public:
        TimeLock(int timeToWaitInMs);
        /// puts thread asleep til ready,
        /// returns false if ready was stolen from other thread
        bool blockUntilReady();

    private:
        bool lock();
        QTime timer;
        int timeToWaitInMs;
    };

    OnlineCredentials();
    void set(const QString name, const QString password);
    void set(const QString name, const QString password, QString userAgent);
    virtual bool login() {return mHasVerifiedCredentials || this->verifyCredentials();}

    virtual const QString identifierKey() const = 0;
    virtual bool fetchFirstAuthorizeToken(QString /*code*/) { return false; }
    virtual const QString connectUri() const { return ""; }
    virtual bool isFresh() { return true; }
    virtual bool refresh() { return true; }
    bool assureFreshness();

    CURL* curlClient(const char* url, CurlResult& userdata);
    CURL* curlClient(TimeLock& lock, const char *url, CurlResult &userdata) const;
    CURL* curlNoAuthClient(const char* url, CurlResult& userdata);
    CURL* curlNoAuthClient(TimeLock &lock, const char *url, CurlResult &userdata) const;

    bool readConfig(QString configFilePath);
    bool hasVerifiedCredentials() const;
    QString getUsername() const;

    TimeLock lock;

protected:
    virtual bool verifyCredentials() = 0;
    virtual void setCredentialsForHandle(CurlResult &userdata, CURL *handle) const;


    bool mHasVerifiedCredentials;
    QString username;
    QString password;
    QString userAgent;

private:
    CURL* curlClientNoLock(const char* url, CurlResult &userdata) const;
    CURL* curlNoAuthClientNoLock(const char* url, CurlResult& userdata) const;
};

#endif // ONLINECREDENTIALS_H
