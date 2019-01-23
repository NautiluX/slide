# slide
Simple slideshow showing random images from specified directory.
Screen background is filled with a scaled version of the image to prevent pure black background.

## Usage

```
slide [-t rotation_seconds] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder
```

* `image_folder`: where to search for images (.jpg files), currently non-recursively
* `rotation_seconds(default=30)`: time until next random image is chosen from the given folder
* `background_opacity(default=150)`: opacity of the background filling image between 0 (black background) and 255
* `blur_radius(default=20)`: blur radius of the background filling image

## Dependencies

* libexif
* qt5

```
sudo apt install libexif-dev qtdeclarative5-dev-tools
```

## Build

```
mkdir -p make
cd make
qmake ../src/slide.pro
make
sudo make install
```
