#include "mainpage.h"
#include "ui_mainpage.h"
#include "tvshowlistwidget.h"
#include "server.h"

MainPage::MainPage(Library& library, QWidget *parent) :
    Page(parent),
    ui(new Ui::MainPage),
    library(library)
{
    ui->setupUi(this);

    QString backgroundPath = library.randomWallpaperPath();
    //this->setStyleSheet(QString("background-color:black;background-image: url('%1');").arg(backgroundPath));

    // TODO update ui
    //this->ui->currentlyAiringShows = new TvShowListWidget();
    this->airingShows = library.filter().airing();
    this->allShows = library.filter().all();
    dynamic_cast<TvShowListWidget*>(this->ui->currentlyAiringShows)->set(airingShows, QString("Airing Shows"));
}

MainPage::~MainPage()
{
    delete ui;
}

bool MainPage::handleApiRequest(QHttpRequest *, QHttpResponse *)
{
    return false;
}
