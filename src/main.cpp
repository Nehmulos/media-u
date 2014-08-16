#include "mainwindow.h"
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include <QDesktopServices>
#include "config.h"
#include "server.h"
#include "library.h"
#include "malclient.h"
#include "mplayer.h"
#include "omxplayer.h"
#include "metadataparseravconv.h"
#include "thumbnailcreatoravconv.h"
#include "transmissionclient.h"
#include "snapshotwallpapergenerator.h"
#include "nyaarss.h"

#include <string.h>
#include <curl/curl.h>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    curl_global_init(CURL_GLOBAL_SSL);

    // TODO move argument parsing partly back here, since it calls exit(0)
    BaseConfig config(argc, argv);

    Library library(config.libraryPath());
    library.readAll();

    MainWindow w(library);
    if (!config.getNoGui()) {
        if (config.getFullScreen()) {
            w.showFullScreen();
        } else {
            w.show();
        }
    }

    VideoPlayer* player;
    /* omx support is dead right now, sorry
    if (config.omxPlayerIsInstalled()) {
        player = new Omxplayer(library, config);
    } else {
        player = new Mplayer(library, config, config.getMplayerConfigConstRef());
    }
    */
    player = new Mplayer(library, config, config.getMplayerConfigConstRef());


    MetaDataParserAvconv metaDataParser(config.getAvconvConfigConstRef());
    ThumbnailCreatorAvconv thumbnailCreator;
    player->setMetaDataParser(&metaDataParser);
    player->setThumbnailCreator(&thumbnailCreator);
    library.setMetaDataParser(&metaDataParser);

    library.addWallpaperDownloader(new SnapshotWallpaperGenerator::Client(metaDataParser, thumbnailCreator));

    // TODO move into the thread
    const RssConfig& rssConfig = config.getRssConfigConstRef();
    TransmissionClient transmission;
    NyaaRss::Client nyaaClient(transmission, library, rssConfig);
    TorrentRss::Thread rssThread(nyaaClient);
    rssThread.start(QThread::LowestPriority);
    //nyaaClient.moveToThread(&rssThread);
    nyaaClient.connectLibrary();


    QDir publicDir = QDir::current();
    publicDir.cd("public");
    Server s(publicDir.path(), w, library, player);
    const int port = s.start(config.serverPort());

    if (config.getAutoOpenBrowser()) {
        QString url(QString("http://localhost:%1").arg(port));
        QDesktopServices::openUrl(url);
    }

    //w.statusBar()->showMessage(QString("Launched on port %1").arg(port));
    w.setPage("MainPage", QString("Launched on port %1").arg(port));

    library.initOnlineSync(config);
    library.startSearch();

    int returnCode = a.exec();
    library.write(); // write before exit

    rssThread.toldToStop = true;
    rssThread.wait();

    return returnCode;
}
