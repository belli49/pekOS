# OS project

This project contains a basic OS built upon the template of the "Meaty-Bones" Project on the OSdev Wiki (source: OSdev wiki: https://wiki.osdev.org/Creating_an_Operating_System).

The cross-compiler used was built with gcc 12.2.0 and binutils-2.40.

(Here I will only explain how to build the OS when using Ubuntu.
Other Linux distros should work with similar steps, but may require additional libraries to be installed.)

## Required packages
The following packages are required:

|libraries|
|---------|
|build-essential|
|bison|
|flex|
|libgmp3-dev|
|libmpc-dev|
|libmpfr-dev|
|texinfo|
|grub2|
|grub2-common|
|xorriso|
|mtools|
|gcc-multilib|

You can install them by running the following command:
```sh
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo grub2 grub2-common xorriso mtools gcc-multilib
```

## Building the compiler
First download and extract `gcc-12.2.0` and `binutils-2.40` to `$HOME/src/`.
We will then build and bootstrap gcc to build the cross-compiler for the OS.

### Preparation
```sh
export PREFIX="$HOME/opt/gcc-12.2.0"
```

### Binutils
```sh
cd $HOME/src
mkdir build-binutils
cd build-binutils
../binutils-2.40/configure --prefix="$PREFIX" --disable-nls --disable-werror
make -j5
make install
```

Adding -j5 (in case of a 4 core processor) to make use of threads to speed up the process.

### GCC
```sh
cd $HOME/src
 
# In new GCC versions, you can ask gcc to download the prerequisites
cd gcc-12.2.0
./contrib/download_prerequisites
cd $HOME/src # Returning the main src folder
 
mkdir build-gcc
cd build-gcc
../gcc-12.2.0/configure --prefix="$PREFIX" --disable-nls --enable-languages=c,c++
make -j5
make install
```

Then export the compiler to current shell session with
```sh
export PATH="$HOME/opt/gcc-12.2.0/bin:$PATH"
```

## Building the cross-compiler
We will then use the compiler created above to make a cross-compiler using the same GCC and binutils version.
The following assumes that the compiler will be located at `$HOME/opt/cross/`, else change the `PREFIX` env variable.
Also, do not forget to delete the `build-binutils` and `build-gcc` folders located at `$HOME/src/` before continuing.

Here, we are compiling to target i686-elf (common x86 architecture).

### Preparation
```sh
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

### Binutils
```sh
cd $HOME/src
 
mkdir build-binutils
cd build-binutils
../binutils-2.40/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j5
make install
```

### GCC
```sh
cd $HOME/src
 
# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
mkdir build-gcc
cd build-gcc
../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

Then, add to current shell session with
```sh
export PATH="$HOME/opt/cross/bin:$PATH"
```

## Buiding the OS iso
To build the OS iso, simply change `default-host-sh` to the path of the i686-elf cross-compiler to be used.

Then, you can run the following.

To clean the source tree:

```sh
./clean.sh
```

To install the system headers into system root:

```sh
./headers.sh
```

To build an ISO with the OS:

```sh
./iso.sh
```

To quickly run the ISO with qemu:

```sh
./qemu.sh
```
* To install qemu simply run `sudo apt install libvirt-daemon`
* To boot it in real hardware, install the ISO (do at your own risk).
