
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
  std::string path = "";
  std::string imageList = "";
  std::string rssFeedURL = "";
  bool exclusive = false; // only use this entry when it is valid, skip others

  bool recursive = false;
  bool shuffle = false;
  bool sorted = false;
  ImageDisplayOptions baseDisplayOptions;

  bool operator==(const PathEntry &b) const
  {
    return !operator!=(b);
  }
  bool operator!=(const PathEntry &b) const
  {
    if (b.exclusive != exclusive)
      return true;
    if (b.recursive != recursive || b.shuffle != shuffle || b.sorted != sorted)
      return true;
    if(b.baseDisplayOptions.fitAspectAxisToWindow != baseDisplayOptions.fitAspectAxisToWindow)
      return true;
    if (b.path != path || b.imageList != imageList || b.rssFeedURL != rssFeedURL)
      return true;
    if (b.baseDisplayOptions.timeWindows.count() != baseDisplayOptions.timeWindows.count())
      return true;
    for(int i = 0; i < baseDisplayOptions.timeWindows.count(); ++i)
    {
      if (b.baseDisplayOptions.timeWindows[i] != baseDisplayOptions.timeWindows[i])
        return true;
    }
    return false;
  }
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
    bool PathOptionsChanged(AppConfig &other) 
    {
      if (paths.count() != other.paths.count())
        return true;
      for(int index = 0; index < paths.count(); ++index)
      {
        if(other.paths[index] != paths[index])
          return true;
      }
      return false;
    }
};

AppConfig loadAppConfiguration(const AppConfig &commandLineConfig);
Config getConfigurationForFolder(const std::string &folderPath, const Config &currentConfig);

ImageAspect parseAspectFromString(char aspect);
QString getAppConfigFilePath(const std::string &configPath);

#endif