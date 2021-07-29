#include "imageselector.h"
#include "pathtraverser.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <libexif/exif-data.h>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine

ImageSelector::ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect):
  pathTraverser(pathTraverser), _aspect(aspect)
{
}

ImageSelector::~ImageSelector(){}

int ImageSelector::getImageRotation(const std::string &fileName)
{
  int orientation = 0;
  ExifData *exifData = exif_data_new_from_file(fileName.c_str());
  if (exifData)
  {
    ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
    ExifEntry *exifEntry = exif_data_get_entry(exifData, EXIF_TAG_ORIENTATION);

    if (exifEntry)
    {
      orientation = exif_get_short(exifEntry->data, byteOrder);
    }
    exif_data_free(exifData);
  }

  int degrees = 0;
  switch(orientation) {
      case 8:
      degrees = 270;
      break;
      case 3:
      degrees = 180;
      break;
      case 6:
      degrees = 90;
      break;
  }
  return degrees;
}

bool ImageSelector::imageValidForAspect(const std::string &fileName)
{
  QPixmap p( fileName.c_str() );
  int imageWidth = p.width();
  int imageHeight = p.height();
  int rotation = getImageRotation(fileName);
  if ( rotation == 90 || rotation == 270 )
  {
    std::swap(imageWidth,imageHeight);
  }

  switch(_aspect)
  {
    case 'a':
      // allow all
      break;
    case 'l':
      if ( imageWidth < imageHeight ) 
      {
        return false;
      }
      break;
    case 'p':
      if ( imageHeight < imageWidth ) 
      {
        return false;
      }
      break;
  }
  return true;
}


RandomImageSelector::RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect):
  ImageSelector(pathTraverser, aspect)
{
  srand (time(NULL));
}

RandomImageSelector::~RandomImageSelector(){}

std::string RandomImageSelector::getNextImage()
{
  std:: string filename;
  try
  {
    while (filename.empty())
    {
      QStringList images = pathTraverser->getImages();
      unsigned int selectedImage = selectRandom(images);
      filename = pathTraverser->getImagePath(images.at(selectedImage).toStdString());
      if (!imageValidForAspect(filename))
      {
        filename.clear();
      }
    }

  }
  catch(const std::string& err) 
  {
    std::cerr << "Error: " << err << std::endl;
  }
  std::cout << "updating image: " << filename << std::endl;
  return filename;
}

unsigned int RandomImageSelector::selectRandom(const QStringList& images) const
{
  if(_debugMode)
  {
    std::cout << "images: " << images.size() << std::endl;
  }
  if (images.size() == 0)
  {
    throw std::string("No jpg images found in given folder");
  }
  return rand() % images.size();
}

ShuffleImageSelector::ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect):
  ImageSelector(pathTraverser, aspect),
  current_image_shuffle(-1),
  images()
{
  srand (time(NULL));
}

ShuffleImageSelector::~ShuffleImageSelector()
{
}

std::string ShuffleImageSelector::getNextImage()
{
  if (images.size() == 0 || current_image_shuffle >= images.size())
  {
    current_image_shuffle = 0;
    images = pathTraverser->getImages();
    std::cout << "Shuffling " << images.size() << " images." << std::endl;
    std::random_device rd;
    std::mt19937 randomizer(rd());
    std::shuffle(images.begin(), images.end(), randomizer);
  }
  if (images.size() == 0)
  {
    return "";
  }
  std::string filename = pathTraverser->getImagePath(images.at(current_image_shuffle).toStdString());
  if(!QFileInfo::exists(QString(filename.c_str())))
  {
    std::cout << "file not found: " << filename << std::endl;
    current_image_shuffle = images.size();
    return getNextImage();
  }
  std::cout << "updating image: " << filename << std::endl;
  current_image_shuffle = current_image_shuffle + 1;
  return filename;
}

SortedImageSelector::SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser, char aspect):
  ImageSelector(pathTraverser, aspect),
  images()
{
  srand (time(NULL));
}

SortedImageSelector::~SortedImageSelector()
{
}

bool operator<(const QString& lhs, const QString& rhs) noexcept{
  if (lhs.count(QLatin1Char('/')) < rhs.count(QLatin1Char('/'))) {
    return true;
  }
  if (lhs.count(QLatin1Char('/')) > rhs.count(QLatin1Char('/'))) {
    return false;
  }

  return lhs.toStdString() < rhs.toStdString();

}

std::string SortedImageSelector::getNextImage()
{
  if (images.size() == 0)
  {
    images = pathTraverser->getImages();
    std::sort(images.begin(), images.end());
    std::cout << "read " << images.size() << " images." << std::endl;
    for (int i = 0;i <images.size();i++){
      
        std::cout << images[i].toStdString() << std::endl;
    }
  }
  if (images.size() == 0)
  {
    return "";
  }
  std::string filename = pathTraverser->getImagePath(images.takeFirst().toStdString());
  if(!QFileInfo::exists(QString(filename.c_str())))
  {
    std::cout << "file not found: " << filename << std::endl;
    return getNextImage();
  }
  std::cout << "updating image: " << filename << std::endl;
  return filename;
}
