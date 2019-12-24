#!/bin/bash

set -euo pipefail

# This file should be executed in a docker container running ubuntu

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

IMG=$(ls $DIR/*.img)

BOOT_SECTOR=$(fdisk -l $IMG | grep .img1 | awk '{ print $2 }')
BOOT_OFFSET=$(expr 512 '*' "$BOOT_SECTOR")
ROOT_SECTOR=$(fdisk -l $IMG | grep .img2 | awk '{ print $2 }')
ROOT_OFFSET=$(expr 512 '*' "$ROOT_SECTOR")

echo "$BOOT_SECTOR -> $BOOT_OFFSET"
echo "$ROOT_SECTOR -> $ROOT_OFFSET"

mkdir -p /mnt/rasp_boot

mount -v -o offset=$BOOT_OFFSET -t vfat $IMG /mnt/rasp_boot
echo "" > /mnt/rasp_boot/ssh
umount /mnt/rasp_boot

mkdir -p /mnt/rasp_root
mount -v -o offset=$ROOT_OFFSET -t ext4 $IMG /mnt/rasp_root
grep ext4 /mnt/rasp_root/etc/fstab | awk '{ print $1 }' | sed 's/PARTUUID=//' > $DIR/partuuid
:> /mnt/rasp_root/etc/ld.so.preload
umount /mnt/rasp_root

