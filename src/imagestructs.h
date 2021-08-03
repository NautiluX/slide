#ifndef IMAGESTRUCTS_H
#define IMAGESTRUCTS_H

#include <string>

// possible aspect ratios of an image
enum EImageAspect { EImageAspect_Landscape = 0, EImageAspect_Portrait, EImageAspect_Any };

// options to consider when displaying an image
struct ImageDisplayOptions_t
{
    EImageAspect onlyAspect = EImageAspect_Any;
    bool fitAspectAxisToWindow = false;
};

// details of a particular image
struct ImageDetails_t
{
    int width = 0;
    int height = 0;
    int rotation = 0;
    EImageAspect aspect = EImageAspect_Any;
    std::string filename;
    ImageDisplayOptions_t options;
};


#endif // IMAGESTRUCTS_H