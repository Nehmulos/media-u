#ifndef MPLAYER_H
#define MPLAYER_H

#include "videoplayer.h"
#include <QObject>

class Mplayer : public VideoPlayer
{
    Q_OBJECT
public:
    Mplayer();
    virtual ~Mplayer();
    virtual int playFile(QString filepath);

    virtual void pause();
    virtual void unPause();

    virtual void stop();

    virtual void backwards(const int seconds = 5);
    virtual void forwards(const int seconds = 5);

    virtual float incrementVolume();
    virtual float decrementVolume();

private slots:
    void onProcessFinished(int exitCode);
    void onProcessOutput();
};

#endif // MPLAYER_H
