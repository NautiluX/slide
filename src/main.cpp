#include "mainwindow.h"
#include "imageselector.h"
#include "imageswitcher.h"
#include "pathtraverser.h"
#include <QApplication>
#include <iostream>
#include <sys/file.h>
#include <errno.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>

void usage(std::string programName) {
    std::cerr << "Usage: " << programName << " [-t rotation_seconds] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder [-r] [-s]" << std::endl;
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
  while ((opt = getopt(argc, argv, "b:p:t:o:rs")) != -1) {
    switch (opt) {
      case 'p':
        path = optarg;
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
      default: /* '?' */
        usage(argv[0]);
        return 1;
    }
  }

  if (path.empty())
  {
    std::cout << "Error: Path expected." << std::endl;
    usage(argv[0]);
    return 1;
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
  if (shuffle)
  {
    selector = std::unique_ptr<ImageSelector>(new ShuffleImageSelector(pathTraverser));
  }
  else
  {
    selector = std::unique_ptr<ImageSelector>(new RandomImageSelector(pathTraverser));
  }

  w.show();

  ImageSwitcher switcher(w, rotationSeconds * 1000, selector);
  switcher.start();
  return a.exec();
}
