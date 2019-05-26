#!/bin/bash

#create chroot directory, cd into it
mkdir new-root
cd new-root

#create lib directories and a place for the binary
mkdir lib
mkdir lib/x86_64-linux-gnu
mkdir lib64
mkdir bin

# copy over lib libraries
sudo cp /lib/x86_64-linux-gnu/libtinfo.so.5 ./lib/x86_64-linux-gnu
sudo cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/x86_64-linux-gnu
sudo cp /lib/x86_64-linux-gnu/libc.so.6 ./lib/x86_64-linux-gnu

# copy over 64-bit library
sudo cp /lib64/ld-linux-x86-64.so.2 ./lib64

# copy over bash executable
sudo cp /bin/bash bin

# run the bash executable within the chroot
cd ..
sudo chroot ./new-root /bin/bash
