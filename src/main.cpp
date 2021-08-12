#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include "overlay.h"
#include "appconfig.h"

#include <QApplication>
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

bool parseCommandLine(AppConfig &appConfig, int argc, char *argv[]) {
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

void ConfigureWindowFromSettings(MainWindow &w, const AppConfig &appConfig)
{
  if (appConfig.blurRadius>= 0)
  {
    w.setBlurRadius(appConfig.blurRadius);
  }

  if (appConfig.backgroundOpacity>= 0)
  {
      w.setBackgroundOpacity(appConfig.backgroundOpacity);
  }
  std::unique_ptr<Overlay> o = std::unique_ptr<Overlay>(new Overlay(appConfig.overlay));
  o->setDebugMode(appConfig.debugMode);
  w.setDebugMode(appConfig.debugMode);
  w.setOverlay(o);
  w.setBaseOptions(appConfig.baseDisplayOptions);
}

std::unique_ptr<ImageSelector> GetSelectorForConfig(const AppConfig&appConfig)
{
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

  std::unique_ptr<ImageSelector> selector;
  if (appConfig.sorted)
  {
    selector = std::unique_ptr<ImageSelector>(new SortedImageSelector(pathTraverser));
  }
  else if (appConfig.shuffle)
  {
    selector = std::unique_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser));
  }
  else
  {
    selector = std::unique_ptr<ImageSelector>(new RandomImageSelector(pathTraverser));
  }

  return selector;
}

void ReloadConfigIfNeeded(AppConfig &appConfig, MainWindow &w, ImageSwitcher *switcher, ImageSelector *selector)
{  
  QString jsonFile = getAppConfigFilePath(appConfig.configPath);
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return;
  }

  if(appConfig.loadTime <  QFileInfo(jsonFile).lastModified())
  {
    const std::string oldPath = appConfig.path;
    const std::string oldImageList = appConfig.imageList;

    AppConfig oldConfig = appConfig;
    appConfig = loadAppConfiguration(appConfig);

    ConfigureWindowFromSettings(w, appConfig);
    if(appConfig.PathOptionsChanged(oldConfig))
    {
      std::unique_ptr<ImageSelector> selector = GetSelectorForConfig(appConfig);
      switcher->setImageSelector(selector);
    }

    selector->setDebugMode(appConfig.debugMode);
    switcher->setRotationTime(appConfig.rotationSeconds * 1000);
  }
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  AppConfig commandLineAppConfig;
  if (!parseCommandLine(commandLineAppConfig, argc, argv))
  {
    usage(argv[0]);
    return 1;
  }

  AppConfig appConfig = loadAppConfiguration(commandLineAppConfig);

  if (appConfig.path.empty() && appConfig.imageList.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
  }

  if(appConfig.debugMode)
  {
    std::cout << "Rotation Time: " << appConfig.rotationSeconds << std::endl;
    std::cout << "Overlay input: " << appConfig.overlay << std::endl;
  }
  
  MainWindow w;
  ConfigureWindowFromSettings(w, appConfig);
  w.show();

  std::unique_ptr<ImageSelector> selector = GetSelectorForConfig(appConfig);
  selector->setDebugMode(appConfig.debugMode);
  
  ImageSwitcher switcher(w, appConfig.rotationSeconds * 1000, selector);
  w.setImageSwitcher(&switcher);
  std::function<void(MainWindow &w, ImageSwitcher *switcher, ImageSelector *selector)> reloader = [&appConfig](MainWindow &w, ImageSwitcher *switcher, ImageSelector *selector) { ReloadConfigIfNeeded(appConfig, w, switcher, selector); };
  switcher.setConfigFileReloader(reloader);
  switcher.start();
  return a.exec();
}
