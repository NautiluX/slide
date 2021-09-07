#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include "overlay.h"
#include "appconfig.h"
#include "logger.h"

#include <QApplication>
#include <QNetworkAccessManager>
#include <QRegularExpression>
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
    {"verbose",       no_argument,       &debugInt,   1},
    {"stretch",       no_argument,       &stretchInt, 1},
    {"overlay-color", required_argument, 0,           'h'},
  };
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "b:p:t:o:O:a:i:c:h:rsSv", long_options, &option_index)) != -1) {
    switch (opt) {
      case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          return false;
          break;
      case 'p':
        if(appConfig.paths.count() == 0)
          appConfig.paths.append(PathEntry());
        appConfig.paths[0].path = optarg;
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
        if(appConfig.paths.count() == 0)
          appConfig.paths.append(PathEntry());
        appConfig.paths[0].recursive = true;
        break;
      case 's':
        if(appConfig.paths.count() == 0)
          appConfig.paths.append(PathEntry());
        appConfig.paths[0].shuffle = true;
        std::cout << "Shuffle mode is on." << std::endl;
        break;
      case 'S':
        if(appConfig.paths.count() == 0)
          appConfig.paths.append(PathEntry());
        appConfig.paths[0].sorted = true;
        break;
      case 'O':
        appConfig.overlay = optarg;
        break;
      case 'h':
        appConfig.overlayHexRGB = QString::fromStdString(optarg);
        break;
      case 'v':
        appConfig.debugMode = true;
        break;
      case 'i':
        if(appConfig.paths.count() == 0)
          appConfig.paths.append(PathEntry());
        appConfig.paths[0].imageList = optarg;
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

  if (!appConfig.overlayHexRGB.isEmpty())
  {
    QRegularExpression hexRGBMatcher("^#([0-9A-Fa-f]{3}){1,2}$");
    if(!hexRGBMatcher.match(appConfig.overlayHexRGB).hasMatch())
    {
      std::cout << "Error: hex rgb string expected. e.g. #FFFFFF or #FFF" << std::endl;
    }
    else
    {
      w.setOverlayHexRGB(appConfig.overlayHexRGB);
    }
  }

  if (!appConfig.overlay.empty()) 
  {
    std::unique_ptr<Overlay> o = std::unique_ptr<Overlay>(new Overlay(appConfig.overlay));
    w.setOverlay(o);
  }
  w.setBaseOptions(appConfig.baseDisplayOptions);
}

std::unique_ptr<ImageSelector> GetSelectorForConfig(const PathEntry& path)
{
  std::unique_ptr<PathTraverser> pathTraverser;
  if (!path.imageList.empty())
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new ImageListPathTraverser(path.imageList));
  }
  else if (path.recursive)
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new RecursivePathTraverser(path.path));
  }
  else
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new DefaultPathTraverser(path.path));
  }

  std::unique_ptr<ImageSelector> selector;
  if (path.sorted)
  {
    selector = std::unique_ptr<ImageSelector>(new SortedImageSelector(pathTraverser));
  }
  else if (path.shuffle)
  {
    selector = std::unique_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser));
  }
  else
  {
    selector = std::unique_ptr<ImageSelector>(new RandomImageSelector(pathTraverser));
  }

  return selector;
}

std::unique_ptr<ImageSelector> GetSelectorForApp(const AppConfig& appConfig)
{
  if(appConfig.paths.count()==1)
  {
    return GetSelectorForConfig(appConfig.paths[0]);
  }
  else
  {
    std::unique_ptr<ListImageSelector> listSelector(new ListImageSelector());
    for(const auto &path : appConfig.paths)
    {
      auto selector = GetSelectorForConfig(path);
      listSelector->AddImageSelector(selector, path.exclusive, path.baseDisplayOptions);
    }
    // new things
    return listSelector;
  }
}


void ReloadConfigIfNeeded(AppConfig &appConfig, MainWindow &w, ImageSwitcher *switcher)
{  
  QString jsonFile = getAppConfigFilePath(appConfig.configPath);
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return;
  }

  if(appConfig.loadTime <  QFileInfo(jsonFile).lastModified())
  {
    AppConfig oldConfig = appConfig;
    appConfig = loadAppConfiguration(appConfig);

    ConfigureWindowFromSettings(w, appConfig);
    if(appConfig.PathOptionsChanged(oldConfig))
    {
      std::unique_ptr<ImageSelector> selector = GetSelectorForApp(appConfig);
      switcher->setImageSelector(selector);
    }

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

  if (appConfig.paths.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
  }
  SetupLogger(appConfig.debugMode);
  Log( "Rotation Time: ", appConfig.rotationSeconds );
  Log( "Overlay input: ", appConfig.overlay );
  
  QNetworkAccessManager webCtrl; 

  MainWindow w;
  ConfigureWindowFromSettings(w, appConfig);
  w.setNetworkManager(&webCtrl);
  w.show();

  std::unique_ptr<ImageSelector> selector = GetSelectorForApp(appConfig);
  
  ImageSwitcher switcher(w, appConfig.rotationSeconds * 1000, selector);
  w.setImageSwitcher(&switcher);
  std::function<void(MainWindow &w, ImageSwitcher *switcher)> reloader = [&appConfig](MainWindow &w, ImageSwitcher *switcher) { ReloadConfigIfNeeded(appConfig, w, switcher); };
  switcher.setConfigFileReloader(reloader);
  switcher.start();
  return a.exec();
}
