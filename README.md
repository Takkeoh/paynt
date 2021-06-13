# Payn't
## _The crappy clone of Paint_

## Features

- Drawing on a surface
- 3 working layers !!
- Save to .png
- and some ✨magic ✨

## Tech

Dillinger uses a number of open source projects to work properly:

- [GTK+] - GTK is a free and open-source cross-platform widget toolkit for creating graphical user interfaces (GUIs).
- [Glade] - Glade is a RAD tool that enables quick and easy development of user interfaces for the GTK toolkit and the GNOME desktop environment.
- [Cairo Graphics] - Cairo is an open source graphics library that provides a vector graphics-based, device-independent API .


And of course Paynt itself is open source with a [public repository](https://github.com/Takkeoh/paynt) on GitHub.

## Installation

This project uses gcc to compile, be sure to have it installed. You can check if you have gcc using

```sh
$ gcc --version
gcc (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

If you don't have gcc installed you can install it using

```sh
$ sudo apt-get update
$ sudo apt-get install build-essential
```

Be sure you have all the required librairies installed

If you are missing some librairies use these commands

GTK+
```sh
$ sudo apt-get install libgtk-3-dev
```

Cairo Graphics
```sh
$ sudo apt-get install libcairo2-dev
```


## Compilation

To compile the project, a makefile is included so you only need to run this in your terminal
```sh
$ make
gcc `pkg-config --cflags gtk+-3.0` -Wall -O3   -c -o paynt.o paynt.c
gcc `pkg-config --cflags gtk+-3.0` -Wall -O3   -c -o tools.o tools.c
gcc   paynt.o tools.o  `pkg-config --libs gtk+-3.0` -o paynt
```

A clean method is included as well
```sh
$ make clean
rm -f paynt
rm -f *.o
```

To run the project use this command as following
```
./paynt <filename>
```

## License

GNU General Public License v3.0

**Free Software, Hell Yeah!**

