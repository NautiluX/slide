#ifndef OVERLAY_H
#define OVERLAY_H

#include <iostream>
#include <QDir>
#include <iostream>
#include <QString>
#include <QStringList>

class MainWindow;
class Overlay
{
  public:
    Overlay(const std::string path);
    virtual ~Overlay();
    std::string getRenderedTopLeft(std::string filename);
    std::string getRenderedTopRight(std::string filename);
    std::string getRenderedBottomLeft(std::string filename);
    std::string getRenderedBottomRight(std::string filename);

    int getMarginTopLeft();
    int getFontsizeTopLeft();
    int getMarginTopRight();
    int getFontsizeTopRight();
    int getMarginBottomLeft();
    int getFontsizeBottomLeft();
    int getMarginBottomRight();
    int getFontsizeBottomRight();


  private:
    const std::string overlayInput;
    int margin;
    int fontsize;

    QString topLeftTemplate;
    QString topRightTemplate;
    QString bottomLeftTemplate;
    QString bottomRightTemplate;

    int topLeftMargin;
    int topRightMargin;
    int bottomLeftMargin;
    int bottomRightMargin;

    int topLeftFontsize;
    int topRightFontsize;
    int bottomLeftFontsize;
    int bottomRightFontsize;

    QStringList getOverlayComponents(QString corner);
    int getMargin(QStringList components);
    int getFontsize(QStringList components);
    QString getTemplate(QStringList components);

    QString getExifDate(std::string filename);
    void parseInput();
    std::string renderString(QString overlayTemplate, std::string filename);
};
#endif
