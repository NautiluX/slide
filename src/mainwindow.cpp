#include "mainwindow.h"
#include "overlay.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPixmap>
#include <QBitmap>
#include <QKeyEvent>
#include <QGraphicsBlurEffect>
#include <libexif/exif-data.h>
#include <iostream>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QRect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QTimer::singleShot(5, this, SLOT(showFullScreen()));
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QLabel *label = this->findChild<QLabel*>("image");
    setCentralWidget(label);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    update();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
         QCoreApplication::quit();
    }
    else
        QWidget::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   updateImage(true);
}

void MainWindow::setImage(std::string path)
{
    currentImage = path;
    updateImage(false);
}

int MainWindow::getImageRotation()
{
    if (currentImage == "")
        return 0;

    int orientation = 0;
    ExifData *exifData = exif_data_new_from_file(currentImage.c_str());
    if (exifData)
    {
      ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
      ExifEntry *exifEntry = exif_data_get_entry(exifData, EXIF_TAG_ORIENTATION);

      if (exifEntry)
      {
        orientation = exif_get_short(exifEntry->data, byteOrder);
      }
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
    }

    return degrees;
}

void MainWindow::updateImage(bool immediately)
{
    if (currentImage == "")
      return;

    QLabel *label = this->findChild<QLabel*>("image");
    const QPixmap* oldImage = label->pixmap();
    if (oldImage != NULL && !immediately)
    {
      QPalette palette;
      palette.setBrush(QPalette::Background, *oldImage);
      this->setPalette(palette);
    }

    QPixmap p( currentImage.c_str() );
    if(debugMode)
    {
      std::cout << "size:" << p.width() << "x" << p.height() << std::endl;
    }

    QPixmap rotated = getRotatedPixmap(p);
    QPixmap scaled = getScaledPixmap(rotated);
    QPixmap background = getBlurredBackground(rotated, scaled);
    drawForeground(background, scaled);
    
    if (overlay != NULL)
    {
      drawText(background, overlay->getMarginTopLeft(), overlay->getFontsizeTopLeft(), overlay->getRenderedTopLeft(currentImage).c_str(), Qt::AlignTop|Qt::AlignLeft);
      drawText(background, overlay->getMarginTopRight(), overlay->getFontsizeTopRight(), overlay->getRenderedTopRight(currentImage).c_str(), Qt::AlignTop|Qt::AlignRight);
      drawText(background, overlay->getMarginBottomLeft(), overlay->getFontsizeBottomLeft(), overlay->getRenderedBottomLeft(currentImage).c_str(), Qt::AlignBottom|Qt::AlignLeft);
      drawText(background, overlay->getMarginBottomRight(), overlay->getFontsizeBottomRight(), overlay->getRenderedBottomRight(currentImage).c_str(), Qt::AlignBottom|Qt::AlignRight);
      if (debugMode)
      {
        // draw a thumbnail version of the source image in the bottom left, to check for cropping issues
        QPainter pt(&background);
        QBrush brush(QColor(255, 255, 255, 255));
        int margin = 10;
        QPixmap thumbNail = p.scaledToWidth(200, Qt::SmoothTransformation);
        pt.fillRect(background.width() - thumbNail.width() - 2*margin,
                     background.height()-thumbNail.height() - 2*margin,
                     thumbNail.width() +2*margin, thumbNail.height()+2*margin, brush);

        pt.drawPixmap( background.width() - thumbNail.width() - margin,
                     background.height()-thumbNail.height() - margin, 
                     thumbNail);
      }
    }

    label->setPixmap(background);

    if (oldImage != NULL && !immediately)
    {
      auto effect = new QGraphicsOpacityEffect(label);
      effect->setOpacity(0.0);
      label->setGraphicsEffect(effect);
      QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
      animation->setDuration(1000);
      animation->setStartValue(0);
      animation->setEndValue(1);
      animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    update();
}

void MainWindow::drawText(QPixmap& image, int margin, int fontsize, QString text, int alignment) {
  //std::cout << "text: " << text.toStdString()  << " margin: " << margin << " fontsize: " << fontsize<< std::endl;
  QPainter pt(&image);
  pt.setPen(QPen(Qt::white));
  pt.setFont(QFont("Sans", fontsize, QFont::Bold));
  QRect marginRect = image.rect().adjusted(
      margin,
      margin,
      margin*-1,
      margin*-1);
  pt.drawText(marginRect, alignment, text);
}

void MainWindow::drawForeground(QPixmap& background, const QPixmap& foreground) {
    QPainter pt(&background);
    QBrush brush(QColor(0, 0, 0, 255-backgroundOpacity));
    pt.fillRect(0,0,background.width(), background.height(), brush);
    pt.drawPixmap((background.width()-foreground.width())/2, (background.height()-foreground.height())/2, foreground);
}

void MainWindow::setOverlay(Overlay* o)
{
  overlay = o;
}

void MainWindow::setAspect(char aspectIn)
{
  aspect = aspectIn;
}

QPixmap MainWindow::getBlurredBackground(const QPixmap& originalSize, const QPixmap& scaled)
{
    if (scaled.width() < width()) {
      QPixmap background = blur(originalSize.scaledToWidth(width(), Qt::SmoothTransformation));
      QRect rect(0, (background.height() - height())/2, width(), height());
      return background.copy(rect);
    } else {
      // aspect 'p' or the image is not as wide as the screen
      QPixmap background = blur(originalSize.scaledToHeight(height(), Qt::SmoothTransformation));
      QRect rect((background.width() - width())/2, 0, width(), height());
      return background.copy(rect);
    }
}

QPixmap MainWindow::getRotatedPixmap(const QPixmap& p)
{
    QMatrix matrix;
    matrix.rotate(getImageRotation());
    return p.transformed(matrix);
}

QPixmap MainWindow::getScaledPixmap(const QPixmap& p)
{
  if (fitAspectAxisToWindow)
  {
    if ( aspect == 'p')
    {
      // potrait mode, make height of image fit screen and crop top/bottom
      QPixmap pTemp = p.scaledToHeight(height(), Qt::SmoothTransformation);
      return pTemp.copy(0,0,width(),height());
    }
    else if ( aspect == 'l')
    {
      // landscape mode, make width of image fit screen and crop top/bottom
      QPixmap pTemp = p.scaledToWidth(width(), Qt::SmoothTransformation);
      //int imageTempWidth = pTemp.width();
      //int imageTempHeight = pTemp.height();
      return pTemp.copy(0,0,width(),height());
    }
  }

  // just scale the best we can for the given photo
  int w = width();
  int h = height();
  return p.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::drawBackground(const QPixmap& originalSize, const QPixmap& scaled)
{
    QPalette palette;
    if (scaled.width() < width()) {
        QPixmap background = blur(originalSize.scaledToHeight(height()));
        QRect rect((background.width() - width())/2, 0, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    } else {
        QPixmap background = blur(originalSize.scaledToHeight(height()));
        QRect rect((background.width() - width())/2, 0, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    }
    this->setPalette(palette);
}

QPixmap MainWindow::blur(const QPixmap& input)
{
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(input);
    QGraphicsBlurEffect effect;
    effect.setBlurRadius(blurRadius);
    item.setGraphicsEffect(&effect);
    scene.addItem(&item);
    QImage res(input.size(), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( 0, 0, input.width(), input.height()) );
    return QPixmap::fromImage(res);
}

void MainWindow::setBlurRadius(unsigned int blurRadius)
{
    this->blurRadius = blurRadius;
}

void MainWindow::setBackgroundOpacity(unsigned int backgroundOpacity)
{
    this->backgroundOpacity = backgroundOpacity;
}

void MainWindow::warn(std::string text)
{
  QLabel *label = this->findChild<QLabel*>("image");
  label->setText(text.c_str());
}
