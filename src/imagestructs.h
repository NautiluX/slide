#ifndef IMAGESTRUCTS_H
#define IMAGESTRUCTS_H

#include <QTime>
#include <QVector>
#include <string>

// possible aspect ratios of an image
enum ImageAspect { ImageAspect_Landscape = 0, ImageAspect_Portrait};
enum ImageAspectScreenFilter { ImageAspectScreenFilter_Landscape = 0, ImageAspectScreenFilter_Portrait, ImageAspectScreenFilter_Any, ImageAspectScreenFilter_Monitor /* match monitors aspect */ };

struct DisplayTimeWindow
{
    QTime startDisplay = QTime(0,0,0,0);
    QTime endDisplay = QTime(23,59,59,0);
    
    bool operator!=(const DisplayTimeWindow &b) const
    {
        return startDisplay != b.startDisplay || endDisplay != b.endDisplay;
    }
};

// options to consider when displaying an image
struct ImageDisplayOptions
{
    ImageAspectScreenFilter onlyAspect = ImageAspectScreenFilter_Any;
    bool fitAspectAxisToWindow = false;
    QVector<DisplayTimeWindow> timeWindows;
};

// details of a particular image
class ImageDetails
{
public:
    ImageDetails();
    ~ImageDetails();
    bool isValidForScreenAspect(const ImageAspectScreenFilter aspectDisplay) const;
    ImageAspect aspect() const;

public:
    int width = 0;
    int height = 0;
    int rotation = 0;
    std::string filename;
    ImageDisplayOptions options;
};


#endif // IMAGESTRUCTS_H