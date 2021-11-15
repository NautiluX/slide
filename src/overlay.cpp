#include "overlay.h"
#include "logger.h"
#include <QString>
#include <QDateTime>
#include <libexif/exif-data.h>
#include <unistd.h>
#include <QDate>
#include <QLocale>
#include <QTime>
#include <QFileInfo>
#include <QStringList>
#include <QRegExp>
#include <iostream>

Overlay::Overlay(const std::string overlayInput):
  overlayInput(overlayInput)
{
  parseInput();
}

Overlay::~Overlay() {}

void Overlay::parseInput() {
  QString str = QString(overlayInput.c_str());
  QStringList corners = str.split(QLatin1Char(';'));
  if (corners.size() > 0) {
    QStringList components = getOverlayComponents(corners[0]);
    topLeftTemplate = getTemplate(components);
    topLeftMargin = getMargin(components);
    topLeftFontsize = getFontsize(components);
  }
  if (corners.size() > 1) {
    QStringList components = getOverlayComponents(corners[1]);
    topRightTemplate = getTemplate(components);
    topRightMargin = getMargin(components);
    topRightFontsize = getFontsize(components);
  }
  if (corners.size() > 2) {
    QStringList components = getOverlayComponents(corners[2]);
    bottomLeftTemplate = getTemplate(components);
    bottomLeftMargin = getMargin(components);
    bottomLeftFontsize = getFontsize(components);
  }
  if (corners.size() > 3) {
    QStringList components = getOverlayComponents(corners[3]);
    bottomRightTemplate = getTemplate(components);
    bottomRightMargin = getMargin(components);
    bottomRightFontsize = getFontsize(components);
  }
}

QString Overlay::getTemplate(QStringList components){
  if (components.size()>3) {
    Log("template: ", components[3].toStdString());
    return components[3];
  }
  return "";
}

int Overlay::getMargin(QStringList components){
  if (components.size()>1) {
    Log("margin: ", components[1].toStdString());
    int num = components[1].toInt();
    if (num > 0) {
      return num;
    }
  }

  return 20;
}

int Overlay::getFontsize(QStringList components){
  if (components.size()>2) {
    Log("fontsize: ", components[2].toStdString());
    int num = components[2].toInt();
    if (num > 0) {
      return num;
    }
  }

  return 12;
}


QStringList Overlay::getOverlayComponents(QString corner) {
  QRegExp regex("([\\d]*)\\|([\\d]*)\\|(.*)");
  if (regex.exactMatch(corner)){
    return regex.capturedTexts();
  }
  QStringList malformed;
  malformed << "" << "" << "" << corner;
  return malformed;
}

std::string Overlay::getRenderedTopLeft(std::string filename) {
  return renderString(topLeftTemplate, filename);
}
std::string Overlay::getRenderedTopRight(std::string filename) {
  return renderString(topRightTemplate, filename);
}
std::string Overlay::getRenderedBottomLeft(std::string filename) {
  return renderString(bottomLeftTemplate, filename);
}
std::string Overlay::getRenderedBottomRight(std::string filename) {
  return renderString(bottomRightTemplate, filename);
}

int Overlay::getMarginTopLeft() {return topLeftMargin;}
int Overlay::getFontsizeTopLeft() {return topLeftFontsize;}
int Overlay::getMarginTopRight() {return topRightMargin;}
int Overlay::getFontsizeTopRight() {return topRightFontsize;}
int Overlay::getMarginBottomLeft() {return bottomLeftMargin;}
int Overlay::getFontsizeBottomLeft() {return bottomLeftFontsize;}
int Overlay::getMarginBottomRight() {return bottomRightMargin;}
int Overlay::getFontsizeBottomRight() {return bottomRightFontsize;}

std::string Overlay::renderString(QString overlayTemplate, std::string filename) {
  QString result = overlayTemplate;
  result.replace("<datetime>", QLocale::system().toString(QDateTime::currentDateTime()));
  result.replace("<date>", QLocale::system().toString(QDate::currentDate()));
  result.replace("<time>", QTime::currentTime().toString("hh:mm"));
  result.replace("<dir>", getDir(filename));
  result.replace("<path>", getPath(filename));
  result.replace("<filepath>", filename.c_str());
  result.replace("<filename>", getFilename(filename));
  result.replace("<basename>", getBasename(filename));
  result.replace("<exifdatetime>", getExifDate(filename));
  return result.toStdString();
}

QString Overlay::getFilename(std::string filename) {
  QFileInfo fileInfo(filename.c_str());
  return fileInfo.fileName();
}

QString Overlay::getBasename(std::string filename) {
  QFileInfo fileInfo(filename.c_str());
  return fileInfo.baseName();
}

QString Overlay::getDir(std::string filename) {
  QFileInfo fileInfo(filename.c_str());
  return fileInfo.dir().dirName();
}

QString Overlay::getPath(std::string filename) {
  QFileInfo fileInfo(filename.c_str());
  return fileInfo.path();
}

QString Overlay::getExifDate(std::string filename) {

  QString dateTime;
  ExifData *exifData = exif_data_new_from_file(filename.c_str());
  if (exifData)
  {
    ExifEntry *exifEntry = exif_data_get_entry(exifData, EXIF_TAG_DATE_TIME_ORIGINAL);

    if (exifEntry)
    {
      char buf[2048];
      dateTime = exif_entry_get_value(exifEntry, buf, sizeof(buf));
    }
    exif_data_free(exifData);
    QString exifDateFormat = "yyyy:MM:dd hh:mm:ss";
    QDateTime exifDateTime = QDateTime::fromString(dateTime, exifDateFormat);
    return QLocale::system().toString(exifDateTime);
  }
  return dateTime;
}
