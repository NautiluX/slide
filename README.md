# slide
Simple slideshow showing random images from specified directory

## Usage

```
slide /path/to/images
```

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
