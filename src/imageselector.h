#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <memory>
#include <QStringList>

class MainWindow;
class PathTraverser;

struct ImageOptions_t
{
    char aspect;
    bool fitAspectAxisToWindow;
    int rotation;
};

class ImageSelector
{
public:
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspectIn, bool fitAspectAxisToWindow);
    virtual ~ImageSelector();
    virtual const std::string getNextImage(ImageOptions_t &options) = 0;
    void setDebugMode(bool debugModeIn) { debugMode = debugModeIn;}

protected:
    int getImageRotation(const std::string &fileName);
    bool imageValidForAspect(const std::string &fileName, const int rotation);
    std::unique_ptr<PathTraverser>& pathTraverser;
    char aspect;
    bool fitAspectAxisToWindow = false;
    bool debugMode = false;
};

class RandomImageSelector : public ImageSelector
{
public:
    RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect, bool fitAspectAxisToWindow);
    virtual ~RandomImageSelector();
    virtual const std::string getNextImage(ImageOptions_t &options);

private:
    unsigned int selectRandom(const QStringList& images) const;
};

class ShuffleImageSelector : public ImageSelector
{
public:
    ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect, bool fitAspectAxisToWindow);
    virtual ~ShuffleImageSelector();
    virtual const std::string getNextImage(ImageOptions_t &options);

private:
    int current_image_shuffle;
    QStringList images;
};

class SortedImageSelector : public ImageSelector
{
public:
    SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect, bool fitAspectAxisToWindow);
    virtual ~SortedImageSelector();
    virtual const std::string getNextImage(ImageOptions_t &options);

private:
    QStringList images;
};
#endif // IMAGESELECTOR_H
