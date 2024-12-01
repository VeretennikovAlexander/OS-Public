#!/bin/bash

mkdir /opt/temp
mkdir /opt/temp/cpp
mkdir /opt/temp/cpp/fat32

modprobe brd rd_nr=1 rd_size=$((4 * 1048576))
mkfs -t vfat -F 32 /dev/ram0
mount /dev/ram0 /opt/temp/cpp/fat32

echo Test > /opt/temp/cpp/fat32/1.txt

df -h -T

