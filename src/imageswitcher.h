#ifndef IMAGESWITCHER_H
#define IMAGESWITCHER_H

#include <QObject>
#include <QTimer>
#include <iostream>
#include <memory>
#include <functional>
#include "imageselector.h"

class MainWindow;
class ImageSwitcher : public QObject
{
    Q_OBJECT
public:
    ImageSwitcher(MainWindow& w, unsigned int timeoutMsec, std::unique_ptr<ImageSelector>& selector);
    void start();
    void scheduleImageUpdate();
    void setConfigFileReloader(std::function<void(MainWindow &w, ImageSwitcher *switcher, ImageSelector *selector)> reloadConfigIfNeededIn);
    void setRotationTime(unsigned int timeoutMsec);
    void setImageSelector(std::unique_ptr<ImageSelector>& selector);

public slots:
    void updateImage();
private:
    MainWindow& window;
    unsigned int timeout;
    std::unique_ptr<ImageSelector> selector;
    QTimer timer;
    const unsigned int timeoutNoContent = 5 * 1000; // 5 sec
    QTimer timerNoContent;
    std::function<void(MainWindow &w, ImageSwitcher *switcher, ImageSelector *selector)> reloadConfigIfNeeded;
};

#endif // IMAGESWITCHER_H
