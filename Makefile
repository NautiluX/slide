VERSION ?= 0.0.0
BINARY ?= slide
ARCH ?= noarch

.PHONY: all
all: build

.PHONY: install-deps-deb
install-deps-deb:
	apt install qt5-qmake libexif12 qt5-default libexif-dev qt5-image-formats-plugins libheif1 libheif-dev cmake

check-deps-deb:
	dpkg -l | grep qt5-qmake
	dpkg -l | grep libexif12
	dpkg -l | grep libexif-dev
	dpkg -l | grep qt5-default
	dpkg -l | grep qt5-image-formats-plugins
	dpkg -l | grep libheif1
	dpkg -l | grep libheif-dev
	dpkg -l | grep cmake

.PHONY: clean
clean:
	rm -rf build

build:
	mkdir -p build
	qmake src/slide.pro -o build/Makefile
	make -C build

PACKAGE_DIR=build/slide_$(VERSION)

.PHONY: package
package: clean build
	mkdir -p $(PACKAGE_DIR)
	cp -r "./build/$(BINARY)" $(PACKAGE_DIR)
	cp "INSTALL.md" $(PACKAGE_DIR)
	cp "LICENSE" $(PACKAGE_DIR) 
	cd build && tar cfz slide_$(ARCH)_$(VERSION).tar.gz slide_$(VERSION)
