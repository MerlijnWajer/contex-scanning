// SimpleScan.cpp : Defines the entry point for the console application.
/*-------------------------------------------------------------------
*                       SimpleScan.exe
*
* Minimal demonstration of how to open scanner library,
* find and open a scanner and issuing a simple scan.
*
* Set the SCANMODE define below for to select between on of the two scan modes:
*   'G' : Gray tone 
*   'C' : Color
*
* Set WRITETOFILE to true or false depending on whether you want file output
*
* Some scanner attributes are read from the pages inside the scanner.
*
* A 200 mm wide and 200 mm long is scanned i 200 dpi. 
*
*
*  The following routines are demonstrated:
*
*     scanCloseLib()
*     scanCloseScanner()
*     scanGetLastAsc()
*     scanGetNextScanner()
*     scanInquiry()
*     scanInquiryPage()
*     scanOpenLib()
*     scanOpenScanner()
*     scanObjectPosition()
*     scanRead()
*     scanScan()
*     scanSetWindow()
*     scanWriteBuffer()
*     scanGetErrorMessage()
*     scanGetScannerNameToDisplay()
*
*
* Last updated: 5 September 2016
-------------------------------------------------------------------*/

#include "StdAfx.h"
#include <conio.h>
#include <math.h>
#include <sys/stat.h>                  // stat
#include "utils.h"
#include "BmpData.h"
#include "ScannerAttributes.h"
#include "ctx_scan_2000.h"             // Include CONTEX SDK header filer
#include "SetWindowParams.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define SCANMODE 'C'
#define WRITETOFILE true

//
//  Global vars.
//
int                  g_errCode=S_OK;
int                  g_errFatal=0;
ScannerAttributes    g_ScanAttr;
bool                 g_UnitReserved=false;

int use_dpi = 1200;
int use_width = 487;
int use_height = 609;

void SetError(HSCANNER hs, int eco)
{

   if(eco==SCSI_STATUS_CHECK_CONDITION)
   {
      g_errCode=scanGetLastAsc(hs);

      unsigned int e=((unsigned int)g_errCode)>>12;
      g_errFatal=(e!=3);
      char msg[256];
      scanGetErrorMessage(g_errCode,msg,sizeof(msg));
      printf("Diagnostic message: %s\n",msg);
   }
   else
   {
      g_errCode=eco;
      g_errFatal=1;
   }
   if(g_errFatal)
   {
      printf("Fatal error encountered.\n\t Error code : 0x%X\n", g_errCode);
      printf("Check REQUEST SENSE in SCSI documentation for error.\n");
   }
   else if(g_errCode)
   {
      printf("Non-fatal error encountered.\n\t Error code : 0x%X\n", g_errCode);
      printf("Check REQUEST SENSE in SCSI documentation for error.\n");
   }
}

//
// Reader of scanner attributes
//
int ReadAttributes(HSCANNER hs)
{
   int rc;
   int listLength;
   BYTE inqPageBuffer[255];
   BYTE inqPagesSupBuffer[255];

   if(S_OK != (rc = scanInquiry(hs,inqPageBuffer, sizeof(inqPageBuffer))))
      return rc;

   // get name of vendor
   g_ScanAttr.vendorId.assign((const char *)inqPageBuffer+8,8);

   // get name of scanner
   g_ScanAttr.scannerId.assign((const char *)inqPageBuffer+16,16);

   // 
   // How many (and which) inquiry pages can be retrieved from scanner ? 
   // 
   scanInquiryPage(hs, inqPagesSupBuffer,sizeof(inqPagesSupBuffer),SCAN_INQUIRY_PAGE_PAGES_SUPPORTED);
   listLength = inqPagesSupBuffer[3];

   //
   // Get and parse all available pages 
   //
   for (int listIndex = 1; listIndex < listLength; listIndex++) 
   {
      if (S_OK == (rc = scanInquiryPage(hs,inqPageBuffer,sizeof(inqPageBuffer),inqPagesSupBuffer[4+listIndex]))) 
      {
         /* Switch on page code */
         switch (inqPageBuffer[1]) 
         {
            //
            // TODO: You should parse these pages completely in a real application
            //
         case SCAN_INQUIRY_PAGE_REVISION:
            g_ScanAttr.generation = GET_DWORD(inqPageBuffer,60);

            break;

         case SCAN_INQUIRY_PAGE_AUTO_CONFIG:

            g_ScanAttr.colorSupport         = GET_BYTE(inqPageBuffer, 8);
            g_ScanAttr.lineEnhancement      = GET_BYTE(inqPageBuffer, 9);
            g_ScanAttr.centerLoad           = GET_BYTE(inqPageBuffer, 11);
            g_ScanAttr.NoOfCameras          = GET_DWORD(inqPageBuffer,16);
            g_ScanAttr.bufferSize           = GET_DWORD(inqPageBuffer,24);
            g_ScanAttr.maxScanWidth         = GET_DWORD(inqPageBuffer,28);
            g_ScanAttr.DSPModes             = GET_DWORD(inqPageBuffer,40);
            g_ScanAttr.thresholdModeSupport = GET_DWORD(inqPageBuffer,51);
            g_ScanAttr.minSharpenSoften     = GET_CHAR(inqPageBuffer,55);
            g_ScanAttr.maxSharpenSoften     = GET_BYTE(inqPageBuffer,56);
            g_ScanAttr.maxDataRate          = GET_DWORD(inqPageBuffer,57);
            g_ScanAttr.minScanWidth         = GET_DWORD(inqPageBuffer,61);
            g_ScanAttr.maxSetWindowLength   = GET_BYTE(inqPageBuffer,66);

            if (inqPageBuffer[3] >= 133-4)
            {
               // Byte 133 and 134 were added at the same time
               g_ScanAttr.colorBitWidthSupport    = GET_BYTE(inqPageBuffer,133);
               g_ScanAttr.graytoneBitWidthSupport = GET_BYTE(inqPageBuffer,134);
            }
            else
            {
               // Old scanners does not have this information, i.e. only the lowest bit width
               g_ScanAttr.colorBitWidthSupport    = 0x01;
               g_ScanAttr.graytoneBitWidthSupport = 0x01;
            }

            break;

         case SCAN_INQUIRY_PAGE_VAR_RESOLUTION:
            g_ScanAttr.minDpiX              = GET_DWORD(inqPageBuffer,8);
            g_ScanAttr.maxDpiX              = GET_DWORD(inqPageBuffer,12);
            break;

         case SCAN_INQUIRY_PAGE_FIXED_RESOLUTION:
            break;
         case SCAN_INQUIRY_PAGE_COLOR:
            g_ScanAttr.SizeOfGammaTable     = GET_DWORD(inqPageBuffer, 36);
            break;
         case SCAN_INQUIRY_PAGE_CAMERA_POSITION:
            break;
         case SCAN_INQUIRY_PAGE_BOOT:
            break;
         case SCAN_INQUIRY_PAGE_CALIBRATION_SUPPORT:
            //          This inquiry page is not supported by old scanners
            //          g_ScanAttr.NoOfCameras = GET_BYTE(inqPageBuffer,14);
            break;
         case SCAN_INQUIRY_PAGE_DEVICE_INFORMATION:
            g_ScanAttr.ProductID.assign((const char *)inqPageBuffer+150,32);
            break;
         case SCAN_INQUIRY_PAGE_DISPLAY_INFORMATION:
            break;
         default:
            printf("Unknown page 0x%02x\n", (int)inqPagesSupBuffer[4+listIndex]);
            break;
         }
      } 
      else 
      {
         printf("Failed retrieving page 0x%02x\n", (int)inqPagesSupBuffer[4+listIndex]);
         return FALSE;
      }
   }


   return rc;
}

void MakeGammaTable(BYTE *pTab, int nReqSize, bool bUSeSRGB)
{
   int i;
   // Return a linear gamma table 
   for (i=0; i < nReqSize; i++){
      pTab[i] = 255*i/(nReqSize-1);
      if(bUSeSRGB)
         pTab[i] = (int)(0.5+255.0*(((double)pTab[i]/255.0<=0.0034)?(12.92*(double)pTab[i]/255.0):(1.055*pow(  (double)pTab[i]/255.0,1/2.4)-0.055)));

   }
}

//
// Small helper functions that cleans up before exit
//
int CloseAndExit(HSCANNER hs)
{
   int rc;
   // 
   // Release Unit if reserved
   //
   if (g_UnitReserved)
   {
      if (S_OK!= (rc=scanReleaseUnit(hs)))
         printf("Release Unit failed with return code %d\n",rc);
      g_UnitReserved = false;
   }

   //
   // Close the scanner
   //
   if(hs>0)
   {
      if (S_OK!= (rc=scanCloseScanner(hs)))
         printf("Close Scanner failed with return code %d\n",rc);
   }
   //
   // Close the library
   //
   if (S_OK!= (rc=scanCloseLib()))
      printf("Close Library failed with return code %d\n",rc);

   int ch = _getch();
   return 0;
}

int DisplayErrorAndExit(HSCANNER hs, int rc, char* Message)
{
   printf("%s\n\n",Message);
   SetError(hs,rc);
   return CloseAndExit(hs);
}

void SetupColorScan(int width, int length, int dpi, bool bUseSRGB, bool bCenterLoad, SETWINDOWPARAMS &swp)
{
   memset(&swp,0,sizeof(swp));
   swp.m_ParameterListLength  = g_ScanAttr.maxSetWindowLength-WINDOW_DESC_OFFSET;
   swp.m_dpix                 = dpi;
   swp.m_dpiy                 = dpi;
   //swp.m_upperLeftX         = 0; // Assume side-loaded paper!!
   //swp.m_upperLeftY         = 0; // No vertical offset
   swp.m_width                = (long)MM_TO_INCHDIV1200(width);
   swp.m_length               = (long)MM_TO_INCHDIV1200(length);
   if (bCenterLoad)
      swp.m_upperLeftX = g_ScanAttr.maxScanWidth/2 - swp.m_width/2 ; 
   swp.m_threshold            = 151; // All above is black if BW mode used
   swp.m_imageComposition     = 5; // RGB mode
   swp.m_colorComposition     = 4;
   swp.m_bitsPerPixel         = 24;
   //swp.m_rif                = 0; // Not reversing BW data
   swp.m_compressionType      = 0x0; // 0x80; if BW
   //swp.m_dynamicThreshold   = 0;
   //swp.m_lineEnhancement    = 0;
   swp.m_lineLimit            = 1000;
   //swp.m_scanDirection      = 0; // scan in the forward direction
   //   swp.m_bufferLimit          = 0x100000;//1MB
   //swp.m_colorComposition   = 0; only used for color image composition
   //swp.m_dspBackgroundLevel = 0;
   //swp.m_dspAdaptiveLevel   = 0;
   //swp.m_dspConstant        = 3;
   swp.m_scanSpeed            = 100;//50;
   //swp.m_useFeatureRam      = 0;
   swp.m_blur                 = 0; // 0 -- 16
   swp.m_sharpening           = 0; // -1 -- 8
   swp.m_ColorSpaceType       = bUseSRGB  ? 1 : 0; // use sRGB color space
   swp.m_ColorSaturationLevel = 100; // only use in color mode
   //
   // finally swap words from little endian to big endian before writing to the scanner
   //
   swp.Convert();
}

void SetupGraytoneScan(int width, int length, int dpi, bool bCenterLoad, SETWINDOWPARAMS &swp)
{
   memset(&swp,0,sizeof(swp));
   swp.m_ParameterListLength  = g_ScanAttr.maxSetWindowLength-WINDOW_DESC_OFFSET;
   swp.m_dpix                 = dpi;
   swp.m_dpiy                 = dpi;
   //swp.m_upperLeftX         = 0; // Assume side-loaded paper!!
   //swp.m_upperLeftY         = 0; // No vertical offset
   swp.m_width                = (long)MM_TO_INCHDIV1200(width);
   swp.m_length               = (long)MM_TO_INCHDIV1200(length);
   if (bCenterLoad)
      swp.m_upperLeftX = g_ScanAttr.maxScanWidth/2 - swp.m_width/2 ; 
   swp.m_threshold            = 151; // All above is black if BW mode used
   swp.m_imageComposition     = 2; // Gray tone mode
   swp.m_bitsPerPixel         = 8;
   //swp.m_rif                = 0; // Not reversing BW data
   swp.m_compressionType      = 0x0; // 0x80; if BW
   //swp.m_dynamicThreshold   = 0;
   //swp.m_lineEnhancement    = 0;
   swp.m_lineLimit            = 1000;
   //swp.m_scanDirection      = 0; // scan in the forward direction
   //swp.m_bufferLimit          = 0x100000;//1MB
   //swp.m_colorComposition   = 0; only used for color image composition
   //swp.m_dspBackgroundLevel = 0;
   //swp.m_dspAdaptiveLevel   = 0;
   //swp.m_dspConstant        = 3;
   swp.m_scanSpeed            = 0;//50;
   //swp.m_useFeatureRam      = 0;
   swp.m_blur                 = 0; // 0 -- 16
   swp.m_sharpening           = 0; // -1 -- 8
   swp.m_ColorSpaceType       = 0; // use sRGB color space
   swp.m_ColorSaturationLevel = 100; // only use in color mode
   //
   // finally swap words from little endian to big endian before writing to the scanner
   //
   swp.Convert();

}

// The Soft paper handling settings is stored in the scanner
// i.e. it stays in effect until being reset.
int SoftHandling(HSCANNER hs, bool bOn)
{
   BYTE buffer[1];
   buffer[0] = (bOn ? 0x10 : 0x00);
   int rc = scanWriteBuffer(hs,buffer,0x01,SCAN_BUFFER_ID_PAPER_HANDLING,0,1);
   if (rc == S_OK)
   {
      DWORD dwSpeed = (bOn? 0x05 : 0xFFFF);
      dwSpeed *= 1200;     // Unit is: inch/1200/sec
      dwSpeed = MySwap(dwSpeed);
      rc = scanWriteBuffer(hs,(BYTE*)&dwSpeed,0x01,SCAN_BUFFER_ID_MAX_MOVE_SPEED,0,sizeof(dwSpeed));
   }
   return rc;
}

// The HD Ultra scanner has a normal and a high internal scanner resolution.
// The scanner application can control which internal scanner resolution is used 
// when the scanner application scans. 
// This setting is volatile and is not stored in the scanner (default is normal resolution).
int SetHighScannerResolution(HSCANNER hs, BYTE ScanMode, BYTE Width)
{
   BYTE buf[3];

   switch (ScanMode)
   {
   case 'G':
      PUT_WORD(buf,0,1);
      break;
   case 'C':
      PUT_WORD(buf,0,0);
      break;
   }
   buf[2] = Width;
   return scanWriteBuffer(hs,buf,0x01,0xEA,0,sizeof(buf));
}

int DisplayScannerInfo(HSCANNER hs)
{
   printf("The Scanner is a model : %.20s\n",(const char *)g_ScanAttr.scannerId.c_str());
   printf("Produced by            : %.20s\n",(const char *)g_ScanAttr.vendorId.c_str());

   BYTE ScannerName[64];
   if (S_OK == scanGetScannerNameToDisplay(hs,ScannerName,sizeof(ScannerName)))
      printf("Scanner name           : %.64s\n",ScannerName);
   else
      printf("Scanner name           : %.64s\n",g_ScanAttr.ProductID.c_str());

   printf("No of Cameras          : %2d\n",g_ScanAttr.NoOfCameras);

   printf("Scanner supports       : ");
   switch (g_ScanAttr.colorSupport)
   {
      case 0 : printf("gray only\n"); break;
      case 1 : printf("color and gray\n"); break;
      case 2 : printf("color only\n"); break;
      default: printf("?"); break;
   }

   {  
      printf("Color bit width support: ");
      int iBits = g_ScanAttr.colorBitWidthSupport;
      int iColorBitWidth[] = { 24, 48 };
      for (int idx=0; idx < (sizeof(iColorBitWidth)/sizeof(int)); idx++)
      {
         if (iBits && 0x01)
            printf("%2d  ", iColorBitWidth[idx]);
         iBits >>= 1;
      }
      printf("\n");
   }

   {  
      printf("Gray bit width support : ");
      int iBits = g_ScanAttr.graytoneBitWidthSupport;
      int iGrayBitWidth[] = { 8, 16 };
      for (int idx=0; idx < (sizeof(iGrayBitWidth)/sizeof(int)); idx++)
      {
         if (iBits && 0x01)
            printf("%2d  ", iGrayBitWidth[idx]);
         iBits >>= 1;
      }
      printf("\n");
   }

   printf("Max scan width         : %2d\"\n\n",g_ScanAttr.maxScanWidth/1200);
   return 1;
}

void parse_args(int argc, char* argv[]) {
  int c;
  char **endptr;

  while (1)
    {
      static struct option long_options[] =
        {
          {"dpi",  required_argument, 0, 'd'},
          {"width",    required_argument, 0, 'w'},
          {"height",    required_argument, 0, 'h'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "d:w:h:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'd':
          printf ("option -d with value `%s'\n", optarg);
		  use_dpi = strtol(optarg, endptr, 10);
          break;

        case 'w':
          printf ("option -w with value `%s'\n", optarg);
		  use_width = strtol(optarg, endptr, 10);
          break;

        case 'h':
          printf ("option -h with value `%s'\n", optarg);
		  use_height= strtol(optarg, endptr, 10);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          exit(1);
        }
    }
}

int main(int argc, char* argv[])
{
   int rc;
   //
   // 32 or 64 bit ?
   //
#ifdef _WIN64
   printf("SimpleScan 64 bit version started\n");
#else
   printf("SimpleScan 32 bit version started\n");
#endif

   parse_args(argc, argv);

   // 
   //  We must open the library first
   //
   if(S_OK != (rc=scanOpenLib()) ) 
   {
      printf("Failed to open scanner library (rc=%d)\n",rc);
      int ch = _getch();
      return 1;
   }

   HSCANNER hs=-1;   // HSCANNER is defined in ctx_scan_2000.h
   BOOL bIsOpen;     // Dummy var. 
   // 
   //  We locate and use the first available wide format scanner
   //
   if(S_OK != (rc=scanGetNextScanner(&hs,&bIsOpen,TRUE))) // get first available CONTEX scanner 
      return DisplayErrorAndExit(hs, rc, "Failed to locate scanner");

   //
   // Uncomment the following if you have more than one scanner
   // and want to use number two in the list for scanning.
   //
   /*
   if(S_OK != (rc=scanGetNextScanner(&hs,&bIsOpen,FALSE))) // get next available CONTEX scanner 
   return DisplayErrorAndExit(hs,rc, "Failed to locate next scanner");
   */

   //
   // Open the scanner
   //
   if(S_OK!=(rc=scanOpenScanner(hs)))
      return DisplayErrorAndExit(hs, rc, "Failed to open scanner");

   //
   // Read some attributes from the scanner
   //  
   if(S_OK!=(rc = ReadAttributes(hs)))
      return DisplayErrorAndExit(hs, rc, "Failed to read scanner attributes");

   if (!DisplayScannerInfo(hs))
      return 3;

   //
   // Reserve Unit
   //
   if(S_OK!=(rc = scanReserveUnit(hs)))
   {
      if(rc == SCSI_STATUS_RESERVATION_CONFLICT)
         return DisplayErrorAndExit(hs, rc, "Scanner already reserved");
      else
         return DisplayErrorAndExit(hs, rc, "Failed to reserve scanner");
   }
   g_UnitReserved = true;

   //
   // Enable the code below if high scanner resolution is required
   //
   /*
   switch (SCANMODE)
   {
   case 'C' :
   if(S_OK!=(rc = SetHighScannerResolution(hs, SCANMODE,48)))
   return DisplayErrorAndExit(hs, rc, "Failed to set high color scanner resolution");
   break;
   case 'G' :
   if(S_OK!=(rc = SetHighScannerResolution(hs, SCANMODE,16)))
   return DisplayErrorAndExit(hs, rc, "Failed to set high color scanner resolution");
   break;
   default:
   break;
   }
   */

   //
   // Enable code below to control soft paper handling 
   //
   /*  
   if (S_OK!=(rc = SoftHandling(hs,true)))
   return DisplayErrorAndExit(hs, rc, "Failed to set soft paper handling");
   */

   //
   // Load paper 
   //
   if(S_OK!=(rc = scanObjectPosition(hs,SCAN_OBJ_POS_LOAD, 0)))
      return DisplayErrorAndExit(hs, rc, "Failed to load media");

   //
   // wait for media to load
   //
   BYTE buf[128];
   bool bIsLoading = true;
   int nBytesReceived=0;

   while(bIsLoading)
   {
      rc = scanRead(hs, buf, 2,
         SCAN_READSEND_CODE_SCANNER_STATUS,
         SCAN_READSEND_QUALIFIER_SCANNER_STATUS,
         &nBytesReceived);

      if (rc == 0 && nBytesReceived == 2)
      {
         if (buf[1]== 0x30)  //Ready; original has just been loaded
            bIsLoading = false;
         else
         {
            bIsLoading = true;
            printf("Loading media...\n");
            Sleep(200);
         }
      }
      else
         return DisplayErrorAndExit(hs, rc, "Failed to read media status");
   }

   //
   //  Set scan window
   //
   SETWINDOWPARAMS swp;
   bool bUseSRGB  = true;
   int  dpi       = use_dpi;
   char scanMode  = SCANMODE;
   //int width      = 490;
   int width      = use_width;
   //int width      = 482;
   int height     = use_height;
   bool bCenterLoad = false;

   switch (g_ScanAttr.centerLoad)
   {
   case 0:  bCenterLoad = false;  // Original can be placed at the first pixel
            break;
   case 1:  bCenterLoad = true;   // Original can be placed at the center pixel
            break;
   case 2:  bCenterLoad = true;   // Original can be placed at both the center and the first pixel
            {
               // Get Paper load position from Scanner Status
               rc = scanRead(hs, buf, 97,
                  SCAN_READSEND_CODE_SCANNER_STATUS,
                  SCAN_READSEND_QUALIFIER_SCANNER_STATUS,
                  &nBytesReceived);

               if (rc == 0 && nBytesReceived == 97)
               {
                  bCenterLoad = (buf[96] == 0) ? true : false;
               }
               else
                  bCenterLoad = false;
            }
            break;
   default: bCenterLoad = false;
            break;
   }
        
   printf("Mode                   : %c\n", scanMode);
   printf("File                   : %s\n", WRITETOFILE ? "yes" : "no");
   printf("Center load            : %s\n", bCenterLoad ? "yes" : "no");
   printf("Dpi                    : %d (%d-%d)\n", dpi, g_ScanAttr.minDpiX, g_ScanAttr.maxDpiX);
   printf("Width*Height           : %d * %d mm\n", width, height);

   switch (scanMode)
   {
   case 'G': // Graytone 
      SetupGraytoneScan(width, height, dpi, bCenterLoad, swp);
      break;
   case 'C': // 24 bit color
      SetupColorScan(width, height, dpi, bUseSRGB, bCenterLoad, swp);
      break;
   default:
      DisplayErrorAndExit(hs, 0, "Invalid scan mode");
   }

   //
   // Is Post Scan Original Handling supported?
   //
   bool bPostScanHandlingSupported=false;
   if (g_ScanAttr.maxSetWindowLength > offsetof(SETWINDOWPARAMS,m_PostScanOriginalHandling))
   {
      bPostScanHandlingSupported = true;
      swp.m_PostScanOriginalHandling = SCAN_OBJ_POST_POS_EJECT_ROLLERS;
   }

   //
   // Send the window to the scanner
   //
   {
      BYTE * pSetWindowBuffer = new BYTE[g_ScanAttr.maxSetWindowLength];
      memset(pSetWindowBuffer,0,g_ScanAttr.maxSetWindowLength);
      memcpy(pSetWindowBuffer,&swp,std::min((int)sizeof(swp),g_ScanAttr.maxSetWindowLength));
      rc = scanSetWindow(hs,pSetWindowBuffer,g_ScanAttr.maxSetWindowLength);
      delete[] pSetWindowBuffer;
      if(rc != S_OK)
         return DisplayErrorAndExit(hs, rc, "Failed to set scan window");
   }

   //
   // Set neutral gammatable
   //
   BYTE *gammaBuf = new BYTE[3*g_ScanAttr.SizeOfGammaTable];
   MakeGammaTable(gammaBuf, g_ScanAttr.SizeOfGammaTable,bUseSRGB ); // Red part
   MakeGammaTable(gammaBuf + g_ScanAttr.SizeOfGammaTable, g_ScanAttr.SizeOfGammaTable,bUseSRGB );// Green part
   MakeGammaTable(gammaBuf + 2*g_ScanAttr.SizeOfGammaTable, g_ScanAttr.SizeOfGammaTable,bUseSRGB );// Blue part
   if(S_OK!=(scanSend(hs, gammaBuf, 3*g_ScanAttr.SizeOfGammaTable, SCAN_READSEND_CODE_GAMMA, SCAN_READSEND_QUALIFIER_GAMMA)))
      return DisplayErrorAndExit(hs, rc, "Failed to send gamma table");
   delete[] gammaBuf;

   //
   // Set BW point
   //  
   BYTE bwPointBuffer[24];
   memset(bwPointBuffer,0,24);
   int wp = (bUseSRGB) ? 275 : 255;
   *((DWORD *)(bwPointBuffer+0)) = wp;
   *((DWORD *)(bwPointBuffer+4)) = wp;
   *((DWORD *)(bwPointBuffer+8)) = wp;

   *((DWORD *)(bwPointBuffer+12)) = 0;
   *((DWORD *)(bwPointBuffer+16)) = 0;
   *((DWORD *)(bwPointBuffer+20)) = 0;

   if(S_OK!=(scanSend(hs,bwPointBuffer, 24,SCAN_READSEND_CODE_BWPOINT, SCAN_READSEND_QUALIFIER_LINEARIZE_WORD)))
      return DisplayErrorAndExit(hs, rc, "Failed to set BW points");

   //
   //  Issue start scan command.
   //
   BYTE tmp=0;
   if(S_OK!=(rc = scanScan(hs, &tmp, 1)))
      return DisplayErrorAndExit(hs, rc, "Failed to send scan command");

   //
   // After a scan command we can read the actual number
   // of bytes that will be read by each camera pr. line.
   // We use this info to calculate approximate amount of data.
   //
   DWORD dwActualBytesPerLine = 0;
   int  nBytesRead;
   int  iPixels;
   int  iCameraBufSize = 2*g_ScanAttr.NoOfCameras;
   BYTE *pbCameraBuf = new BYTE[iCameraBufSize];
   rc = scanRead(hs, pbCameraBuf, iCameraBufSize, 0xFF, 0x02, &nBytesRead);

   if (rc == 0 && nBytesRead == iCameraBufSize)
   {
      for (int iCamNo=0; iCamNo < g_ScanAttr.NoOfCameras; iCamNo++)
         dwActualBytesPerLine += GET_WORD(pbCameraBuf, iCamNo*2);

      iPixels = (int)((double)dwActualBytesPerLine/((double)swp.m_bitsPerPixel/8.0));
   }
   delete[] pbCameraBuf;

   // Use max buffer size specified by the scanner to get highest performance
   int iBytesToRead  =  g_ScanAttr.bufferSize;
   int iBytesRead;
   BYTE *pBuffer     = new BYTE[iBytesToRead];
   if(!pBuffer)
      return CloseAndExit(hs);

   const char demofilename[] = "SimpleScan.bmp";
   CBmpData * pBmpData = new CBmpData((char*)(WRITETOFILE? demofilename: ""));
   if (!pBmpData)
      return CloseAndExit(hs);
   if(WRITETOFILE)
      pBmpData->SetLineInfo((scanMode == 'C') ? 3 : 1 , iPixels);

   printf("Actual pixels per line : %d\n", iPixels);

   //
   //  Enter loop to read data
   //
   do
   {
      rc = scanRead( hs,
         pBuffer,
         iBytesToRead,
         SCAN_READSEND_CODE_IMAGE,
         SCAN_READSEND_QUALIFIER_IMAGE,
         &iBytesRead);
      switch (rc)
      {
      case SCSI_STATUS_GOOD:
         {
            if(WRITETOFILE)
               pBmpData->AddData(pBuffer,(ULONG)iBytesRead);
            break;
         }
      case SCSI_STATUS_CHECK_CONDITION:
         {
            int err = scanGetLastAsc(hs);
            switch (err)
            {
            case ERROR_READ_PAST_END:
            case ERROR_READ_PAST_BEGINNING:
               // Correct end of scan reached
               break;

            default:
               {  // Error while reading data
                  char msg[256];
                  memset(msg,0,sizeof(msg));
                  scanGetErrorMessage(err,msg,sizeof(msg));
                  printf("Scan ended with error %d: %s\n",err, msg);
               }
               break;
            }
            break;
         }
      default:
         // Error reading data
         break;
      }
   } while(rc==S_OK);

   //
   // We save the image data in a BMP format
   //

   printf("Number of scan lines   : %d\n", pBmpData->GetNumberOfScanLines());

   delete pBmpData;                 // Close the file now
   pBmpData = NULL;
   delete[] pBuffer;

   //
   // Eject paper to back rollers
   //
   if (!bPostScanHandlingSupported)
      if(S_OK!=(rc = scanObjectPosition(hs,SCAN_OBJ_POS_UNLOAD, 0)))
         return DisplayErrorAndExit(hs, rc, "Failed to load media");

   struct stat stFileInfo;
   if (stat(demofilename, &stFileInfo) == 0)
      printf("Scanned image filename : %s \n",demofilename);
   else
      printf("Scanned failed - no output file\n");

   //
   // Enable code below to get a HD Ultra X scanner into sleep mode
   //
/*   if (scanGetHardwareType(hs) == CTX_CONNECTION_USBCY) // Use only with an USB cable
   {
      if (S_OK!= (rc=scanReleaseUnit(hs)))
         return DisplayErrorAndExit(hs, rc, "Failed to release scanner");
      g_UnitReserved=false;
      BYTE buf[1];
      buf[0] = 0;    // power down 
      if (S_OK!=(rc = scanSend(hs, buf, 1, SCAN_READSEND_CODE_POWER_DOWN, SCAN_READSEND_QUALIFIER_POWERDOWN)))
         return DisplayErrorAndExit(hs, rc, "Failed to get scanner to sleep");
      // Wait for scanner to sleep - max 15 * 100ms
      for (int i = 0; i < 15; i++)
      {
         int nBytesReceived=0;
         rc = scanRead(hs, buf, 1,
            SCAN_READSEND_CODE_SCANNER_STATUS,
            SCAN_READSEND_QUALIFIER_SCANNER_STATUS,
            &nBytesReceived);
         if (buf[0] == 10)          // Sleeping
            break;
         Sleep(100);
      }
   } */

   printf("\nPress any key to continue ...");
   return CloseAndExit(hs);
}

