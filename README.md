# snakepit-pi

A port of Snake Pit by Mike Singleton to the Raspberry Pi.  Written to run bare metal on the Pi in C++ using [Circle](https://github.com/rsta2/circle).

## Quick-start

To automatically create an SD card with the necessary files you need to:
* Insert an SD card (e.g. `/dev/sdb`)
* Run `./create_sd.sh /dev/sdb`

This will build all the files, and wipe the SD card, and flash all the necessary files for all versions of the Raspberry Pi.

Note - anything on the SD card will be lost.

## Building

First get the ARM GNU toolchains for your target(s).  These instructions install both AArch32 and AArch64 target toolchains on x86_64:

```
cd ~/builds
wget "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz?rev=a05df3001fa34105838e6fba79ee1b23&hash=D63F63D13F01626D207019956E7122B5" -O arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz
unxz arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz
tar xf arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar
rm arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar
wget "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz?rev=e434b9ea4afc4ed7998329566b764309&hash=CA590209F5774EE1C96E6450E14A3E26" -O arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
unxz arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar
rm arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar
```

Now get, configure and build Circle (this builds circle libraries for the Pi Zero and 1)

```
sudo apt install build-essential
cd ~/builds
git clone https://github.com/rsta2/circle
cd circle
PATH=$PATH:~/builds/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin ./configure -f -r 1 -p arm-none-eabi- --keymap UK
PATH=$PATH:~/builds/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin ./makeall clean
PATH=$PATH:~/builds/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin ./makeall
```

Finally, you can build snakepit-pi:

```
cd ~/builds
git clone https://github.com/piersfinlayson/snakepit-pi
cd snakepit-pi
PATH=$PATH:~/builds/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin make
```

