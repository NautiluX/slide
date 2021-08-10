#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include "overlay.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QFileInfo>
#include <iostream>
#include <sys/file.h>
#include <errno.h>

#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>

void usage(std::string programName) {
    std::cerr << "Usage: " << programName << " [-t rotation_seconds] [-a aspect('l','p','a', 'm')] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder [-r] [-s] [-v] [--verbose] [--stretch] [-c config_file_path]" << std::endl;
}

struct Config {
  std::string path = "";
  std::string configPath = "";
  unsigned int rotationSeconds = 30;
  int blurRadius = -1;
  int backgroundOpacity = -1;
  bool recursive = false;
  bool shuffle = false;
  bool sorted = false;
  bool debugMode = false;
  ImageDisplayOptions baseDisplayOptions;
  std::string valid_aspects = "alpm"; // all, landscape, portait
  std::string overlay = "";
  std::string imageList = ""; // comma delimited list of images to show
  QDateTime loadTime;
};

ImageAspect parseAspectFromString(char aspect) {
  switch(aspect)
  {
    case 'l':
      return ImageAspect_Landscape;
      break;
    case 'p':
      return ImageAspect_Portrait;
      break;
    case 'm':
      return ImageAspect_Monitor;
      break;
    default:
    case 'a':
      return ImageAspect_Any;
      break;
  }
}

std::string ParseJSONString(QJsonObject jsonDoc, const char *key) {
  if(jsonDoc.contains(key) && jsonDoc[key].isString())
  {
   return jsonDoc[key].toString().toStdString();
  }
  return "";
}

void SetJSONBool(bool &value, QJsonObject jsonDoc, const char *key) {
  if(jsonDoc.contains(key) && jsonDoc[key].isBool())
  {
    value = jsonDoc[key].toBool();
  }
}

QString getConfigFilePath(const std::string &configPath) {
  std::string userConfigFolder = "~/.config/slide/";
  std::string systemConfigFolder = "/etc/slide";
  QString baseConfigFilename("slide.options.json");
  
  QDir directory(userConfigFolder.c_str());
  QString jsonFile = "";
  if (!configPath.empty())
  { 
    directory.setPath(configPath.c_str());
    jsonFile = directory.filePath(baseConfigFilename);    
  }
   if(!directory.exists(jsonFile))
  { 
    directory.setPath(userConfigFolder.c_str());
    jsonFile = directory.filePath(baseConfigFilename);    
  }
  if(!directory.exists(jsonFile))
  {
    directory.setPath(systemConfigFolder.c_str());
    jsonFile = directory.filePath(baseConfigFilename);
  }

  if(directory.exists(jsonFile))
  {
    return jsonFile;
  }

  return "";
}

Config loadConfiguration(const Config &commandLineConfig) {
  QString jsonFile = getConfigFilePath(commandLineConfig.configPath);
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return commandLineConfig; // nothing to load
  }

  Config userConfig = commandLineConfig;

  if(userConfig.debugMode)
  {
    std::cout << "Found options file: " << jsonFile.toStdString() << std::endl;
  }

  QString val;
  QFile file;
  file.setFileName(jsonFile);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  val = file.readAll();
  file.close();
  QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
  QJsonObject jsonDoc = d.object();
  SetJSONBool(userConfig.baseDisplayOptions.fitAspectAxisToWindow, jsonDoc, "fitAspectAxisToWindow");
  SetJSONBool(userConfig.recursive, jsonDoc, "recursive");
  SetJSONBool(userConfig.shuffle, jsonDoc, "shuffle");
  SetJSONBool(userConfig.debugMode, jsonDoc, "debug");

  std::string aspectString = ParseJSONString(jsonDoc, "aspect");
  if(!aspectString.empty())
  {
    userConfig.baseDisplayOptions.onlyAspect = parseAspectFromString(aspectString[0]);
  }
  if(jsonDoc.contains("rotationSeconds") && jsonDoc["rotationSeconds"].isDouble())
  {
    userConfig.rotationSeconds = (int)jsonDoc["rotationSeconds"].toDouble();
  }

  if(jsonDoc.contains("opacity") && jsonDoc["opacity"].isDouble())
  {
    userConfig.backgroundOpacity = (int)jsonDoc["opacity"].toDouble();
  }

  std::string overlayString = ParseJSONString(jsonDoc, "overlay");
  if(!overlayString.empty())
  {
    userConfig.overlay = overlayString;
  }
  std::string pathString = ParseJSONString(jsonDoc, "path");
  if(!pathString.empty())
  {
    userConfig.path = pathString;
  }

  userConfig.loadTime = QDateTime::currentDateTime();
  return userConfig;
}

bool parseCommandLine(Config &appConfig, int argc, char *argv[]) {
  int opt;
  int debugInt = 0;
  int stretchInt = 0;
  static struct option long_options[] =
  {
    {"verbose", no_argument,       &debugInt, 1},
    {"stretch", no_argument,       &stretchInt, 1},
  };
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "b:p:t:o:O:a:i:c:rsSv", long_options, &option_index)) != -1) {
    switch (opt) {
      case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          return false;
          break;
      case 'p':
        appConfig.path = optarg;
        break;
      case 'a':
        if (appConfig.valid_aspects.find(optarg[0]) == std::string::npos)
        {
          std::cout << "Invalid Aspect option, defaulting to all" << std::endl;
          appConfig.baseDisplayOptions.onlyAspect = ImageAspect_Any;
        }
        else
        {
          appConfig.baseDisplayOptions.onlyAspect = parseAspectFromString(optarg[0]);
        }
        break;
      case 't':
        appConfig.rotationSeconds = atoi(optarg);
        break;
      case 'b':
        appConfig.blurRadius = atoi(optarg);
        break;
      case 'o':
        appConfig.backgroundOpacity = atoi(optarg);
        break;
      case 'r':
        appConfig.recursive = true;
        break;
      case 's':
        appConfig.shuffle = true;
        std::cout << "Shuffle mode is on." << std::endl;
        break;
      case 'S':
        appConfig.sorted = true;
        break;
      case 'O':
        appConfig.overlay = optarg;
        break;
      case 'v':
        appConfig.debugMode = true;
        break;
      case 'i':
        appConfig.imageList = optarg;
        break;
      case 'c':
        appConfig.configPath = optarg;
        break;
      default: /* '?' */
        return false;
    }
  }
  
  if(debugInt==1)
  {
    appConfig.debugMode = true;
  }
  if(stretchInt==1)
  {
    appConfig.baseDisplayOptions.fitAspectAxisToWindow = true;
  }

  return true;
}

void ReloadConfigIfNeeded(Config &appConfig, MainWindow &w, ImageSwitcher &switcher, std::shared_ptr<ImageSelector> &selector)
{  
  QString jsonFile = getConfigFilePath(appConfig.configPath);
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return;
  }

  if(appConfig.loadTime <  QFileInfo(jsonFile).lastModified())
  {
    appConfig = loadConfiguration(appConfig);
    w.setBaseOptions(appConfig.baseDisplayOptions);
    w.setDebugMode(appConfig.debugMode);
    selector->setDebugMode(appConfig.debugMode);
    //Overlay o(appConfig.overlay);
    //w.setOverlay(&o);
    switcher.setRotationTime(appConfig.rotationSeconds * 1000);
  }
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow w;
  Config commandLineAppConfig;
  if (!parseCommandLine(commandLineAppConfig, argc, argv))
  {
    usage(argv[0]);
    return 1;
  }

  Config appConfig = loadConfiguration(commandLineAppConfig);

  if (appConfig.path.empty() && appConfig.imageList.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
  }

  if (appConfig.blurRadius>= 0)
  {
    w.setBlurRadius(appConfig.blurRadius);
  }

  if (appConfig.backgroundOpacity>= 0)
  {
      w.setBackgroundOpacity(appConfig.backgroundOpacity);
  }

  std::unique_ptr<PathTraverser> pathTraverser;
  if (!appConfig.imageList.empty())
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new ImageListPathTraverser(appConfig.imageList, appConfig.debugMode));
  }
  else if (appConfig.recursive)
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new RecursivePathTraverser(appConfig.path, appConfig.debugMode));
  }
  else
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new DefaultPathTraverser(appConfig.path, appConfig.debugMode));
  }

  std::shared_ptr<ImageSelector> selector;
  if (appConfig.sorted)
  {
    selector = std::shared_ptr<ImageSelector>(new SortedImageSelector(pathTraverser));
  }
  else if (appConfig.shuffle)
  {
    selector = std::shared_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser));
  }
  else
  {
    selector = std::shared_ptr<ImageSelector>(new RandomImageSelector(pathTraverser));
  }
  selector->setDebugMode(appConfig.debugMode);
  if(appConfig.debugMode)
  {
    std::cout << "Rotation Time: " << appConfig.rotationSeconds << std::endl;
    std::cout << "Overlay input: " << appConfig.overlay << std::endl;
  }
  Overlay o(appConfig.overlay);
  o.setDebugMode(appConfig.debugMode);
  w.setOverlay(&o);
  w.setBaseOptions(appConfig.baseDisplayOptions);
  w.show();

  ImageSwitcher switcher(w, appConfig.rotationSeconds * 1000, selector);
  w.setImageSwitcher(&switcher);
  std::function<void()> reloader = [&appConfig, &w, &switcher, &selector]() { ReloadConfigIfNeeded(appConfig, w, switcher, selector); };
  switcher.setConfigFileReloader(reloader);
  switcher.start();
  return a.exec();
}
