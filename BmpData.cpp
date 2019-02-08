#include "StdAfx.h"
#include "BmpData.h"
 

CBmpData::CBmpData(char * Filename)
{
   m_fp = 0;               
   if (*Filename)
    fopen_s(&m_fp, Filename, "wb");
   m_bAnyFile = (m_fp != 0);
   m_NumberOfLines = 0;
   m_paletteSize = 0;
}

CBmpData::~CBmpData()
{
   if (m_bAnyFile)
   {
      WriteHeader();
      fclose(m_fp);
      m_fp = 0;
      m_bAnyFile = false;
   }
   m_NumberOfLines = 0;
}

void CBmpData::SetLineInfo(const BYTE BytesPerPixel, const ULONG PixelsPerLine)
{
   m_BytesPerPixel = BytesPerPixel;
   m_PixelsPerLine = PixelsPerLine;
   m_ExtraBytes = 4 - ((m_PixelsPerLine * BytesPerPixel) % 4);         
   if (m_ExtraBytes == 4)
      m_ExtraBytes = 0;
   if (m_bAnyFile)
   {
      char dummyHeader[54];
      memset(dummyHeader,0,sizeof(dummyHeader));
      memcpy(dummyHeader,"BM",2);
      for (int i=0; i < sizeof(dummyHeader); i++)
         fprintf(m_fp,"%c",dummyHeader[i]);

      if (BytesPerPixel == 1)
      {
         m_paletteSize = 256;
         for (ULONG i = 0; i < m_paletteSize; i++)
            m_palette[i].rgbRed = m_palette[i].rgbGreen = m_palette[i].rgbBlue = (BYTE)i;
         fwrite(m_palette,sizeof(RGBQUAD), m_paletteSize, m_fp);
      }
   }

}

void CBmpData::AddData(const BYTE *pData, ULONG nBytes)
{
   if (!m_bAnyFile)
      return;

   ULONG lines = nBytes / (m_PixelsPerLine * m_BytesPerPixel); 
   if (lines*m_PixelsPerLine*m_BytesPerPixel != nBytes)
      printf("*** Line Length Error\n");
   m_NumberOfLines += lines;


   BYTE red, green, blue;
   BYTE* ptr = (BYTE*)pData;
   for (ULONG y = 0; y < lines; y++)
   {
      if (m_BytesPerPixel == 3)                          // Color
      {
         for (ULONG x = 0; x < m_PixelsPerLine; x++)
         {
            red   = *ptr++;
            green = *ptr++;
            blue  = *ptr++;
            fprintf(m_fp, "%c%c%c", blue, green, red);
         }
      }
      else
      {                                                  // Grey
         fwrite(ptr,sizeof(BYTE),m_PixelsPerLine,m_fp);     
         ptr += m_PixelsPerLine;
      }

      for (ULONG n = 1; n <= m_ExtraBytes; n++)
         fprintf(m_fp, "%c", 0);
   }
}

void CBmpData::WriteHeader()
{
   int paddedsize;
   int n;
   paddedsize = ((m_PixelsPerLine * m_BytesPerPixel) + m_ExtraBytes) * m_NumberOfLines;

   m_headers[0]  = paddedsize + 54 + m_paletteSize*sizeof(RGBQUAD);     // whole file size
   m_headers[1]  = 0;                    // Reserved 
   m_headers[2]  = 54+ m_paletteSize*sizeof(RGBQUAD);
   m_headers[3]  = 40;                   // 
   m_headers[4]  = m_PixelsPerLine;      
   m_headers[5]  = -((long)m_NumberOfLines); 

   m_headers[7]  = 0;                    // Compression
   m_headers[8]  = paddedsize;           // SizeImage
   m_headers[9]  = 0;                    // XPelsPerMeter
   m_headers[10] = 0;                    // YPelsPerMeter
   m_headers[11] = 0;                    // ClrUsed
   m_headers[12] = 0;                    // ClrImportant

   fseek(m_fp,0,SEEK_SET);               // Beginning of file
   fprintf(m_fp, "BM");

   for (n = 0; n <= 5; n++)
      fprintf(m_fp, "%c%c%c%c", m_headers[n] & 0x000000FF, (m_headers[n] & 0x0000FF00) >> 8,
                                (m_headers[n] & 0x00FF0000) >> 16, (m_headers[n] & (unsigned int) 0xFF000000) >> 24);

   // Handle Planes and BitCount:
   fprintf(m_fp, "%c%c%c%c", 1, 0, 8*m_BytesPerPixel, 0);

   for (n = 7; n <= 12; n++)
      fprintf(m_fp, "%c%c%c%c", m_headers[n] & 0x000000FF, (m_headers[n] & 0x0000FF00) >> 8,
      (m_headers[n] & 0x00FF0000) >> 16, (m_headers[n] & (unsigned int) 0xFF000000) >> 24);
}
