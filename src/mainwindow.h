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
    void setBlurRadius(unsigned int blurRadius);
    void setBackgroundOpacity(unsigned int opacity);
    void warn(std::string text);

private:
    Ui::MainWindow *ui;

    std::string currentImage;
    unsigned int blurRadius = 20;
    unsigned int backgroundOpacity = 150;

    void updateImage();
    int getImageRotation();

    QPixmap getRotatedPixmap(const QPixmap& p);
    QPixmap getScaledPixmap(const QPixmap& p);
    void drawBackground(const QPixmap& originalSize, const QPixmap& scaled);
    QPixmap blur(const QPixmap& input);
};

#endif // MAINWINDOW_H
