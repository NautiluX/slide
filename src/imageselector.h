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
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspectIn);
    virtual ~ImageSelector();
    virtual std::string getNextImage() = 0;
    void setDebugMode(bool debugModeIn) { debugMode = debugModeIn;}

protected:
    int getImageRotation(const std::string& fileName);
    bool imageMatchesFilter(const std::string& fileName);
    bool imageValidForAspect(const std::string& fileName);
    std::unique_ptr<PathTraverser>& pathTraverser;
    char aspect;
    bool debugMode = false;
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
    void reloadImagesIfNoneLeft();
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
    void reloadImagesIfEmpty();
    QStringList images;
};
#endif // IMAGESELECTOR_H
