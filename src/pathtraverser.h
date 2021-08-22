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
    PathTraverser(const std::string path, bool debugModeIn);
    virtual ~PathTraverser();
    virtual QStringList getImages() const = 0;
    virtual const std::string getImagePath(const std::string image) const = 0;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const = 0;

  protected:
    const std::string path;
    bool debugMode = false;
    QStringList getImageFormats() const;
    ImageDisplayOptions LoadOptionsForDirectory(const std::string &directoryPath, const ImageDisplayOptions &baseOptions) const;
};

class RecursivePathTraverser : public PathTraverser
{
  public:
    RecursivePathTraverser(const std::string path, bool debugModeIn);
    virtual ~RecursivePathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const;
};

class DefaultPathTraverser : public PathTraverser
{
  public:
    DefaultPathTraverser(const std::string path, bool debugModeIn);
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
    ImageListPathTraverser(const std::string &imageListString, bool debugModeIn);
    virtual ~ImageListPathTraverser();
    QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& options) const;
  private:
    QStringList imageList;
};

class RedditRSSFeedPathTraverser: public QObject, public PathTraverser
{
  Q_OBJECT 
 public:
    RedditRSSFeedPathTraverser(const std::string& rSSFeedURL,QNetworkAccessManager& networkManager, bool debugModeIn);
    virtual ~RedditRSSFeedPathTraverser();

    virtual QStringList getImages() const;
    virtual const std::string getImagePath(const std::string image) const;
    virtual ImageDisplayOptions UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const;

  private slots:
    void fileDownloaded(QNetworkReply* pReply);
  private:
    void RequestRSSFeed();
    std::string rssFeedURL;
    QStringList imageURLS;
    QNetworkAccessManager& webCtrl; 
    QNetworkReply *pendingReply = nullptr;
    QDateTime rssRequestedTime;
};

#endif // PATHTRAVERSER_H
