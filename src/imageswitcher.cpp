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
    timer(this),
    timerNoContent(this)
{
}

void ImageSwitcher::updateImage()
{
    ImageDetails_t imageDetails = selector->getNextImage(window.getBaseOptions());
    if (imageDetails.filename == "")
    {
      window.warn("No image found.");
      timerNoContent.start(timeoutNoContent);
    }
    else
    {
      window.setImage(imageDetails);
      timerNoContent.stop(); // we have loaded content so stop the fast polling
    }
}

void ImageSwitcher::start()
{
    updateImage();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(&timerNoContent, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(timeout);
}
