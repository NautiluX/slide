# slide

Simple, lightweight slideshow selecting random images from specified directory. This slideshow is designed to compile and run also on a Raspberry Pi, for example to turn it into a digital picture frame.

Tested versions: 
 * Raspberry Pi 3 running Raspbian Stretch.
 * Raspberry Pi 3 running Raspbian Buster.

Screen background is filled with a scaled version of the image to prevent pure black background.


![screenshot](doc/screen.jpg)
## Usage

```
slide [-t rotation_seconds] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder -r
```

* `image_folder`: where to search for images (.jpg files)
* `-r` for recursive traversal of `image_folder`
* `-s` for shuffle instead of random image rotation
* `-S` for sorted rotation (files ordered by name, first images then subfolders)
* `rotation_seconds(default=30)`: time until next random image is chosen from the given folder
* `background_opacity(default=150)`: opacity of the background filling image between 0 (black background) and 255
* `blur_radius(default=20)`: blur radius of the background filling image

## Dependencies

* libexif
* qt5

```
sudo apt install libexif12 qt5-default
```

## Build

dev libs needed to build slide on from source:

```
sudo apt install libexif-dev
```

```
mkdir -p make
cd make
qmake ../src/slide.pro
make
sudo make install
```

### macOS

Prerequisite: brew

```
brew install qt5
brew install libexif
mkdir -p build
cd build
qmake ../src/slide.pro
make
```

## Article on using slides

```
This article has more helpful ways that you could use this repo as a picture frame
https://opensource.com/article/19/2/wifi-picture-frame-raspberry-pi
```

## Removing black border

```
if you find that you have a black border around your screen you can remove it by disabling overscan. This is done by editing /boot/config.txt and uncommenting disable_overscan=1 
```
