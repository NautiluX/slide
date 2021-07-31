#ifndef PATHTRAVERSER_H
#define PATHTRAVERSER_H

#include <iostream>
#include <QDir>
#include <QStringList>

static const QStringList supportedFormats={"jpg","jpeg","png","tif","tiff"};

class MainWindow;
class PathTraverser
{
  public:
    PathTraverser(const std::string path);
    virtual ~PathTraverser();
    virtual QStringList getImages() const = 0;
    virtual const std::string getImagePath(const std::string image) const = 0;

  protected:
    const std::string path;
    QStringList getImageFormats() const;
};

class RecursivePathTraverser : public PathTraverser
{
  public:
    RecursivePathTraverser(const std::string path);
    virtual ~RecursivePathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
};

class DefaultPathTraverser : public PathTraverser
{
  public:
    DefaultPathTraverser(const std::string path);
    virtual ~DefaultPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
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
  private:
    QStringList imageList;
};
#endif // PATHTRAVERSER_H
