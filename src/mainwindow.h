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
class ImageSwitcher;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent* event) override;
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    ~MainWindow();
    void setImage(const ImageDetails &imageDetails);
    void setBlurRadius(unsigned int blurRadius);
    void setBackgroundOpacity(unsigned int opacity);
    void setTransitionTime(unsigned int transitionSeconds);
    void warn(std::string text);
    void setOverlay(std::unique_ptr<Overlay> &overlay);
    void setBaseOptions(const ImageDisplayOptions &baseOptionsIn);
    const ImageDisplayOptions &getBaseOptions();
    void setImageSwitcher(ImageSwitcher *switcherIn);
    void setOverlayHexRGB(QString overlayHexRGB);
public slots:
    void checkWindowSize();
private:
    Ui::MainWindow *ui;

    unsigned int blurRadius = 20;
    unsigned int backgroundOpacity = 150;
    ImageDisplayOptions baseImageOptions;
    bool imageAspectMatchesMonitor = false;
    ImageDetails currentImage;
    QSize lastScreenSize = {0,0};
    QString overlayHexRGB = "#FFFF";
    unsigned int transitionSeconds = 1;

    std::unique_ptr<Overlay> overlay;
    ImageSwitcher *switcher = nullptr;

    void drawText(QPixmap& image, int margin, int fontsize, QString text, int alignment);

    void updateImage();
    int getImageRotation();

    QPixmap getBlurredBackground(const QPixmap& originalSize, const QPixmap& scaled);
    QPixmap getRotatedPixmap(const QPixmap& p);
    QPixmap getScaledPixmap(const QPixmap& p);
    void drawBackground(const QPixmap& originalSize, const QPixmap& scaled);
    void drawForeground(QPixmap& background, const QPixmap& foreground);
    QPixmap blur(const QPixmap& input);
};

#endif // MAINWINDOW_H
