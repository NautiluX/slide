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

ImageSelector::ImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
  pathTraverser(pathTraverser)
{
}

ImageSelector::~ImageSelector(){}

void ImageSelector::setDebugMode(bool debugModeIn) 
{ 
  debugMode = debugModeIn;
}

int ReadExifTag(ExifData* exifData, ExifTag tag, bool shortRead = false)
{
  int value = -1;
  ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
  ExifEntry *exifEntry = exif_data_get_entry(exifData, tag);

  if (exifEntry)
  {
    if (shortRead)
    {
      value = exif_get_short(exifEntry->data, byteOrder);
    }
    else
    {
      value = exif_get_long(exifEntry->data, byteOrder);
    }
  }
  return value;
}

ImageDetails ImageSelector::populateImageDetails(const std::string&fileName, const ImageDisplayOptions &baseOptions)
{
  ImageDetails imageDetails;
  int orientation = -1;
  int imageWidth = -1;
  int imageHeight = -1;
  ExifData *exifData = exif_data_new_from_file(fileName.c_str());
  if (exifData)
  {
    orientation = ReadExifTag(exifData, EXIF_TAG_ORIENTATION, true);

    /*
    // It looks like you can't trust Exif dimensions, so just forcefully load the file below
    // try to get the image dimensions from exifData so we don't need to fully load the file
    imageWidth = ReadExifTag(exifData, EXIF_TAG_IMAGE_WIDTH);
    if ( imageWidth == -1)
      imageWidth = ReadExifTag(exifData, EXIF_TAG_PIXEL_X_DIMENSION);

    imageHeight = ReadExifTag(exifData, EXIF_TAG_RELATED_IMAGE_WIDTH); // means height, height is related to width
    if ( imageHeight == -1)
      imageHeight = ReadExifTag(exifData, EXIF_TAG_PIXEL_Y_DIMENSION);*/

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
      default:
        break;
  }

  if (imageWidth <=0 || imageHeight <=0) 
  {
    // fallback to QPixmap to determine image size
    QPixmap p( fileName.c_str() );
    imageWidth = p.width();
    imageHeight = p.height();
  }

  // if the image is rotated then swap height/width here to show displayed sizes
  if( degrees == 90 || degrees == 270 )
  {
    std::swap(imageWidth,imageHeight);
  }

  // setup the imageDetails structure
  imageDetails.filename = fileName;

  imageDetails.width = imageWidth;
  imageDetails.height = imageHeight;
  imageDetails.rotation = degrees;
  if (imageWidth > imageHeight) {
    imageDetails.aspect = ImageAspect_Landscape;
  } else if (imageHeight > imageWidth) {
    imageDetails.aspect = ImageAspect_Portrait;
  } else {
    imageDetails.aspect = ImageAspect_Any;
  }
  imageDetails.options = baseOptions;
  return imageDetails;
}

bool ImageSelector::imageMatchesFilter(const ImageDetails& imageDetails)
{
  if(!QFileInfo::exists(QString(imageDetails.filename.c_str())))
  {
    if(debugMode)
    {
      std::cout << "file not found: " << imageDetails.filename << std::endl;
    }
    return false;
  }

  if(!imageValidForAspect(imageDetails)) 
  {
    if(debugMode)
    {
      std::cout << "image aspect ratio doesn't match filter '" << imageDetails.options.onlyAspect << "' : " << imageDetails.filename << std::endl;
    }
    return false;
  }

  return true;
}

bool ImageSelector::imageValidForAspect(const ImageDetails& imageDetails)
{
  if (imageDetails.options.onlyAspect == ImageAspect_Any ||
      imageDetails.aspect == imageDetails.options.onlyAspect)
  {
    return true;
  }
  return false;
}


RandomImageSelector::RandomImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
  ImageSelector(pathTraverser)
{
  srand (time(NULL));
}

RandomImageSelector::~RandomImageSelector(){}

const ImageDetails RandomImageSelector::getNextImage(const ImageDisplayOptions &baseOptions)
{
  ImageDetails imageDetails;
  try
  {
    QStringList images = pathTraverser->getImages();
    unsigned int selectedImage = selectRandom(images);
    imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(selectedImage).toStdString()), baseOptions);
    while(!imageMatchesFilter(imageDetails))
    {
      unsigned int selectedImage = selectRandom(images);
      imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(selectedImage).toStdString()), baseOptions);
    }
  }
  catch(const std::string& err) 
  {
    std::cerr << "Error: " << err << std::endl;
  }
  std::cout << "updating image: " << imageDetails.filename << std::endl;
  imageDetails.options = pathTraverser->UpdateOptionsForImage(imageDetails.filename, imageDetails.options);
  return imageDetails;
}

unsigned int RandomImageSelector::selectRandom(const QStringList& images) const
{
  if(debugMode)
  {
    std::cout << "images: " << images.size() << std::endl;
  }
  if (images.size() == 0)
  {
    throw std::string("No jpg images found in given folder");
  }
  return rand() % images.size();
}

ShuffleImageSelector::ShuffleImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
  ImageSelector(pathTraverser),
  current_image_shuffle(-1),
  images()
{
  srand (time(NULL));
}

ShuffleImageSelector::~ShuffleImageSelector()
{
}

const ImageDetails ShuffleImageSelector::getNextImage(const ImageDisplayOptions &baseOptions)
{
  reloadImagesIfNoneLeft();
  ImageDetails imageDetails;
  if (images.size() == 0)
  {
    return imageDetails;
  }
  imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(current_image_shuffle).toStdString()), baseOptions);
  current_image_shuffle = current_image_shuffle + 1; // ignore and move to next image
  while(!imageMatchesFilter(imageDetails)) {
    reloadImagesIfNoneLeft();
    imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(current_image_shuffle).toStdString()),baseOptions);
    current_image_shuffle = current_image_shuffle + 1; // ignore and move to next image
  }
  std::cout << "updating image: " << imageDetails.filename << std::endl;
  imageDetails.options = pathTraverser->UpdateOptionsForImage(imageDetails.filename, imageDetails.options);
  return imageDetails;
}

void ShuffleImageSelector::reloadImagesIfNoneLeft()
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
}

SortedImageSelector::SortedImageSelector(std::unique_ptr<PathTraverser>& pathTraverser):
  ImageSelector(pathTraverser),
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

const ImageDetails SortedImageSelector::getNextImage(const ImageDisplayOptions &baseOptions)
{
  reloadImagesIfEmpty();
  ImageDetails imageDetails;
  if (images.size() == 0)
  {
    return imageDetails;
  }
  imageDetails = populateImageDetails(pathTraverser->getImagePath(images.takeFirst().toStdString()), baseOptions);
  while(!imageMatchesFilter(imageDetails)) {
    reloadImagesIfEmpty();
    imageDetails = populateImageDetails(pathTraverser->getImagePath(images.takeFirst().toStdString()), baseOptions);
  }

  std::cout << "updating image: " << imageDetails.filename << std::endl;
  imageDetails.options = pathTraverser->UpdateOptionsForImage(imageDetails.filename, imageDetails.options);
  return imageDetails;
}

void SortedImageSelector::reloadImagesIfEmpty()
{
  if (images.size() == 0)
  {
    images = pathTraverser->getImages();
    std::sort(images.begin(), images.end());
    if(debugMode)
    {
      std::cout << "read " << images.size() << " images." << std::endl;
      for (int i = 0;i <images.size();i++){
          std::cout << images[i].toStdString() << std::endl;
      }
    }
  }
}
