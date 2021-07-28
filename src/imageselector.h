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
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect);
    virtual ~ImageSelector();
    virtual std::string getNextImage() = 0;

protected:
    int getImageRotation(const std::string &fileName);
    bool imageValidForAspect(const std::string &fileName);
    std::unique_ptr<PathTraverser>& pathTraverser;
    char _aspect;
};

class RandomImageSelector : public ImageSelector
{
public:
    RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect);
    virtual ~RandomImageSelector();
    virtual std::string getNextImage();

private:
    unsigned int selectRandom(const QStringList& images) const;
};

class ShuffleImageSelector : public ImageSelector
{
public:
    ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect);
    virtual ~ShuffleImageSelector();
    virtual std::string getNextImage();

private:
    int current_image_shuffle;
    QStringList images;
};

class SortedImageSelector : public ImageSelector
{
public:
    SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect);
    virtual ~SortedImageSelector();
    virtual std::string getNextImage();

private:
    QStringList images;
};
#endif // IMAGESELECTOR_H
