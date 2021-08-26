#include "pathtraverser.h"
#include "mainwindow.h"
#include "appconfig.h"
#include "logger.h"

#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomAttr>
#include <iostream>
#include <stdlib.h>     /* srand, rand */


PathTraverser::PathTraverser(const std::string path):
  path(path)
{}

PathTraverser::~PathTraverser() {}

QStringList PathTraverser::getImageFormats() const {
  QStringList imageFormats;
  for ( const QString& s : supportedFormats )
      imageFormats<<"*."+s<<"*."+s.toUpper();
  return imageFormats;
}

ImageDisplayOptions PathTraverser::LoadOptionsForDirectory(const std::string &directoryPath, const ImageDisplayOptions &baseOptions) const
{
  Config baseConfig;
  baseConfig.baseDisplayOptions = baseOptions;
  return getConfigurationForFolder(directoryPath, baseConfig).baseDisplayOptions;
}

RecursivePathTraverser::RecursivePathTraverser(const std::string path):
  PathTraverser(path)
{}

RecursivePathTraverser::~RecursivePathTraverser() {}


QStringList RecursivePathTraverser::getImages() const
{
    QDirIterator it(QString(path.c_str()), getImageFormats(),
                    QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
       files.append(it.next());
    }
    return files;
}

const std::string RecursivePathTraverser::getImagePath(const std::string image) const
{
  return image;
}

ImageDisplayOptions RecursivePathTraverser::UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const
{
  QDir d = QFileInfo(filename.c_str()).absoluteDir();
  return LoadOptionsForDirectory(d.absolutePath().toStdString(), baseOptions);
}

DefaultPathTraverser::DefaultPathTraverser(const std::string path):
  PathTraverser(path),
  directory(path.c_str())
{}

DefaultPathTraverser::~DefaultPathTraverser() {}


QStringList DefaultPathTraverser::getImages() const
{
  return directory.entryList(getImageFormats(), QDir::Files);
}

const std::string DefaultPathTraverser::getImagePath(const std::string image) const
{
  return directory.filePath(QString(image.c_str())).toStdString();
}

ImageDisplayOptions DefaultPathTraverser::UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const
{
  Q_UNUSED(filename);
  return LoadOptionsForDirectory(directory.absolutePath().toStdString(), baseOptions);
}

ImageListPathTraverser::ImageListPathTraverser(const std::string &imageListString):
  PathTraverser("")
{
  QString str = QString(imageListString.c_str());
  imageList = str.split(QLatin1Char(','));
}

ImageListPathTraverser::~ImageListPathTraverser() {}


QStringList ImageListPathTraverser::getImages() const
{
  return imageList;
}

const std::string ImageListPathTraverser::getImagePath(const std::string image) const
{
  return image;
}

ImageDisplayOptions ImageListPathTraverser::UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const
{
  // no per file options modification supported
  Q_UNUSED(filename);
  Q_UNUSED(baseOptions);
  return baseOptions;
}


RedditRSSFeedPathTraverser::RedditRSSFeedPathTraverser(const std::string& rssFeedURLIn, QNetworkAccessManager& networkManager) :
  PathTraverser(""), rssFeedURL(rssFeedURLIn), webCtrl(networkManager)
{
  connect( &webCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
  RequestRSSFeed();
}

RedditRSSFeedPathTraverser::~RedditRSSFeedPathTraverser()
{
}

void RedditRSSFeedPathTraverser::RequestRSSFeed()
{
  if (pendingReply)
  {
    pendingReply->abort();
  }
  Log("Requesting RSS feed:", rssFeedURL);
  rssRequestedTime = QDateTime::currentDateTime();
  QNetworkRequest request(QUrl(rssFeedURL.c_str()));
  pendingReply = webCtrl.get(request);
}

void RedditRSSFeedPathTraverser::fileDownloaded(QNetworkReply* netReply) 
{
  if (netReply != pendingReply)
    return;

  pendingReply = nullptr;

  QNetworkReply::NetworkError err = netReply->error();
  if (err != QNetworkReply::NoError)
  {
    std::cout << "Failed to load Reddit RSS URL: " << err << std::endl;
    return;
  }

  QString str (netReply->readAll());
  QVariant vt = netReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  netReply->deleteLater();
  if (!vt.isNull())
  {
    Log("Redirected to:", vt.toUrl().toString().toStdString());
    webCtrl.get(QNetworkRequest(vt.toUrl()));
  }
  else
  {
    QDomDocument doc;
    QString error;
    if (!doc.setContent(str, false, &error))
    {
      Log("Failed to load page:", error.toStdString());
    }
    else
    {
      QDomElement docElem = doc.documentElement();
      QDomNodeList nodeList = docElem.elementsByTagName("entry"); 
      for (int iEntry = 0; iEntry < nodeList.length(); ++iEntry)
      {
        QDomNode node = nodeList.item(iEntry);
        QDomElement e = node.toElement();
        QDomNode contentNode =  e.elementsByTagName("content").item(0).firstChild();
        QDomDocument docContent;
        if (!docContent.setContent(contentNode.nodeValue(), false, &error))
        {
          continue;
        }

        QDomNodeList addressEntries = docContent.documentElement().elementsByTagName("a");
        for (int iAddr = 0; iAddr < addressEntries.length(); ++iAddr)
        {
          QDomNode node = addressEntries.item(iAddr);
          /*QString output;
          QTextStream stream(&output);
          node.save(stream, 0);
          qDebug() << "nodeValue: " << output;*/
          if (node.toElement().text() == "[link]" && node.hasAttributes() )
          {
            QDomAttr a = node.toElement().attributeNode("href");
            // check if the URL matches one of our supported formats
            for ( const QString& format : supportedFormats ) 
            {
              if (a.value().endsWith(format))
              {
                imageURLS.append(a.value());
              }
            }
          }
        }
      }
    }
  }
}


QStringList RedditRSSFeedPathTraverser::getImages() const
{
  // refresh the feed after 5 hours
  if (rssRequestedTime.secsTo(QDateTime::currentDateTime()) > 60*60*5 )
  {
    const_cast<RedditRSSFeedPathTraverser *>(this)->RequestRSSFeed(); 
  }
  return imageURLS;
}

const std::string RedditRSSFeedPathTraverser::getImagePath(const std::string image) const
{
  return image;
}

ImageDisplayOptions RedditRSSFeedPathTraverser::UpdateOptionsForImage(const std::string& filename, const ImageDisplayOptions& baseOptions) const
{
  // no per file options modification supported
  Q_UNUSED(filename);
  return baseOptions;
}
