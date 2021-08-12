#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <memory>
#include <QStringList>
#include "imagestructs.h"

class MainWindow;
class PathTraverser;

class ImageSelector
{
public:
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~ImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions) = 0;
    void setDebugMode(bool debugModeIn);

protected:
    ImageDetails populateImageDetails(const std::string&filename, const ImageDisplayOptions &baseOptions);
    bool imageValidForAspect(const ImageDetails& imageDetails);
    bool imageMatchesFilter(const ImageDetails& imageDetails);
    bool imageInsideTimeWindow(const QVector<DisplayTimeWindow> &timeWindows);
    std::unique_ptr<PathTraverser> pathTraverser;
    bool debugMode = false;
};

class RandomImageSelector : public ImageSelector
{
public:
    RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~RandomImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions);

private:
    unsigned int selectRandom(const QStringList& images) const;
};

class ShuffleImageSelector : public ImageSelector
{
public:
    ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~ShuffleImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions);

private:
    void reloadImagesIfNoneLeft();
    int current_image_shuffle;
    QStringList images;
};

class SortedImageSelector : public ImageSelector
{
public:
    SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~SortedImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions);

private:
    void reloadImagesIfEmpty();
    QStringList images;
};
#endif // IMAGESELECTOR_H
