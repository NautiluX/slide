#include "imageswitcher.h"
#include "imageselector.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <iostream>
#include <memory>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

ImageSwitcher::ImageSwitcher(MainWindow& w, unsigned int timeout, std::unique_ptr<ImageSelector>& selector):
    QObject::QObject(),
    window(w),
    timeout(timeout),
    selector(selector),
    timer(this)
{
}

void ImageSwitcher::updateImage()
{
    std::string filename(selector->getNextImage());
    if (filename == "")
    {
      window.warn("No image found.");
    }
    else
    {
      window.setImage(filename);
    }
}

void ImageSwitcher::start()
{
    updateImage();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(timeout);
}
