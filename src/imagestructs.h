#ifndef IMAGESTRUCTS_H
#define IMAGESTRUCTS_H

#include <string>

// possible aspect ratios of an image
enum ImageAspect { ImageAspect_Landscape = 0, ImageAspect_Portrait, ImageAspect_Any, ImageAspect_Monitor /* match monitors aspect */ };

// options to consider when displaying an image
struct ImageDisplayOptions
{
    ImageAspect onlyAspect = ImageAspect_Any;
    bool fitAspectAxisToWindow = false;
};

// details of a particular image
struct ImageDetails
{
    int width = 0;
    int height = 0;
    int rotation = 0;
    ImageAspect aspect = ImageAspect_Any;
    std::string filename;
    ImageDisplayOptions options;
};


#endif // IMAGESTRUCTS_H