#ifndef GIFCREATOR_H
#define GIFCREATOR_H

#include "shortclipcreator.h"

class GifCreator : public ShortClipCreator
{
    Q_OBJECT
public:
    class Config : public ShortClipCreator::Config {
    public:
        Config();

        int framesDropped;
    };

    explicit GifCreator(Config* config, QObject *parent = 0);
    void init(QString videoPath, QString outputPath, float startSec, float endSec, std::pair<int, int> resolution, float maxSizeMib = 3, int framesDropped = 2);

    bool generate();
public slots:
    
private:
    QString videoPath;
    QString outputPath;
    float startSec;
    float endSec;
    std::pair<int, int> resolution;
    float maxSizeMib;
    int framesDropped;
};

#endif // GIFCREATOR_H
