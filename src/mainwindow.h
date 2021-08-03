#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include "imagestructs.h"
#include "imageselector.h"

namespace Ui {
class MainWindow;
}
class QLabel;
class QKeyEvent;
class Overlay;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent* event) override;
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    ~MainWindow();
    void setImage(const ImageDetails_t &imageDetails);
    void setBlurRadius(unsigned int blurRadius);
    void setBackgroundOpacity(unsigned int opacity);
    void warn(std::string text);
    void setOverlay(Overlay* overlay);
    void setDebugMode(bool debugModeIn) {debugMode = debugModeIn;}
    void setBaseOptions(const ImageDisplayOptions_t &baseOptionsIn) { baseImageOptions = baseOptionsIn; }
    const ImageDisplayOptions_t &getBaseOptions() { return baseImageOptions; }
private:
    Ui::MainWindow *ui;

    unsigned int blurRadius = 20;
    unsigned int backgroundOpacity = 150;
    ImageDisplayOptions_t baseImageOptions;
    ImageDetails_t currentImage;
    bool debugMode = false;

    Overlay* overlay;

    void drawText(QPixmap& image, int margin, int fontsize, QString text, int alignment);

    void updateImage(bool immediately);
    int getImageRotation();

    QPixmap getBlurredBackground(const QPixmap& originalSize, const QPixmap& scaled);
    QPixmap getRotatedPixmap(const QPixmap& p);
    QPixmap getScaledPixmap(const QPixmap& p);
    void drawBackground(const QPixmap& originalSize, const QPixmap& scaled);
    void drawForeground(QPixmap& background, const QPixmap& foreground);
    QPixmap blur(const QPixmap& input);
};

#endif // MAINWINDOW_H
