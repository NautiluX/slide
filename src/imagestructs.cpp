
#include "imagestructs.h"
#include "logger.h"

ImageDetails::ImageDetails()
{

}

ImageDetails::~ImageDetails()
{

}

ImageAspect ImageDetails::aspect() const
{
  if (width > height) 
  {
    return ImageAspect_Landscape;
  } 
  else if (height > width) 
  {
    return ImageAspect_Portrait;
  }

  // must be square, so just pick something
  return ImageAspect_Portrait;
}


bool ImageDetails::isValidForScreenAspect(const ImageAspectScreenFilter aspectScreen) const
{
  if(aspectScreen == ImageAspectScreenFilter_Any) 
  {
    return true;
  }
  if(aspectScreen == ImageAspectScreenFilter_Monitor) 
  {
    Log("Error: invalid aspect of ImageAspectScreenFilter_Monitor" );
    return false;
  }

  if (width > height) 
  {
    return aspectScreen == ImageAspectScreenFilter_Landscape;
  } 
  else if (height > width) 
  {
    return aspectScreen == ImageAspectScreenFilter_Portrait;
  } 

  // must be square so let is always display
  return true;
}
