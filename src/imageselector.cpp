#include "imageselector.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

ImageSelector::ImageSelector(MainWindow& w, unsigned int timeout, std::string path, bool recursive):
    QObject::QObject(),
    window(w),
    timeout(timeout),
    path(path),
    recursive(recursive),
    timer(this)
{
    srand (time(NULL));
}

void ImageSelector::updateImage()
{
    QDir directory(path.c_str());
    QStringList images;
    if (recursive)
    {
      images = listImagesRecursive();
    }
    else
    {
      images = directory.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files);
    }
    std::cout << "images: " << images.size() << std::endl;
    if (images.size() == 0)
    {
      std::cerr << "No jpg images found in folder " << path << std::endl;
      return;
    }
    unsigned int selectedImage = rand() % images.size();
    std::string filename = directory.filePath(images.at(selectedImage)).toStdString();
    std::cout << "updating image: " << filename << std::endl;
    window.setImage(filename);
}

void ImageSelector::start()
{
    updateImage();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(timeout);
}

QStringList ImageSelector::listImagesRecursive()
{
    QDirIterator it(QString(path.c_str()), QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
       files.append(it.next());
    }
    return files;
}
