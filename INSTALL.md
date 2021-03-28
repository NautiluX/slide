# Installing slide

## Install dependencies

This project depends on the following dynamically linked libraries:

* qt5
* libexif
* libheif

### OSX

```
brew install qt libexif
```

### Raspbian Stretch

```
brew install qt5 libexif12
```

## Extract binaries

```
tar xf slide_<arch>_<version>.tar.gz
```

## Move binary to executable folder

### OSX

```
mv slide_<version>/slide.app/Contents/MacOS/slide /usr/local/bin/
```

### Linux

```
mv slide_<version>/slide /usr/bin/
```
