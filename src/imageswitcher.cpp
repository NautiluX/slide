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
#include <chrono>
#include <thread>

ImageSwitcher::ImageSwitcher(MainWindow& w, unsigned int timeout, std::unique_ptr<ImageSelector>& selector):
    QObject::QObject(),
    window(w),
    timeout(timeout),
    selector(selector),
    timer(this)
{
    watcher = new QFutureWatcher<void>;
    connect(this, SIGNAL(imageUpdated()), this, SLOT(getNextImageThread()), Qt::QueuedConnection);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));

    getNextImage();
    updateImage();
}

void ImageSwitcher::getNextImage()
{
    nextImageName = selector->getNextImage();

    while (nextImageName.empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        nextImageName = selector->getNextImage();
    }
    nextImage = QPixmap(nextImageName.c_str());
}

void ImageSwitcher::getNextImageThread()
{
    watcher->setFuture(QtConcurrent::run(this, &ImageSwitcher::getNextImage));
}


void ImageSwitcher::updateImage()
{
    while (watcher->isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    window.setImage(nextImageName, nextImage);
    emit imageUpdated();
}

void ImageSwitcher::start()
{
    timer.start(timeout);
}
