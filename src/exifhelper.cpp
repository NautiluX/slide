#include "exifhelper.h"
#include <QVariant>
#include <QDateTime>
#include <QLocale>

ExifHelper::ExifHelper():
    m_haveExifData(false)
{

}
ExifHelper::~ExifHelper(){
    if(m_exifData)
        exif_data_free(m_exifData);
}

void ExifHelper::setImage(const std::string path){
    m_path=path.c_str();
    if(m_haveExifData){
        exif_data_free(m_exifData);
    }
    m_exifData=exif_data_new_from_file(path.c_str());
    if(m_exifData){
        m_haveExifData=true;
        m_byteOrder=exif_data_get_byte_order(m_exifData);
    }
    else m_haveExifData=false;
}

ExifData *ExifHelper::getExifData(){
    return(m_exifData);
}

int ExifHelper::getImageRotation()
{
    int orientation = 0;
    if (m_exifData)
    {
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_ORIENTATION);

        if (exifEntry)
        {
            orientation = exif_get_short(exifEntry->data, m_byteOrder);
        }
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



const QString ExifHelper::getImageInfo(){
    QString txt("Path: ");
    txt=txt+m_path+"\n";
    if (m_exifData)
    {

        txt=txt+"XxY: "+getExifXDimension()+"x"+getExifYDimension()+"\n";
        txt=txt+"XxY resolution: "+getExifXResolution()+"x"+getExifYResolution()+"\n";
        txt=txt+"date: "+getExifDate()+"\n";
        txt=txt+"camera: "+getExifCamera()+"\n";
        txt=txt+getExifGPS();

    }else{
        txt=txt+"No exif data found :(";
    }

    return(txt);
}

const QString ExifHelper::getExifXResolution(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_X_RESOLUTION);
        if (exifEntry)
        {
            txt=txt+QString::number(exif_get_long(exifEntry->data, m_byteOrder));
        }
    }
    return txt;
}


const QString ExifHelper::getExifYResolution(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_Y_RESOLUTION);
        if (exifEntry)
        {
            txt=txt+QString::number(exif_get_long(exifEntry->data, m_byteOrder));
        }
    }
    return txt;
}

const QString ExifHelper::getExifXDimension(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_PIXEL_X_DIMENSION);
        if (exifEntry)
        {
            txt=txt+QString::number(exif_get_long(exifEntry->data, m_byteOrder));
        }
    }
    return txt;
}

const QString ExifHelper::getExifYDimension(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_PIXEL_Y_DIMENSION);
        if (exifEntry)
        {
            txt=txt+QString::number(exif_get_long(exifEntry->data, m_byteOrder));
        }
    }
    return txt;
}

const QString ExifHelper::getExifDate(){
    QString dateTime;
    if (m_exifData)
    {
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_DATE_TIME_ORIGINAL);
        if (exifEntry)
        {
            dateTime = getExifAscii(exifEntry);
        }
        QString exifDateFormat = "yyyy:MM:dd hh:mm:ss";
        QDateTime exifDateTime = QDateTime::fromString(dateTime, exifDateFormat);
        return QLocale::system().toString(exifDateTime);
    }
    return dateTime;
}

const QString ExifHelper::getExifGPS(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_LATITUDE_REF);
        if(exifEntry)
        {
            //txt=txt+" "+((getExifAscii(exifEntry)[0]=='N')?"+":"-");
            txt=txt+getExifAscii(exifEntry)+" ";
        }
        exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_LATITUDE);
        if(exifEntry){
            txt=txt+QString::number(getExifGeoCooValue(exifEntry->data,m_byteOrder),'f',4)+"°; ";
        }
        exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_LONGITUDE_REF);
        if(exifEntry){
            //txt=txt+" "+((getExifAscii(exifEntry)[0]=='E')?"+":"-");
            txt=txt+" "+getExifAscii(exifEntry)+" ";
        }
        exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_LONGITUDE);
        if(exifEntry){
            txt=txt+QString::number(getExifGeoCooValue(exifEntry->data,m_byteOrder),'f',4)+"°; ";
        }
        exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_ALTITUDE);
        if(exifEntry){
            txt=txt+QString::number(getExifRationalValue(exifEntry->data,m_byteOrder),'f',2);
        }
        exifEntry = exif_content_get_entry(m_exifData->ifd[EXIF_IFD_GPS], (ExifTag)EXIF_TAG_GPS_ALTITUDE_REF);
        if(exifEntry){
            if(*(exifEntry->data)==0)
            {
                txt=txt+"m ASL"; //Meters Above Sea Level
            }else{
                txt=txt+"m BSL"; //Meters Below Sea Level?
            }
        }
    }
    return txt;
}

const QString ExifHelper::getExifCamera(){
    QString txt("");
    if(m_exifData){
        ExifEntry *exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_MAKE);
        if (exifEntry)
        {
            txt=txt+getExifAscii(exifEntry).toUpper()+", ";
        }
        exifEntry = exif_data_get_entry(m_exifData, EXIF_TAG_MODEL);
        if (exifEntry)
        {
            txt=txt+getExifAscii(exifEntry);
        }
    }
    return txt;
}

const QString ExifHelper::getExifAscii(ExifEntry *e){
    char data[e->size];
    strncpy(data,(char*)e->data,e->size);
    return QVariant(data).toString();
}
double ExifHelper::getExifRationalValue(const unsigned char *e, ExifByteOrder o){
    ExifRational r(exif_get_rational(e,o));
    return (r.numerator/(double)r.denominator);
}
double ExifHelper::getExifGeoCooValue(const unsigned char *e, ExifByteOrder o){
    return( getExifRationalValue(e,o)+
            getExifRationalValue(e+sizeof(ExifRational),o)/60+
            getExifRationalValue(e+2*sizeof(ExifRational),o)/3600
            );
}
