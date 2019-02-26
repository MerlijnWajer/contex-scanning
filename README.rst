Contex scanning on Linux
========================

The Contex IQ FLEX has only Windows drivers:

* TWAIN driver
* Low level driver

The TWAIN driver internally calls a program called *Nextimage.exe*, which is a
huge .NET program that doesn't run in WINE or Mono. It has a lot of supporting
libraries and generally doesn't seem to support headless scanning. It wraps .NET
functions around *GS.Task.dll*, which is a high level task manager to
*CtxScan64.dll* and *CtxSws64.dll*, which perform the actual scanning.

The low level driver is called *ctx_scan_2000.dll* (and *ScanWing.dll*), and is
shipped with the *WIDEsystem* "driver". The *WIDEsystem* driver also contains a
GUI to find/add scanners, called *WS.exe* - but this is in no way required to
actually use the drivers. The scanners are stored in
*C:\ProgramData\Contex\WIDEsystem\Settings.ini* (or, in WINE, in:
*C:\ProgramData\Settings.ini*) and can be tweaked manually or defined
programmatically in that file. The *ctx_scan_2000.dll* API will also require
this file to be present, otherwise it will not initialise. It also requires
*CtxWia64.dll* to be present in *C:\Windows\System32* but doesn't seem to
actually use it.

Using the Contex SDK "example scan code", it is possible to perform headless
scans. Their example builds on Visual Studio 2017 on Windows. I've taken the
code and made it work with MinGW and a Makefile. Set *WRITEFILE* to true if you
want it to save the bitmap, but be aware that it's **REALLY SLOW** at writing
bitmaps, so do not use this for any kind of benchmarking.

Currently, *simplescan.exe* is built by the Makefile, and it supports the
following options/features:

* *-d* set DPI
* *-w* set width in millimeters
* *-h* set height in millimeters
* *-l* set left-offset in millimeters
* *-t* set top-offset in millimeters
* *-s* save image (boolean, will print to default stdout)
* *-f* save image to file instead of stdout
* *-i* embed colour profile from file, default is no profile
* *-i* name for embedded colour profile (default is
  "sRGB (Contex IQ Quattro 24/44, IQ FLEX)")

There's more work to be done, see the `TODO`_ section.


Building
========

Install toolchain:

* *gcc-mingw-w64*
* *g++-mingw-w64*

Get/Download:

* dlls from *dlls* branch

Type *make*.


WINE
----

Install wine, set up a wine prefix, like so::

    sudo apt install wine-stable
    mkdir -p ~/wines/contex
    export WINEPREFIX=/home/USERHERE/wines/contex
    winecfg
    # Create Settings.ini in $WINEPREFIX/drive_c/ProgramData/Settings.ini
    # Copy CtxWia64.dll to $WINEPREFIX/drive_c/windows/system32
    # It looks like it is not used, but it has to be there(?)

Running
=======


(Coords may not be entirely accurate)

To scan a front of LP artwork::

    wine simplescan.exe -d 100 -h 309 -w 312 -l 175 -t 0

To scan a gatefold::

    wine simplescan.exe -d 100 -h 618 -w 312 -l 175 -t 0

To scan a full bed::

    wine simplescan.exe -d 600 -h 635 -w 487 -l 0 -t 0


You might have to set WINEPATH to a location that contains the mingw dlls,
or we can just ship them in the dlls/ directory in the future.

You will also need zlib and libpng; for example from here:
http://win-builds.org/1.5.0/packages/windows_32/ (use windows_64 if you need 64
bit builds)


Documentation
=============

There is some Contex documentation in the `documentation` branch.


Settings.ini
============

Example::

    [General]
    USBdriverVersion=49877
    DisableAutoInstall=1
    SurveyDate=2019-03-24
    [Ethernet]
    ReservationKey=94,c6,91,a6,7f,d3
    [RemoteScanners]
    Use=1,
    [Scanner1]
    IP=127.0.0.1
    Port=58170
    HostName=127.0.0.1
    Model=
    Serial=
    Name=Network scanner at 127.0.0.1

TODO
====

* Load/package their sRGB ICC and add it
* 'No image data!' is sometimes written by ctx_scan_2000.dll (esp. with 1200
  DPI) and then we lack some image data. Need to debug what happens there.
