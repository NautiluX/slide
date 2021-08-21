
#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <QDateTime>
#include "imagestructs.h"
#include <QVector>

// configuration options that apply to an image/folder of images
struct Config {
  public:
    unsigned int rotationSeconds = 30;
    int blurRadius = -1;
    int backgroundOpacity = -1;
    ImageDisplayOptions baseDisplayOptions;
    QDateTime loadTime;
};

struct PathEntry {
  QVector<DisplayTimeWindow> timeWindows;
  std::string path = "";
  std::string imageList = "";
  std::string type = "";
  bool exclusive = false;

  bool recursive = false;
  bool shuffle = false;
  bool sorted = false;
};

// app level configuration
struct AppConfig : public Config {
    AppConfig() {}
    AppConfig( const Config &inConfig ) : Config(inConfig) {}
    std::string configPath = "";
    std::string overlay = "";
    QVector<PathEntry> paths;

    bool debugMode = false;

    static const std::string valid_aspects; 
  public:
    bool PathOptionsChanged(AppConfig &other) {
      if (paths.count() != other.paths.count())
        return true;
      for(int index = 0; index < paths.count(); ++index)
      {
        if ( other.paths[index].recursive != paths[index].recursive || other.paths[index].shuffle != paths[index].shuffle
        || other.paths[index].sorted != paths[index].sorted)
          return true;
        if ( other.paths[index].path != paths[index].path || other.paths[index].imageList != paths[index].imageList )
          return true;
      }
     return false;
    }
};

AppConfig loadAppConfiguration(const AppConfig &commandLineConfig);
Config getConfigurationForFolder(const std::string &folderPath, const Config &currentConfig, bool debugMode);

ImageAspect parseAspectFromString(char aspect);
QString getAppConfigFilePath(const std::string &configPath);

#endif