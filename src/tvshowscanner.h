#ifndef TVSHOWSCANNER_H
#define TVSHOWSCANNER_H

#include "mediascanner.h"

class Library;
class TvShowScanner : public MediaScanner
{
    Q_OBJECT
public:
    TvShowScanner(Library &library, QObject* parent = NULL);
    void scanFiles(const QStringList &files, const QDir &dir);

protected:
    Library& library;
};

#endif // TVSHOWSCANNER_H
