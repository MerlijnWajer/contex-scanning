.PHONY: default clean

CFLAGS+=-std=c++11 -Wall -Wextra -O2 -Wno-clobbered
LDFLAGS+=-lctx_scan_2000 -lpng14 -L. -Ibins/windows_32/include/libpng14 -Ibins/windows_32/include -Lbins/windows_32/lib
#CC=x86_64-w64-mingw32-g++
CC=i686-w64-mingw32-g++

SCAN_OS=simplescan.cpp StdAfx.cpp
SCAN_HS=ScannerAttributes.h SetWindowParams.h StdAfx.h utils.h


default: simplescan.exe

simplescan.exe: $(SCAN_HS) $(SCAN_OS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f simplescan.exe
