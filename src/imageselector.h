#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QObject>
#include <QTimer>
#include <iostream>
class MainWindow;
class ImageSelector : public QObject
{
    Q_OBJECT
public:
    ImageSelector(MainWindow& w, unsigned int timeout, std::string path, bool recursive);
    void start();

public slots:
    void updateImage();
private:
    QStringList listImagesRecursive();
    MainWindow& window;
    unsigned int timeout;
    std::string path;
    bool recursive;
    QTimer timer;
};

#endif // IMAGESELECTOR_H
