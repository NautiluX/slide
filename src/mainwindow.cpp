#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPixmap>
#include <QBitmap>
#include <QKeyEvent>
#include <QGraphicsBlurEffect>
#include <libexif/exif-data.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    image(new QLabel)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QMainWindow::showFullScreen();
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QLabel *label = this->findChild<QLabel*>("image");
    setCentralWidget(label);
    label->setStyleSheet("QLabel {  background-color: rgba(0, 0, 0, 180);}");
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
    updateImage();
}

void MainWindow::setImage(std::string path)
{
    currentImage = path;
    updateImage();
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

void MainWindow::updateImage()
{
    if (currentImage == "")
        return;

    QPixmap p( currentImage.c_str() );
    QPixmap rotated = getRotatedPixmap(p);
    QPixmap scaled = getScaledPixmap(rotated);

    QLabel *label = this->findChild<QLabel*>("image");
    label->setPixmap(scaled);

    drawBackground(rotated, scaled);
    update();
}

QPixmap MainWindow::getRotatedPixmap(const QPixmap& p)
{
    QMatrix matrix;
    matrix.rotate(getImageRotation());
    return p.transformed(matrix);
}

QPixmap MainWindow::getScaledPixmap(const QPixmap& p)
{
    int w = width();
    int h = height();
    return p.scaled(w, h, Qt::KeepAspectRatio);
}

void MainWindow::drawBackground(const QPixmap& originalSize, const QPixmap& scaled)
{
    QPalette palette;
    if (scaled.width() < width()) {
        QPixmap background = originalSize.scaledToWidth(width());
        QRect rect(0, (background.height() - height())/2, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    } else {
        QPixmap background = originalSize.scaledToHeight(height());
        QRect rect((background.width() - width())/2, 0, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    }
    this->setPalette(palette);
}
