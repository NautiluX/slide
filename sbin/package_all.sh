#!/bin/bash
set -euxo pipefail
VERSION=$1
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

make clean
$DIR/package_remote.sh pi picframe pi $VERSION
$DIR/package_remote.sh pi raspberrypi pi-0 $VERSION
$DIR/package.sh amd64 $VERSION
mv build/slide_amd64_$VERSION.tar.gz .
