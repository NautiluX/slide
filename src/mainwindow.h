#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

namespace Ui {
class MainWindow;
}
class QLabel;
class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent* event);
    ~MainWindow();
    void setImage(std::string path);

private:
    Ui::MainWindow *ui;
    QLabel *image;
    std::string currentImage;
    void updateImage();
    int getImageRotation();

    QPixmap getRotatedPixmap(const QPixmap& p);
    QPixmap getScaledPixmap(const QPixmap& p);
    void drawBackground(const QPixmap& originalSize, const QPixmap& scaled);
};

#endif // MAINWINDOW_H
