#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <QStringList>

class MainWindow;
class ImageSelector
{
public:
    ImageSelector(std::string path, bool recursive, bool shuffle);
    std::string getNextImage() const;

private:
    QStringList listImagesRecursive() const;
    unsigned int selectRandom(const QStringList& images) const;
    std::string path;
    bool recursive;
    bool shuffle;
};

#endif // IMAGESELECTOR_H
