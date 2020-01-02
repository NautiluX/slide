#ifndef IMAGESWITCHER_H
#define IMAGESWITCHER_H

#include <QObject>
#include <QTimer>
#include <iostream>

class MainWindow;
class ImageSelector;
class ImageSwitcher : public QObject
{
    Q_OBJECT
public:
    ImageSwitcher(MainWindow& w, unsigned int timeout, const ImageSelector& selector);
    void start();

public slots:
    void updateImage();
private:
    MainWindow& window;
    unsigned int timeout;
    const ImageSelector& selector;
    QTimer timer;
};

#endif // IMAGESWITCHER_H
