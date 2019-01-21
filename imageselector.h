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
    ImageSelector(MainWindow& w, unsigned int timeout, std::string path);
    void start();

public slots:
    void updateImage();
private:
    MainWindow& window;
    unsigned int timeout;
    std::string path;
    QTimer timer;
};

#endif // IMAGESELECTOR_H
