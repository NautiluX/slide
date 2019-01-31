#!/bin/bash


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ARCH=$1
VERSION=$2

set -euo pipefail

if [[ "$ARCH" == "" ]]; then
    echo "missing argument target_architecture"
    exit 1
fi
if [[ "$VERSION" == "" ]]; then
    echo "missing argument version"
    exit 1
fi

BINARY=slide
if [[ "$ARCH" == "osx" ]]; then
    BINARY=slide.app
fi

cd "$DIR/.."
mkdir -p make/slide_$VERSION
cd make
qmake ../src/slide.pro
make
cp -r "$BINARY" "slide_$VERSION/"
cp "../INSTALL.md" "slide_$VERSION/"
cp "../LICENSE" "slide_$VERSION/"
tar cfz slide_${ARCH}_$VERSION.tar.gz "slide_$VERSION"
