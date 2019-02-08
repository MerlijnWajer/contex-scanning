#ifndef _BMPDATA_H_INCLUDED_
#define _BMPDATA_H_INCLUDED_ 1

#include <windows.h>

class CBmpData 
{
public:
   CBmpData(char * Filename);
   ~CBmpData();
   void AddData(const BYTE *pData, ULONG nBytes);
   void SetLineInfo(const BYTE BytePerPixel, const ULONG BytesPerLine);
   ULONG GetNumberOfScanLines() { return m_NumberOfLines; };

private:
   void WriteHeader();

   bool m_bAnyFile;
   FILE * m_fp;
   BYTE m_BytesPerPixel;
   ULONG m_PixelsPerLine;
   ULONG m_ExtraBytes;
   ULONG m_NumberOfLines;
   unsigned int m_headers[13];
   RGBQUAD m_palette[256];
   ULONG   m_paletteSize;
};

#endif