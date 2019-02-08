#ifndef _SETWINDOWPARAMS_H_INCLUDED_
#define _SETWINDOWPARAMS_H_INCLUDED_ 1

#include <windows.h>
#include "ctx_scan_2000.h" // Include CONTEX SDK header filer

#include "utils.h"

#define WINDOW_DESC_OFFSET 8

#pragma pack(push,1) // Force it to be byte-aligned and -sized
class SETWINDOWPARAMS 
{
public:
   SETWINDOWPARAMS(void){};

public:
   // ---- Parameter list header
   BYTE m_reservedA;
   BYTE m_reservedB;
   BYTE m_reservedC;
   BYTE m_reservedD;
   BYTE m_reservedE;
   BYTE m_reservedF;
   WORD m_ParameterListLength;
   // ---- Parameter list
   BYTE m_wid;
   BYTE m_reserved1;
   WORD m_dpix;
   WORD m_dpiy;
   DWORD m_upperLeftX;
   DWORD m_upperLeftY;
   DWORD m_width;
   DWORD m_length;
   BYTE m_reserved22;
   BYTE m_threshold;
   BYTE m_reserved24;
   BYTE m_imageComposition;
   BYTE m_bitsPerPixel;
   BYTE m_reserved27;
   BYTE m_reserved28;
   BYTE m_rif;
   BYTE m_reserved30;
   BYTE m_reserved31;
   BYTE m_compressionType;
   BYTE m_reserved33;
   BYTE m_reserved34;
   BYTE m_reserved35;
   BYTE m_reserved36;
   BYTE m_reserved37;
   BYTE m_reserved38;
   BYTE m_reserved39;
   BYTE m_dynamicThreshold;
   BYTE m_lineEnhancement;
   BYTE m_scanDirection;
   BYTE m_enableDspThresholding;
   DWORD m_lineLimit;
   DWORD m_bufferLimit;
   BYTE m_colorComposition;
   BYTE m_reserved53;
   BYTE m_reserved54;
   BYTE m_reserved55;
   BYTE m_dspBackgroundLevel;
   BYTE m_dspAdaptiveLevel;
   BYTE m_dspConstant;
   BYTE m_reserved59;
   BYTE m_scanSpeed;
   BYTE m_useFeatureRam;
   BYTE m_blur;
   BYTE m_reserved63;
   BYTE m_5GThresholdMode;
   BYTE m_5GAdaptiveLevel;
   BYTE m_sharpening;
   BYTE m_5GBkgndSuppression;
   BYTE m_ColorSpaceType; // NTSC (0) or sRGB (1)
   BYTE m_ColorSaturationLevel; // 0..255. 100 is neutral
   BYTE m_DataRateSelector;
   WORD m_SoftwareControlParameters;
   BYTE m_reserved73;
   BYTE m_PostScanOriginalHandling;
   
public:
   /****************************/
   // Call just after reading from
   // or writing to the scanner.
   // It converts from Little Endian
   // to Big Endian or vice versa.
   // (Call it twice in a row to
   //  end up with what you started
   //  out with).
   /****************************/
   void Convert(void)
   {
      m_ParameterListLength = MySwap(m_ParameterListLength);
      m_dpix = MySwap(m_dpix);
      m_dpiy = MySwap(m_dpiy);
      m_upperLeftX= MySwap(m_upperLeftX);
      m_upperLeftY= MySwap(m_upperLeftY);
      m_width= MySwap(m_width);
      m_length= MySwap(m_length);
      m_lineLimit= MySwap(m_lineLimit);
      m_bufferLimit= MySwap(m_bufferLimit);
   }
};
#pragma pack(pop)



#endif
