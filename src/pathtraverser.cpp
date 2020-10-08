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

RecursivePathTraverser::RecursivePathTraverser(const std::string path):
  PathTraverser(path)
{}

RecursivePathTraverser::~RecursivePathTraverser() {}


QStringList RecursivePathTraverser::getImages() const
{
    QDirIterator it(QString(path.c_str()), QStringList() << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG", QDir::Files, QDirIterator::Subdirectories);
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
  return directory.entryList(QStringList() << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG", QDir::Files);
}

const std::string DefaultPathTraverser::getImagePath(const std::string image) const
{
  return directory.filePath(QString(image.c_str())).toStdString();
}
