#!/usr/bin/bash
set -e

SDCARD=$1
if [ -z $SDCARD ] || [ $SDCARD = '-?' ] || [ $SDCARD = '-h' ] || [ $SDCARD = '--help' ]; then
    echo "Usage $0 sd_card_path"
    exit
fi
if [ $SDCARD = "/dev/sda" ]; then
    echo "You probably don't want me to reformat /dev/sda.  Exiting"
    exit
fi

echo "Testing whether SD card: $SDCARD exists ..."
if [ -b "$SDCARD" ]; then
    echo "Exists"
else
    echo "Does not exist"
    exit 1
fi

echo "Will create a snakepit-pi SD card image on SD card: $SDCARD"
echo "Hit enter to continue or Ctrl-C to exit"
read

# Do the build
echo "Building snakepit-pi files"
./build.sh

# Get the files out of the container image
echo "Retrieving files from container"
IMG_NAME=snakepit-pi-build
CONT_NAME=snakepit-pi-cont
FILES_DIR=/tmp/snakepit-pi-files
rm -fr $FILES_DIR
docker rm -f $CONT_NAME 2>/dev/null
mkdir $FILES_DIR
docker run -qd --name $CONT_NAME $IMG_NAME 1> /dev/null
docker cp -q $CONT_NAME:/output/ $FILES_DIR/
docker rm -f $CONT_NAME 1> /dev/null

# Install necessary packages for SD card formatting
sudo apt update && sudo apt install -y parted dosfstools

# Format the SD card
echo "Formatting SD card: $SDCARD"
sudo wipefs -q --all --force $SDCARD
sudo parted -s $SDCARD mklabel msdos
sudo parted -s $SDCARD mkpart primary fat32 1MiB 100%
sudo mkfs -t vfat ${SDCARD}1 1> /dev/null

# Mount the SD card and copy the files over
echo "Writing files to SD card: $SDCARD"
MNT_DIR=/tmp/snakepit-pi-sd
rm -fr $MNT_DIR
mkdir $MNT_DIR
sudo mount ${SDCARD}1 $MNT_DIR
sudo cp -r $FILES_DIR/output/* $MNT_DIR/
echo "All files created on SD Card: $SDCARD"
cd $MNT_DIR
find . -type f | xargs ls -ltr
sleep 1
sudo umount -l ${SDCARD}1
rm -r $MNT_DIR

# Remove the files
echo "Cleaning up"
rm -r $FILES_DIR

echo "Done"