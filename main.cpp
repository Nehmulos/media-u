#include "mainwindow.h"
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include "config.h"
#include "server.h"
#include "library.h"
#include "directoryscanner.h"
#include "tvshowscanner.h"
#include "malclient.h"
#include "mplayer.h"

#include <string.h>
#include <curl/curl.h>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    curl_global_init(CURL_GLOBAL_SSL);

    QString configPath;
    bool fullscreen = false;

    for (int i=0; i < argc; ++i) {

        if (strcmp(argv[i], "--configdir") == 0) {
            ++i;
            if (i < argc) {
                configPath = (QString(argv[i]));

                QDir dir(configPath);
                if (!dir.exists() && !QDir::root().mkpath(QString(argv[i]))) {
                    configPath = QString();
                    qDebug() << "config dir does not exist and couldn't be created";
                }
            }
        } else if (strcmp(argv[i], "--fullscreen") == 0) {
            fullscreen = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            std::cout << "A Qt Application to manage and play your media library.\n\n";
            std::cout << "available Arguments:\n";
            std::cout << "--fullscreen\n";
            std::cout << "    launch qt gui in fullscreen.\n";
            std::cout << "--configdir %dir\n";
            std::cout << "    %dir path to a directory where configs shall be written/read\n";
            std::cout << "    if %dir does not exist it will be created. If it can't be created\n";
            std::cout << "    .media-u in the home directory will be the default configdir.\n";
            std::cout.flush();
            return 0;
        }
    }

    Config config(configPath);

    Library library(config.libraryPath());
    library.readAll();

    MainWindow w(library);
    if (fullscreen) {
        w.showFullScreen();
    } else {
        w.show();
    }

    VideoPlayer* player = new Mplayer(); // TODO get player from config

    QDir publicDir = QDir::current();
    publicDir.cd("public");
    Server s(publicDir.path(), w, library, player);
    int port = s.start(config.serverPort());

    w.statusBar()->showMessage(QString("Launched on port %1").arg(port));
    w.setPage("MainPage");

    // debug scanner
    DirectoryScanner scanner;
    scanner.addScanner(new TvShowScanner(library));
    scanner.scan("/mnt/fields1/torrents/");
    scanner.scan("/media/nehmulos/INTENSO/anime");
   //library.write();

    library.initMalClient(config.malConfigFilePath());
    library.fetchMetaData();

    // time to die
    library.downloadWallpapers();
    //player->playFile("/media/nehmulos/INTENSO/anime/[Commie] Inferno Cop/[Commie] Inferno Cop - 04v2 [B6264EE0].mkv");
    return a.exec();
}
