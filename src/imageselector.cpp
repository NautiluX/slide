#include "imageselector.h"
#include "mainwindow.h"
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

ImageSelector::ImageSelector(MainWindow& w, unsigned int timeout, std::string path):
    QObject::QObject(),
    window(w),
    timeout(timeout),
    path(path),
    timer(this)
{
    srand (time(NULL));
}

void ImageSelector::updateImage()
{
    QDir directory(path.c_str());
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    unsigned int selectedImage = rand() % images.size();
    std::string filename = directory.filePath(images.at(selectedImage)).toStdString();
    std::cout << "updating image: " << filename << std::endl;
    window.setImage(filename);
}

void ImageSelector::start(){
    updateImage();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(timeout);
}
