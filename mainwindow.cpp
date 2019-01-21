#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPixmap>
#include <QBitmap>
#include <QKeyEvent>
#include <QGraphicsBlurEffect>

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
    label->setStyleSheet("QLabel {  background-color: rgba(0, 0, 0, 230);}");
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

void MainWindow::updateImage()
{
    if (currentImage == "")
    {
        return;
    }
    QLabel *label = this->findChild<QLabel*>("image");
    QPixmap p( currentImage.c_str() );
    int w = label->width();
    int h = label->height();
    QPixmap scaled = p.scaled(w,h,Qt::KeepAspectRatio);
    label->setPixmap(scaled);


    QPalette palette;
    if (scaled.width() < width()) {
        QPixmap background = p.scaledToWidth(width());
        QRect rect(0, (background.height() - height())/2, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    } else {
        QPixmap background = p.scaledToHeight(height());
        QRect rect((background.width() - width())/2, 0, width(), height());
        background = background.copy(rect);
        palette.setBrush(QPalette::Background, background);
    }
    this->setPalette(palette);
    update();
}
