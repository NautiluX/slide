#!/bin/bash


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export ARCH=$1
export VERSION=$2

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
if ! make check-deps-deb; then
  sudo make install-deps-deb
fi

make package
