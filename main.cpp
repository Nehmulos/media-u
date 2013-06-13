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

        if (strcmp(argv[i], "--configdir")) {
            configPath = (QString(argv[i]));

            QDir dir(configPath);
            if (!dir.exists() && !dir.mkdir(QString(argv[i]))) {
                configPath = QString();
            }
        } else if (strcmp(argv[i], "--fullscreen")) {
            fullscreen = true;
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
    Server s(publicDir.path(), w, player);
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
    //library.downloadWallpapers();
    return a.exec();
}
