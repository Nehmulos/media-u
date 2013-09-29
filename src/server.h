#ifndef SERVER_H
#define SERVER_H

#include <qhttpserver.h>
#include <qhttpresponse.h>
#include <qhttprequest.h>

#include <QFile>
#include <QDir>

#include "mainwindow.h"
#include "videoplayer.h"
#include "apipushevents.h"

namespace mime {
const QString json = "application/json";
const QString text = "text/plain";
};

class RequestBodyListener : public QObject {
    Q_OBJECT
public:
    RequestBodyListener(QHttpResponse* resp, QObject* parent);

signals:
    void bodyReceived(QHttpRequest* req, QHttpResponse* resp);

public slots:
    void onRequestEnd();

private:
    QHttpResponse* resp;
};

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString publicDirectoryPath, MainWindow& window, Library& library, VideoPlayer* player);
    int start(int serverPort); ///< returns the listening port
    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    void sendFile(QHttpRequest *req, QHttpResponse *resp);


    static void simpleWrite(QHttpResponse *resp, int statusCode, const QString &data, QString mime = mime::text);
    static void simpleWriteBytes(QHttpResponse *resp, int statusCode, const QByteArray &data, QString mime = mime::text);
    void streamVideo(QHttpRequest *req, QHttpResponse *resp);
public slots:
    void handleRequest(QHttpRequest *req, QHttpResponse* resp);

private:
    QHttpServer* server;
    QDir publicDirectory; ///< send files in this directory as HTTP GET responses
    MainWindow& window;
    Library& library;
    VideoPlayer* player;
    ApiPushEvents pushEvents;
    Q_DISABLE_COPY(Server)
};

#endif // SERVER_H
