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

ImageSwitcher::ImageSwitcher(MainWindow& w, unsigned int timeoutMsec, std::unique_ptr<ImageSelector>& selector):
    QObject::QObject(),
    window(w),
    timeout(timeoutMsec),
    selector(std::move(selector)),
    timer(this),
    timerNoContent(this)
{
}

void ImageSwitcher::updateImage()
{
    if(reloadConfigIfNeeded)
    {
      reloadConfigIfNeeded(window, this);
    }
    ImageDetails imageDetails = selector->getNextImage(window.getBaseOptions());
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

void ImageSwitcher::scheduleImageUpdate()
{
  // update our image in 100msec, to let the system settle
  QTimer::singleShot(100, this, SLOT(updateImage())); 
}

void ImageSwitcher::setConfigFileReloader(std::function<void(MainWindow &w, ImageSwitcher *switcher)> reloadConfigIfNeededIn)
{
  reloadConfigIfNeeded = reloadConfigIfNeededIn;
}

void ImageSwitcher::setRotationTime(unsigned int timeoutMsecIn)
{
  timeout = timeoutMsecIn;
  timer.start(timeout);
}

void ImageSwitcher::setImageSelector(std::unique_ptr<ImageSelector>& selectorIn)
{
  selector = std::move(selectorIn);
}
