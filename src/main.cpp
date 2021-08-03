#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include "overlay.h"
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
    std::cerr << "Usage: " << programName << " [-t rotation_seconds] [-a aspect('l','p','a')] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder [-r] [-s] [-v] [--verbose] [--stretch]" << std::endl;
}

int main(int argc, char *argv[])
{
  unsigned int rotationSeconds = 30;
  std::string path = "";

  QApplication a(argc, argv);

  MainWindow w;
  int opt;
  bool recursive = false;
  bool shuffle = false;
  bool sorted = false;
  bool debugMode = false;
  ImageDisplayOptions_t baseDisplayOptions;
  std::string valid_aspects = "alp"; // all, landscape, portait
  std::string overlay = "";
  std::string imageList = ""; // comma delimited list of images to show
  int debugInt = 0;
  int stretchInt = 0;
  static struct option long_options[] =
  {
    {"verbose", no_argument,       &debugInt, 1},
    {"stretch", no_argument,       &stretchInt, 1},
  };
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "b:p:t:o:O:a:i:rsSv", long_options, &option_index)) != -1) {
    switch (opt) {
      case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          usage(argv[0]);
          return 1;
          break;
      case 'p':
        path = optarg;
        break;
      case 'a':
        if ( valid_aspects.find(optarg[0]) == std::string::npos )
        {
          std::cout << "Invalid Aspect option, defaulting to all" << std::endl;
          baseDisplayOptions.onlyAspect = EImageAspect_Any;
        }
        else
        {
          switch(optarg[0])
          {
            case 'l':
              baseDisplayOptions.onlyAspect = EImageAspect_Landscape;
              break;
            case 'p':
              baseDisplayOptions.onlyAspect = EImageAspect_Portrait;
              break;
            default:
            case 'a':
              baseDisplayOptions.onlyAspect = EImageAspect_Any;
              break;
          }
        }
        break;
      case 't':
        rotationSeconds = atoi(optarg);
        break;
      case 'b':
        w.setBlurRadius(atoi(optarg));
        break;
      case 'o':
        w.setBackgroundOpacity(atoi(optarg));
        break;
      case 'r':
        recursive = true;
        break;
      case 's':
        shuffle = true;
        std::cout << "Shuffle mode is on." << std::endl;
        break;
      case 'S':
        sorted = true;
        break;
      case 'O':
        overlay = optarg;
        break;
      case 'v':
        debugMode = true;
        break;
      case 'i':
        imageList = optarg;
        break;
      default: /* '?' */
        usage(argv[0]);
        return 1;
    }
  }
  if(debugInt==1)
  {
    debugMode = true;
  }
  if(stretchInt==1)
  {
    baseDisplayOptions.fitAspectAxisToWindow = true;
  }

  if (path.empty() && imageList.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
  }

  std::unique_ptr<PathTraverser> pathTraverser;
  if (!imageList.empty())
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new ImageListPathTraverser(imageList, debugMode));
  }
  else if (recursive)
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new RecursivePathTraverser(path, debugMode));
  }
  else
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new DefaultPathTraverser(path, debugMode));
  }

  std::unique_ptr<ImageSelector> selector;
  if (sorted)
  {
    selector = std::unique_ptr<ImageSelector>(new SortedImageSelector(pathTraverser));
  }
  else if (shuffle)
  {
    selector = std::unique_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser));
  }
  else
  {
    selector = std::unique_ptr<ImageSelector>(new RandomImageSelector(pathTraverser));
  }
  selector->setDebugMode(debugMode);
  if(debugMode)
  {
    std::cout << "Rotation Time: " << rotationSeconds << std::endl;
    std::cout << "Overlay input: " << overlay << std::endl;
  }
  Overlay o(overlay);
  o.setDebugMode(debugMode);
  w.setOverlay(&o);
  w.setDebugMode(debugMode);
  w.setBaseOptions(baseDisplayOptions);
  w.show();

  ImageSwitcher switcher(w, rotationSeconds * 1000, selector);
  switcher.start();
  return a.exec();
}
