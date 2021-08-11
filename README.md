# slide

Simple, lightweight slideshow selecting random images from specified directory. This slideshow is designed to compile and run also on a Raspberry Pi, for example to turn it into a digital picture frame.

Tested versions: 
 * Raspberry Pi 3 running Raspbian Stretch.
 * Raspberry Pi 3 running Raspbian Buster.
 * Raspberry Pi Zero running Raspbian Buster.
 * Raspberry Pi 4B running Raspbian Buster.

Screen background is filled with a scaled version of the image to prevent pure black background.


![screenshot](doc/screen.jpg)

This project is maintained by myself during my spare time. If you like and use it, consider [buying me a coffee](https://www.buymeacoffee.com/nautilux).

## Usage

```
slide [-t rotation_seconds] [-a aspect] [-o background_opacity(0..255)] [-b blur_radius] [-p image_folder|-i imageFile,...] [-r] [-O overlay_string] [-v] [--verbose] [--stretch] [-c path_to_config_json]
```

* `image_folder`: where to search for images (.jpg files)
* `-i imageFile,...`: comma delimited list of full paths to image files to display
* `-c path_to_config_json`: the path to an optional slide.options.json file containing configuration parameters
* `-t` how many seconds to display each picture for
* `-r` for recursive traversal of `image_folder`
* `-s` for shuffle instead of random image rotation
* `-S` for sorted rotation (files ordered by name, first images then subfolders)
* `rotation_seconds(default=30)`: time until next random image is chosen from the given folder
* `aspect(default=a)`: the required aspect ratio of the picture to display. Valid values are 'a' (all), 'l' (landscape), 'p' (portrait) and 'm' (monitor). Monitor will match the aspect ratio of the display we are running on.
* `background_opacity(default=150)`: opacity of the background filling image between 0 (black background) and 255
* `blur_radius(default=20)`: blur radius of the background filling image
* `-v` or `--verbose`: Verbose debug output when running, plus a thumbnail of the original image in the bottom left of the screen
* `--stretch`: When in aspect mode 'l','p' or 'm' crop the image rather than leaving a blurred background. For example, in landscape mode this will make images as wide as the screen and crop the top and bottom to fit.
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

To exit the application, press escape. If you're using a touch display, touch all 4 corners at the same time.

## Configuration file
Slide supports loading configuration from a JSON formatted file called `slide.options.json`. This file can be specified by the `-c` command line option, we will also attempt to read `~/.config/slide/slide.options.json` and `/etc/slide/slide.options.json` in that order. The first file to load is used and its options will override command line parameters.
The file format is:
```
{
   "path" : "/path/to/pictures",
   "aspect" : "m",
   "overlay" : "20|20|<filename>",
   "shuffle" : true,
   "recursive" : true,
   "sorted" : false,
   "stretch": false,
   "rotationSeconds" : 300,
   "opacity" : 200,
   "debug" : false
}
```
Supported keys and values in the JSON configuration are:
* `path` : where to search for images (.jpg files)
* `aspect` : the same as the command line argument
* `overlay` : the same as the overlay command line argument
* `shuffle` : set to true to enable shuffle mode for file display
* `recursive` : set to true to enable recursive mode for file display
* `sorted` : set to true to enable shuffle mode for file display
* `stretch` : set to true to enable, the same as the `--stretch` command line argument
* `rotationSeconds` : the same as the `-t` command line argument
* `opacity` : the same as the command line `-o` argument
* `blur` : the same as the command line `-b` argument
* `debug` : set to true to enable verbose output from the program

## Folder Options file
When using the default or recursive folder mode we support having per folder display options. The options are stored in a file called "options.json" and currently support the following option
```
{
    "fitAspectAxisToWindow": false
}
```
* `fitAspectAxisToWindow` : apply the `--stretch` option to files in this folder

## Dependencies

* qt5-qmake
* qt5
* qt5-image-formats-plugins
* libexif

Ubuntu/Raspbian:

```
sudo make install-deps-deb
```

## Build

Install dependencies

```
make install-deps-deb
```

Build project

```
make
```

Install binaries

```
sudo make install
```

### macOS

Prerequisite: brew

```
brew install qt5
brew install libexif
brew install libexif
make
```

## Article on using slides

```
This article has more helpful ways that you could use this repo as a picture frame
https://opensource.com/article/19/2/wifi-picture-frame-raspberry-pi
```

## Removing black border (Raspberry Pi)

```
if you find that you have a black border around your screen you can remove it by disabling overscan. This is done by editing /boot/config.txt and uncommenting disable_overscan=1 
```
