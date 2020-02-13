#include "imageselector.h"
#include "pathtraverser.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


ImageSelector::ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
    pathTraverser(pathTraverser)
{
}

ImageSelector::~ImageSelector(){}

DefaultImageSelector::DefaultImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
    ImageSelector(pathTraverser)
{
    srand (time(NULL));
}

DefaultImageSelector::~DefaultImageSelector(){}

std::string DefaultImageSelector::getNextImage() const
{
    std:: string filename;
    try
    {
      QStringList images = pathTraverser->getImages();
      unsigned int selectedImage = selectRandom(images);
      filename = pathTraverser->getImagePath(images.at(selectedImage).toStdString());
    }
    catch(const std::string& err) 
    {
      std::cerr << "Error: " << err << std::endl;
    }
    std::cout << "updating image: " << filename << std::endl;
    return filename;
}

unsigned int DefaultImageSelector::selectRandom(const QStringList& images) const
{
    std::cout << "images: " << images.size() << std::endl;
    if (images.size() == 0)
    {
      throw std::string("No jpg images found in given folder");
    }
    return rand() % images.size();
}
