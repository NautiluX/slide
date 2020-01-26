#include "imageselector.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

ImageSelector::ImageSelector(std::string path, bool recursive):
    path(path),
    recursive(recursive)
{
    srand (time(NULL));
}

std::string ImageSelector::getNextImage() const
{
    QDir directory(path.c_str());
    std:: string filename;
    try
    {
      if (recursive)
      {
        QStringList images = listImagesRecursive();
        unsigned int selectedImage = selectRandom(images);
        filename = images.at(selectedImage).toStdString();
      }
      else
      {
        QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files);
        unsigned int selectedImage = selectRandom(images);
        filename = directory.filePath(images.at(selectedImage)).toStdString();
      }
    }
    catch(const std::string& err) 
    {
      std::cerr << "Error: " << err << std::endl;
    }
    std::cout << "updating image: " << filename << std::endl;
    return filename;
}

unsigned int ImageSelector::selectRandom(const QStringList& images) const
{
    std::cout << "images: " << images.size() << std::endl;
    if (images.size() == 0)
    {
      throw std::string("No jpg images found in folder " + path);
    }
    return rand() % images.size();
}


QStringList ImageSelector::listImagesRecursive() const
{
    QDirIterator it(QString(path.c_str()), QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
       files.append(it.next());
    }
    return files;
}
