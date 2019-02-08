#ifndef _SCANNERATTRIBUTES_H_INCLUDED_
#define _SCANNERATTRIBUTES_H_INCLUDED_ 1

#include <windows.h>
#include <string>

using namespace std;

struct ScannerAttributes
{

   int bufferSize;
   int colorSupport;
   int generation;
   int hasColor;
   int lineEnhancement;
   int centerLoad;
   int DSPModes;
   int maxDataRate;
   int minScanWidth;
   int maxScanWidth;
   int minSharpenSoften;
   int maxSharpenSoften;
   int thresholdModeSupport;
   int maxSetWindowLength;
   int colorBitWidthSupport;
   int graytoneBitWidthSupport;
   int minDpiX;
   int maxDpiX;

   int SizeOfGammaTable;
   string scannerId;
   string vendorId;

   BYTE NoOfCameras;
   string ProductID;

};

#endif
