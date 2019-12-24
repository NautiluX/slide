#!/bin/bash

set -euxo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR

if [[ ! -f rasp.zip ]]; then
  wget https://downloads.raspberrypi.org/raspbian_lite_latest -O rasp.zip
  unzip rasp.zip
fi

IMG=$(ls $DIR/*.img)

if [[ ! -f kernel-buster ]]; then
  wget https://github.com/dhruvvyas90/qemu-rpi-kernel/raw/master/kernel-qemu-4.19.50-buster -o kernel-buster
fi

if [[ ! -f versatile-pb.dtb ]]; then
  wget https://github.com/dhruvvyas90/qemu-rpi-kernel/raw/master/versatile-pb.dtb -O versatile-pb.dtb
fi

docker run --privileged --rm -v $DIR:/dist --entrypoint=/dist/patch.sh ubuntu:latest

PARTUUID=$(cat partuuid)
qemu-system-arm -M versatilepb -cpu arm1176 -m 256M -drive file=$IMG,format=raw,if=sd -net nic -net user,hostfwd=tcp::5022-:22 \
  -dtb $(pwd)/versatile-pb.dtb -kernel $(pwd)/kernel-buster \
  -append "rw console=ttyAMA0 root=PARTUUID=$PARTUUID rootfstype=ext4 loglevel=8 rootwait fsck.repair=yes memtest=1 rdinit=/sbin/init panic=1" \
  -serial stdio -no-reboot \
   -vnc :1
