#ifndef IMAGESWITCHER_H
#define IMAGESWITCHER_H

#include <QObject>
#include <QTimer>
#include <iostream>
#include <memory>
#include <QPixmap>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

class MainWindow;
class ImageSelector;
class ImageSwitcher : public QObject
{
    Q_OBJECT
public:
    ImageSwitcher(MainWindow& w, unsigned int timeout, std::unique_ptr<ImageSelector>& selector);
    void start();

public slots:
    void updateImage();
    void getNextImage();
    void getNextImageThread();
signals:
    void imageUpdated();
private:
    MainWindow& window;
    unsigned int timeout;
    std::unique_ptr<ImageSelector>& selector;
    QTimer timer;
    const unsigned int timeoutNoContent = 5 * 1000; // 5 sec
    std::string nextImageName;
    QPixmap nextImage;
    QFutureWatcher<void> *watcher;
};

#endif // IMAGESWITCHER_H
