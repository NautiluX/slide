#ifndef IMAGESWITCHER_H
#define IMAGESWITCHER_H

#include <QObject>
#include <QTimer>
#include <iostream>
#include <memory>
#include <functional>

class MainWindow;
class ImageSelector;
class ImageSwitcher : public QObject
{
    Q_OBJECT
public:
    ImageSwitcher(MainWindow& w, unsigned int timeoutMsec, std::shared_ptr<ImageSelector>& selector);
    void start();
    void scheduleImageUpdate();
    void setConfigFileReloader(std::function<void()> reloadConfigIfNeededIn);
    void setRotationTime(unsigned int timeoutMsec);

public slots:
    void updateImage();
private:
    MainWindow& window;
    unsigned int timeout;
    std::shared_ptr<ImageSelector>& selector;
    QTimer timer;
    const unsigned int timeoutNoContent = 5 * 1000; // 5 sec
    QTimer timerNoContent;
    std::function<void()> reloadConfigIfNeeded;
};

#endif // IMAGESWITCHER_H
