# SWFcheck
An under development cli swf verification program based on github.com/flash-computer/libswftag

## Build Instructions:

1. Download the libswftag source code or clone the repository from [the repo](https://github.com/flash-computer/libswftag)

```sh
git clone https://github.com/flash-computer/libswftag
```

2. Make sure you have a version of make and a C compiler adhering to atleast c99 installed. The code is tested primarily with the GNU compiler collection (gcc) and GNU make, but other should work too

3. cd into the directory and run make and wait for the **ALL DONE!** message

```sh
cd libswftag
make
```

4. A new folder named `build` will be created inside. move the `include` and `lib` folders inside to the include and lib directories of your machine. For linux, it's usually `/usr/include/` and `/var/lib/`. Further, we'll alias these as $INCLUDE and $LIB.

5. Download or clone [the swfcheck repo](https://github.com/flash-computer/swfcheck)

```sh
git clone https://github.com/flash-computer/swfcheck
```

6. cd into the directory and compile with the following options:

```sh
cc swfcheck.c error.c -L$LIB -lswftag -Wall -std=c99 -o swfcheck
```

7. This will create an executable named `swfcheck` (followed by an extension like .exe if your OS mandates that for executables) in the current directory. You can copy this somewhere onto your path and use it.

```sh
swfcheck -h
```
