#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <memory>
#include <QStringList>

class MainWindow;
class PathTraverser;

class ImageSelector
{
public:
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~ImageSelector();
    virtual std::string getNextImage() = 0;

protected:
    std::unique_ptr<PathTraverser>& pathTraverser;
};

class RandomImageSelector : public ImageSelector
{
public:
    RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~RandomImageSelector();
    virtual std::string getNextImage();

private:
    unsigned int selectRandom(const QStringList& images) const;
};

class ShuffleImageSelector : public ImageSelector
{
public:
    ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~ShuffleImageSelector();
    virtual std::string getNextImage();

private:
    int current_image_shuffle;
    QStringList images;
};

class SortedImageSelector : public ImageSelector
{
public:
    SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~SortedImageSelector();
    virtual std::string getNextImage();

private:
    QStringList images;
};
#endif // IMAGESELECTOR_H
