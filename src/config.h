#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QDir>
#include "nwutils.h"
#include "videoclipcreator.h"
#include "gifcreator.h"

class MplayerConfig {
public:
    void describe(nw::Describer*de);
    QString path;
    QStringList arguments;
private:
    bool needsInit();
    void setDefaultValues();
};

class SnapshotConfig {
public:
    void describe(nw::Describer& de);
    QString snapshotDir;
    QString snapshotFormat;
    QString snapshotName;
    qint8 snapshotQuality;

private:
    bool needsInit();
    void setDefaultValues();
    QString createSaveDir(const QString parentDir, const QString dirname);
};

class RssConfig {
public:
    RssConfig();
    void describe(nw::Describer& de);
    bool autoDownload;
    bool requireFavouriteReleaseGroup;
    bool includeEnglish;
    bool includeRaw;
};

// I am consequently calling it avconv instead of ffmpeg,
// because the debian package maintainers managed to fool me that ffmpeg
// is deprecated and not just forked, now I don't want to change it everywhere.
class AvconvConfig {
public:
    QString command;
    static AvconvConfig detectCommand();
private:
    AvconvConfig(QString command);
};

class BaseConfig
{
public:
    BaseConfig(int argc, char* argv[]);
    BaseConfig(QString initPath);
    void fromArgs(int argc, char* argv[]);

    bool init(QString path = QString());
    bool parse(const QString &jsonData);
    bool createNewConfig(QString filepath);
    void describe(nw::Describer *de);

    QString libraryPath();
    int serverPort() const;

    QDir configPath() const;
    QString malConfigFilePath() const;
    QString anilistConfigFilePath() const;

    QString mplayerLocation();
    QString omxplayerLocation();
    bool omxPlayerIsInstalled();
    bool mplayerIsInstalled();

    std::pair<ShortClipCreator *, ShortClipCreator::Config *> cloneGifShortClipCreator() const;
    std::pair<ShortClipCreator *, ShortClipCreator::Config *> cloneWebmShortClipCreator() const;
    const SnapshotConfig& getSnapshotConfigConstRef() const;
    const MplayerConfig& getMplayerConfigConstRef() const;
    const RssConfig& getRssConfigConstRef() const;
    const AvconvConfig& getAvconvConfigConstRef() const;

    bool getNoGui() const;
    bool getFullScreen() const;
    bool getAutoOpenBrowser() const;

    const VideoClipCreator::Config& getVideoClipCreatorConfig() const;
    const GifCreator::Config& getGifCreatorConfig() const;
private:
    QString mConfigPath;
    QString mLibraryPath;
    QString shortClipCreatorType;

    int mServerPort;
    bool noGui;
    bool fullScreen;
    bool autoOpenBrowser;

    bool initialized;

    SnapshotConfig snapshotConfig;
    MplayerConfig mplayerConfig;
    RssConfig rssConfig;
    AvconvConfig avconvConfig;

    VideoClipCreator::Config videoClipCreatorConfig;
    GifCreator::Config gifCreatorConfig;

    void setDefaults();
};

#endif // CONFIG_H
