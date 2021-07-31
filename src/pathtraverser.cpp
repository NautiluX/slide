#include "pathtraverser.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
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