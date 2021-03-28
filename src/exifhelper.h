#ifndef EXIFHELPER_H
#define EXIFHELPER_H

#include <libexif/exif-data.h>
#include <QString>


class ExifHelper
{
public:
    ExifHelper();
    virtual ~ExifHelper();

    void setImage(const std::string path);
    ExifData *getExifData();
    int getImageRotation();
    const QString getImageInfo();
    const QString getExifXResolution();
    const QString getExifYResolution();
    const QString getExifXDimension();
    const QString getExifYDimension();
    const QString getExifDate();
    const QString getExifGPS();
    const QString getExifCamera();

private:
    bool isHeif(const std::string path);
    ExifData * getExifDataFromHeif(const std::string path);
    const QString getExifAscii(ExifEntry *e);
    double getExifRationalValue(const unsigned char *e, ExifByteOrder o);
    double getExifGeoCooValue(const unsigned char *e, ExifByteOrder o);

    ExifData *m_exifData;
    bool m_isHeif;
    ExifByteOrder m_byteOrder;
    QString m_path;

};

#endif // EXIFHELPER_H
