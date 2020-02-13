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
    virtual std::string getNextImage() const = 0;

protected:
    std::unique_ptr<PathTraverser>& pathTraverser;
};

class DefaultImageSelector : public ImageSelector
{
public:
    DefaultImageSelector(std::unique_ptr<PathTraverser>& pathTraverser);
    virtual ~DefaultImageSelector();
    virtual std::string getNextImage() const;

private:
    unsigned int selectRandom(const QStringList& images) const;
};

#endif // IMAGESELECTOR_H
