#!/bin/bash

set -euo pipefail

USER=$1
HOSTNAME=$2
ARCH=$3
VERSION=$4

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR/..
rsync -av --delete $(pwd)/* $USER@$HOSTNAME:/tmp/slide_build
ssh $USER@$HOSTNAME "/tmp/slide_build/sbin/package.sh $ARCH $VERSION"
scp $USER@$HOSTNAME:/tmp/slide_build/make/slide_*.tar.gz .
