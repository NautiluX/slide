#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <QStringList>

class MainWindow;
class ImageSelector
{
public:
    ImageSelector(std::string path, bool recursive);
    std::string getNextImage() const;

private:
    QStringList listImagesRecursive() const;
    std::string path;
    bool recursive;
};

#endif // IMAGESELECTOR_H
