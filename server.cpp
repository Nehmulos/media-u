#include "server.h"
#include <iostream>
#include <QFile>
#include <string>
#include <qhttprequest.h>
#include "systemutils.h"

Server::Server(QString publicDirectoryPath, MainWindow &window) :
    QObject(NULL), publicDirectory(publicDirectoryPath),
    window(window)
{

}

int Server::start(int serverPort) {
    this->server = new QHttpServer();

    int port = serverPort;
    while (port >= serverPort && !this->server->listen(QHostAddress(QHostAddress::Any), port)) {
        ++port;
    }

    connect(this->server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

     return port;
}

void Server::handleRequest(QHttpRequest* req, QHttpResponse* resp) {
    std::cout << "Connection" << std::endl;

    if (req->path().contains(QRegExp("^\\/api\\/"))) {
        if (!handleApiRequest(req, resp)) {
            simpleWrite(resp, 405, "Api request not supported. Maybe a typo");
        }
    } else {
        sendFile(req, resp);
    }
}

bool Server::handleApiRequest(QHttpRequest* req, QHttpResponse* resp) {
    QString path = req->path();
    if (path.startsWith(QString("/api/changePage"))) {
        QString pageName = req->url().toString();
        QRegExp regex("\\?(.+)$");
        regex.indexIn(pageName);
        window.setPage(regex.cap(1));
        simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"page\":\"%1\"}").arg(regex.cap(1)));
        return true;
    } else if (path.startsWith("/api/activePage")) {
        simpleWrite(resp, 200, QString("{\"page\":\"%1\"}").arg(window.activePageId()));
        return true;
    } else if (path.startsWith("/api/airingTvShows")) {
        //simpleWrite(window.getLibrary().airingShowsJson());
        //return true;
    } else if (window.activePage()) {
        return window.activePage()->handleApiRequest(req, resp);
    }
    return false;
}

void Server::sendFile(QHttpRequest* req, QHttpResponse* resp) {
    QDir localDir = QDir(this->publicDirectory.path().append(req->path()));

    QString filePath;
    if (localDir.exists()) {
        filePath = localDir.absoluteFilePath("index.html");
    } else {
        QString path = req->path();
        if (path.at(0) == '/') {
            path = path.right(path.length()-1);
        }
        filePath = this->publicDirectory.absoluteFilePath(path);
    }
    std::cout << filePath.toStdString() << std::endl;


    if (!filePath.startsWith(this->publicDirectory.absolutePath())) {
        simpleWrite(resp, 403, QString("Access denied. Only files in the public directory will be send."));
        return;
    }

    QFile file(filePath);
    if (file.exists() && file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        simpleWrite(resp, 200, QString(data), SystemUtils::fileMime(filePath));
        file.close();
    } else {
        simpleWrite(resp, 404, "File not found (Error 404)");
    }
}

void Server::simpleWrite(QHttpResponse* resp, int statusCode, const QString& data, QString mime) {
    resp->setHeader("Content-Length", QString("%1").arg(data.size()));
    //resp->setHeader("Content-Type", mime);
    resp->writeHead(statusCode);
    resp->write(data);
    resp->end();
}
