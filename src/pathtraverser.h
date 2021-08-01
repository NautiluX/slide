#ifndef PATHTRAVERSER_H
#define PATHTRAVERSER_H

#include <iostream>
#include <QDir>
#include <QStringList>
#include "imageselector.h"

static const QStringList supportedFormats={"jpg","jpeg","png","tif","tiff"};

class MainWindow;
class PathTraverser
{
  public:
    PathTraverser(const std::string path, bool debugModeIn);
    virtual ~PathTraverser();
    virtual QStringList getImages() const = 0;
    virtual const std::string getImagePath(const std::string image) const = 0;
    virtual void UpdateOptionsForImage(const std::string& filename, ImageOptions_t& options) const = 0;

  protected:
    const std::string path;
    bool debugMode = false;
    QStringList getImageFormats() const;
    void LoadOptionsForDirectory(const std::string &directoryPath, ImageOptions_t &options) const;
};

class RecursivePathTraverser : public PathTraverser
{
  public:
    RecursivePathTraverser(const std::string path, bool debugModeIn);
    virtual ~RecursivePathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual void UpdateOptionsForImage(const std::string& filename, ImageOptions_t& options) const;
};

class DefaultPathTraverser : public PathTraverser
{
  public:
    DefaultPathTraverser(const std::string path, bool debugModeIn);
    virtual ~DefaultPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual void UpdateOptionsForImage(const std::string& filename, ImageOptions_t& options) const;
  private:
    QDir directory;
};

class ImageListPathTraverser : public PathTraverser
{
  public:
    ImageListPathTraverser(const std::string &imageListString, bool debugModeIn);
    virtual ~ImageListPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual void UpdateOptionsForImage(const std::string& filename, ImageOptions_t& options) const;
  private:
    QStringList imageList;
};
#endif // PATHTRAVERSER_H
