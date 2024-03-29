#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <iostream>
#include <memory>
#include <QStringList>
#include <QVector>
#include "imagestructs.h"

class MainWindow;
class PathTraverser;

class ImageSelector
{
public:
    ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    ImageSelector(); // use case for when you don't own your own traverser
    virtual ~ImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions) = 0;
 
protected:
    ImageDetails populateImageDetails(const std::string&filename, const ImageDisplayOptions &baseOptions);
    bool imageValidForAspect(const ImageDetails& imageDetails);
    bool imageMatchesFilter(const ImageDetails& imageDetails);
    bool imageInsideTimeWindow(const QVector<DisplayTimeWindow> &timeWindows);
    std::unique_ptr<PathTraverser> pathTraverser;
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

class ListImageSelector : public ImageSelector
{
public:
    ListImageSelector();
    virtual ~ListImageSelector();
    virtual const ImageDetails getNextImage(const ImageDisplayOptions &baseOptions);
    void AddImageSelector(std::unique_ptr<ImageSelector>& selector, const bool exclusiveIn, const ImageDisplayOptions& baseDisplayOptionsIn);

private:
    struct SelectoryEntry {
        std::unique_ptr<ImageSelector> selector;
        ImageDisplayOptions baseDisplayOptions;
        bool exclusive = false;
    };
    std::vector<SelectoryEntry> imageSelectors;
    std::vector<SelectoryEntry>::iterator currentSelector;
};
#endif // IMAGESELECTOR_H
