#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include "overlay.h"
#include <QApplication>
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
  char aspect = 'a';
  bool fitAspectAxisToWindow = false;
  std::string valid_aspects = "alp"; // all, landscape, portait
  std::string overlay = "";
  QString overlayHexRGB = QString();
  QRegularExpression hexRGBMatcher("^#([0-9A-Fa-f]{3}){1,2}$");
  int debugInt = 0;
  int stretchInt = 0;
  static struct option long_options[] =
  {
    {"verbose",       no_argument,       &debugInt,   1},
    {"stretch",       no_argument,       &stretchInt, 1},
    {"overlay-color", required_argument, 0,           'c'},
  };
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "b:p:t:o:O:c:a:rsSv", long_options, &option_index)) != -1) {
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
        aspect = optarg[0];
        if ( valid_aspects.find(aspect) == std::string::npos )
        {
          std::cout << "Invalid Aspect option, defaulting to all" << std::endl;
          aspect = 'a';
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
      case 'c':
        overlayHexRGB = QString::fromStdString(optarg);
        break;
      case 'v':
        debugMode = true;
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
    fitAspectAxisToWindow = true;
  }

  if (path.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
  }

  if (!overlayHexRGB.isEmpty())
  {
    if(!hexRGBMatcher.match(overlayHexRGB).hasMatch())
    {
      std::cout << "Error: hex rgb string expected. e.g. #FFFFFF or #FFF" << std::endl;
      return 1;
    }
    w.setOverlayHexRGB(overlayHexRGB);
  }

  std::unique_ptr<PathTraverser> pathTraverser;
  if (recursive)
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new RecursivePathTraverser(path));
  }
  else
  {
    pathTraverser = std::unique_ptr<PathTraverser>(new DefaultPathTraverser(path));
  }

  std::unique_ptr<ImageSelector> selector;
  if (sorted)
  {
    selector = std::unique_ptr<ImageSelector>(new SortedImageSelector(pathTraverser, aspect));
  }
  else if (shuffle)
  {
    selector = std::unique_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser, aspect));
  }
  else
  {
    selector = std::unique_ptr<ImageSelector>(new RandomImageSelector(pathTraverser, aspect));
  }
  selector->setDebugMode(debugMode);
  if(debugMode)
  {
    std::cout << "Rotation Time: " << rotationSeconds << std::endl;
    std::cout << "Overlay input: " << overlay << std::endl;
  }

  Overlay o(overlay);
  o.setDebugMode(debugMode);
  if (!overlay.empty())
  {
    w.setOverlay(&o);
  }

  w.setAspect(aspect);
  w.setDebugMode(debugMode);
  w.setFitAspectAxisToWindow(fitAspectAxisToWindow);
  w.show();

  ImageSwitcher switcher(w, rotationSeconds * 1000, selector);
  switcher.start();
  return a.exec();
}
