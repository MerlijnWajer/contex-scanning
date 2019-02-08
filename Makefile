.PHONY: default clean

CFLAGS+=-std=c++11 -Wall -Wextra
LDFLAGS+=-lctx_scan_2000 -L.
CC=i686-w64-mingw32-g++

SCAN_OS=BmpData.cpp  SimpleScan.cpp  StdAfx.cpp
SCAN_HS=BmpData.h ScannerAttributes.h SetWindowParams.h StdAfx.h utils.h


default: simplescan.exe

simplescan.exe: $(SCAN_HS) $(SCAN_OS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
