#ifndef PATHTRAVERSER_H
#define PATHTRAVERSER_H

#include <iostream>
#include <QDir>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "imageselector.h"

static const QStringList supportedFormats={"jpg","jpeg","png","tif","tiff"};

class MainWindow;
class PathTraverser
{
  public:
    PathTraverser(const std::string path);
    virtual ~PathTraverser();
    virtual QStringList getImages() const = 0;
    virtual const std::string getImagePath(const std::string image) const = 0;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const = 0;

  protected:
    const std::string path;
    QStringList getImageFormats() const;
    ImageDisplayOptions LoadOptionsForDirectory(const std::string &directoryPath, const ImageDisplayOptions &baseOptions) const;
};

class RecursivePathTraverser : public PathTraverser
{
  public:
    RecursivePathTraverser(const std::string path);
    virtual ~RecursivePathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const;
};

class DefaultPathTraverser : public PathTraverser
{
  public:
    DefaultPathTraverser(const std::string path);
    virtual ~DefaultPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const;
  private:
    QDir directory;
};

class ImageListPathTraverser : public PathTraverser
{
  public:
    ImageListPathTraverser(const std::string &imageListString);
    virtual ~ImageListPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& options) const;
  private:
    QStringList imageList;
};
#endif // PATHTRAVERSER_H
