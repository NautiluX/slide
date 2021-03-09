# slide

Simple, lightweight slideshow selecting random images from specified directory. This slideshow is designed to compile and run also on a Raspberry Pi, for example to turn it into a digital picture frame.

Tested versions: 
 * Raspberry Pi 3 running Raspbian Stretch.
 * Raspberry Pi 3 running Raspbian Buster.

Screen background is filled with a scaled version of the image to prevent pure black background.


![screenshot](doc/screen.jpg)

This project is maintained by myself during my spare time. If you like and use it, consider [buying me a coffee](https://www.buymeacoffee.com/nautilux).

## Usage

```
slide [-t rotation_seconds] [-o background_opacity(0..255)] [-b blur_radius] -p image_folder [-r] [-O overlay_string]
```

* `image_folder`: where to search for images (.jpg files)
* `-r` for recursive traversal of `image_folder`
* `-s` for shuffle instead of random image rotation
* `-S` for sorted rotation (files ordered by name, first images then subfolders)
* `rotation_seconds(default=30)`: time until next random image is chosen from the given folder
* `background_opacity(default=150)`: opacity of the background filling image between 0 (black background) and 255
* `blur_radius(default=20)`: blur radius of the background filling image
* `-O` is used to create a overlay string.
  * It defines overlays for all four edges in the order `top-left;top-right;bottom-left;bottom-right`
  * All edges overlays are separated by `;`
  * Each edge can either be just a test or contain formatting in the form `margin|fontsize|text`
  * the text can contain special strings which are replaced during rendering:
    * `<time>` current time
    * `<date>` current date
    * `<datetime>` current time and date
    * `<exifdatetime>` time stamp from the EXIF data of the image
    * `<filename>` filename of the current image
    * `<basename>` basename of the current image (without suffix)
    * `<filepath>` filename including the path of the current image
    * `<dir>`directory of the current image
    * `<path>`path to the current image without filename
  * Example: `slide -p ./images -O "20|60|Time: <time>;;;Picture taken at <exifdatetime>"`
      
## Dependencies

* libexif
* qt5
* qt5-image-formats-plugins for tiff
```
sudo apt install libexif12 qt5-default
sudo apt install qt5-image-formats-plugins
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
