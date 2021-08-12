
#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <QDateTime>
#include "imagestructs.h"

// configuration options that apply to an image/folder of images
struct Config {
  public:
    unsigned int rotationSeconds = 30;
    int blurRadius = -1;
    int backgroundOpacity = -1;
    ImageDisplayOptions baseDisplayOptions;
    QDateTime loadTime;
};

// app level configuration
struct AppConfig : public Config
{
    AppConfig() {}
    AppConfig( const Config &inConfig ) : Config(inConfig) {}
    std::string path = "";
    std::string configPath = "";

    std::string overlay = "";
    std::string imageList = ""; // comma delimited list of images to show

    bool recursive = false;
    bool shuffle = false;
    bool sorted = false;
    bool debugMode = false;
    static const std::string valid_aspects; 

  public:
    bool PathOptionsChanged(AppConfig &other) {
      if ( other.recursive != recursive || other.shuffle != shuffle
      || other.sorted != sorted)
        return true;
      if ( other.path != path || other.imageList != imageList )
        return true;
      return false;
    }
};


AppConfig loadAppConfiguration(const AppConfig &commandLineConfig);
Config getConfigurationForFolder(const std::string &folderPath, const Config &currentConfig, bool debugMode);

ImageAspect parseAspectFromString(char aspect);
QString getAppConfigFilePath(const std::string &configPath);

#endif