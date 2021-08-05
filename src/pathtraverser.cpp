#include "pathtraverser.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#define UNUSED(x) (void)(x)

PathTraverser::PathTraverser(const std::string path, bool debugModeIn):
  path(path), debugMode(debugModeIn)
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
  ImageDisplayOptions options = baseOptions;
  QDir directory(directoryPath.c_str());
  QString jsonFile = directory.filePath(QString("options.json"));
  if(directory.exists(jsonFile))
  {
    if(debugMode)
    {
      std::cout << "Found options file" << std::endl;
    }
    QString val;
    QFile file;
    file.setFileName(jsonFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonDoc = d.object();
    if(jsonDoc.contains("fitAspectAxisToWindow") && jsonDoc["fitAspectAxisToWindow"].isBool())
    {
      options.fitAspectAxisToWindow = jsonDoc["fitAspectAxisToWindow"].toBool();
      if(debugMode)
      {
        std::cout << "Fit Aspect:" << options.fitAspectAxisToWindow << std::endl;
      }
    }
  }
  return options;
}

RecursivePathTraverser::RecursivePathTraverser(const std::string path,bool debugMode):
  PathTraverser(path,debugMode)
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

DefaultPathTraverser::DefaultPathTraverser(const std::string path,bool debugMode):
  PathTraverser(path,debugMode),
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
  UNUSED(filename);
  return LoadOptionsForDirectory(directory.absolutePath().toStdString(), baseOptions);
}

ImageListPathTraverser::ImageListPathTraverser(const std::string &imageListString,bool debugMode):
  PathTraverser("",debugMode)
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
  UNUSED(filename);
  UNUSED(baseOptions);
  return ImageDisplayOptions();
}
