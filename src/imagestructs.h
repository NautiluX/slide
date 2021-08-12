#ifndef IMAGESTRUCTS_H
#define IMAGESTRUCTS_H

#include <QTime>
#include <QVector>
#include <string>

// possible aspect ratios of an image
enum ImageAspect { ImageAspect_Landscape = 0, ImageAspect_Portrait, ImageAspect_Any, ImageAspect_Monitor /* match monitors aspect */ };

struct DisplayTimeWindow
{
    QTime startDisplay = QTime(0,0,0,0);
    QTime endDisplay = QTime(23,59,59,0);
};

// options to consider when displaying an image
struct ImageDisplayOptions
{
    ImageAspect onlyAspect = ImageAspect_Any;
    bool fitAspectAxisToWindow = false;
    QVector<DisplayTimeWindow> timeWindows;
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