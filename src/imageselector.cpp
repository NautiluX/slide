#include "imageselector.h"
#include "pathtraverser.h"
#include "mainwindow.h"
#include "logger.h"
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

ImageSelector::ImageSelector(std::unique_ptr<PathTraverser>& pathTraverserIn):
  pathTraverser(std::move(pathTraverserIn))
{
}

ImageSelector::ImageSelector() {}

ImageSelector::~ImageSelector(){}

int ReadExifTag(ExifData* exifData, ExifTag tag, bool shortRead = false)
{
  int value = -1;
  ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
  ExifEntry *exifEntry = exif_data_get_entry(exifData, tag);

  if (exifEntry)
  {
    if (shortRead)
    {
      return exif_get_short(exifEntry->data, byteOrder);
    }
    
    return exif_get_long(exifEntry->data, byteOrder);
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

  imageDetails.options = pathTraverser->UpdateOptionsForImage(imageDetails.filename, baseOptions);
  
  return imageDetails;
}

bool ImageSelector::imageInsideTimeWindow(const QVector<DisplayTimeWindow> &timeWindows)
{
  if(timeWindows.count() == 0)
  {
      return true; // no specified time windows means always display
  }
  const QTime currentTime = QTime::currentTime();
  for(auto &window : timeWindows)
  {
    if(currentTime > window.startDisplay && currentTime < window.endDisplay)
    {
      return true;
    }
  }
  if(ShouldLog() && timeWindows.count() > 0)
  {
    Log( "image display time outside window: ");
    for(auto &timeWindow : timeWindows) 
    {
      Log("time: ", timeWindow.startDisplay.toString().toStdString(), "-", timeWindow.endDisplay.toString().toStdString());
    }
  }
  return false;
}

bool ImageSelector::imageMatchesFilter(const ImageDetails& imageDetails)
{
  if(!QFileInfo::exists(QString(imageDetails.filename.c_str())))
  {
    Log("file not found: ", imageDetails.filename);
    return false;
  }

  if(!imageValidForAspect(imageDetails)) 
  {
    Log("image aspect ratio doesn't match filter '", imageDetails.options.onlyAspect, "' : ", imageDetails.filename);
    return false;
  }

  if(!imageInsideTimeWindow(imageDetails.options.timeWindows))
  {
    return false;
  }
  return true;
}

bool ImageSelector::imageValidForAspect(const ImageDetails& imageDetails)
{
  if (imageDetails.isValidForScreenAspect(imageDetails.options.onlyAspect))
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
  return imageDetails;
}

unsigned int RandomImageSelector::selectRandom(const QStringList& images) const
{
  Log("images: ", images.size());
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
  bool bReloadedImages = false;
  imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(current_image_shuffle).toStdString()), baseOptions);
  current_image_shuffle = current_image_shuffle + 1; // ignore and move to next image
  while(!imageMatchesFilter(imageDetails)) {
    if(current_image_shuffle >= images.size()) {
      // don't keep looping
      if(bReloadedImages == true)
        return ImageDetails();
      bReloadedImages = true;
    }
    reloadImagesIfNoneLeft();
    imageDetails = populateImageDetails(pathTraverser->getImagePath(images.at(current_image_shuffle).toStdString()),baseOptions);
    current_image_shuffle = current_image_shuffle + 1; // ignore and move to next image
  }
  std::cout << "updating image: " << imageDetails.filename << std::endl;
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
  bool bReloadedImages = false;
  imageDetails = populateImageDetails(pathTraverser->getImagePath(images.takeFirst().toStdString()), baseOptions);
  while(!imageMatchesFilter(imageDetails)) {
    if (images.size() == 0) {
      // don't keep looping
      if(bReloadedImages == true)
        return ImageDetails();
      bReloadedImages = true;
    }

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
    if(ShouldLog())
    {
      Log( "read ", images.size(), " images.");
      for (int i = 0;i <images.size();i++){
          Log(images[i].toStdString());
      }
    }
  }
}


ListImageSelector::ListImageSelector()
{
  currentSelector = imageSelectors.begin();
}

ListImageSelector::~ListImageSelector() 
{
}

void ListImageSelector::AddImageSelector(std::unique_ptr<ImageSelector>& selector, const bool exclusiveIn, const ImageDisplayOptions& baseDisplayOptionsIn)
{
  SelectoryEntry entry;
  entry.selector = std::move(selector);
  entry.exclusive = exclusiveIn;
  entry.baseDisplayOptions = baseDisplayOptionsIn;
  imageSelectors.push_back(std::move(entry));
  currentSelector = imageSelectors.begin();
}


const ImageDetails ListImageSelector::getNextImage(const ImageDisplayOptions& baseOptions)
{
  // check for exclusive time windows
  for(auto& selector: imageSelectors)
  {
    if (imageInsideTimeWindow(selector.baseDisplayOptions.timeWindows) && selector.exclusive) 
    {
      ImageDisplayOptions options = baseOptions;
      if (selector.baseDisplayOptions.fitAspectAxisToWindow)
        options.fitAspectAxisToWindow = true;
      return selector.selector->getNextImage(options);
    }
  }

  // fall back to the next in the list
  do
  {  
    ++currentSelector;
    if(currentSelector == imageSelectors.end())
    {
      currentSelector = imageSelectors.begin();
    }
    if (imageInsideTimeWindow(currentSelector->baseDisplayOptions.timeWindows))
    {
      ImageDisplayOptions options = baseOptions;
      if (currentSelector->baseDisplayOptions.fitAspectAxisToWindow)
        options.fitAspectAxisToWindow = true;
      return currentSelector->selector->getNextImage(options);
    }
  }
  while(true);
}