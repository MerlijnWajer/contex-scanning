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
* Set write_to_file to true or false depending on whether you want file output
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
#include <sys/stat.h>		// stat
#include "utils.h"
#include "BmpData.h"
#include "ScannerAttributes.h"
#include "ctx_scan_2000.h"	// Include CONTEX SDK header filer
#include "SetWindowParams.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <png.h>
#include <math.h>

#include <fcntl.h>
#include <io.h>

#define SCANMODE 'C'

//
//  Global vars.
//
int g_errCode = S_OK;
int g_errFatal = 0;
ScannerAttributes g_ScanAttr;
bool g_UnitReserved = false;

long total_scanned_bytes = 0;

int use_dpi = 1200;
int use_width = 487;
int use_height = 609;

int use_left = 0;
int use_top = 0;

char *file_to_save_to = NULL;
bool write_to_file = false;

void SetError(HSCANNER hs, int eco)
{

	if (eco == SCSI_STATUS_CHECK_CONDITION) {
		g_errCode = scanGetLastAsc(hs);

		unsigned int e = ((unsigned int)g_errCode) >> 12;
		g_errFatal = (e != 3);
		char msg[256];
		scanGetErrorMessage(g_errCode, msg, sizeof(msg));
		fprintf(stderr, "Diagnostic message: %s\n", msg);
	} else {
		g_errCode = eco;
		g_errFatal = 1;
	}
	if (g_errFatal) {
		fprintf(stderr,
			"Fatal error encountered.\n\t Error code : 0x%X\n",
			g_errCode);
		fprintf(stderr,
			"Check REQUEST SENSE in SCSI documentation for error.\n");
	} else if (g_errCode) {
		fprintf(stderr,
			"Non-fatal error encountered.\n\t Error code : 0x%X\n",
			g_errCode);
		fprintf(stderr,
			"Check REQUEST SENSE in SCSI documentation for error.\n");
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

	if (S_OK !=
	    (rc = scanInquiry(hs, inqPageBuffer, sizeof(inqPageBuffer))))
		return rc;

	// get name of vendor
	g_ScanAttr.vendorId.assign((const char *)inqPageBuffer + 8, 8);

	// get name of scanner
	g_ScanAttr.scannerId.assign((const char *)inqPageBuffer + 16, 16);

	//
	// How many (and which) inquiry pages can be retrieved from scanner ?
	//
	scanInquiryPage(hs, inqPagesSupBuffer, sizeof(inqPagesSupBuffer),
			SCAN_INQUIRY_PAGE_PAGES_SUPPORTED);
	listLength = inqPagesSupBuffer[3];

	//
	// Get and parse all available pages
	//
	for (int listIndex = 1; listIndex < listLength; listIndex++) {
		if (S_OK ==
		    (rc =
		     scanInquiryPage(hs, inqPageBuffer, sizeof(inqPageBuffer),
				     inqPagesSupBuffer[4 + listIndex]))) {
			/* Switch on page code */
			switch (inqPageBuffer[1]) {
				//
				// TODO: You should parse these pages completely in a real application
				//
			case SCAN_INQUIRY_PAGE_REVISION:
				g_ScanAttr.generation =
				    GET_DWORD(inqPageBuffer, 60);

				break;

			case SCAN_INQUIRY_PAGE_AUTO_CONFIG:

				g_ScanAttr.colorSupport =
				    GET_BYTE(inqPageBuffer, 8);
				g_ScanAttr.lineEnhancement =
				    GET_BYTE(inqPageBuffer, 9);
				g_ScanAttr.centerLoad =
				    GET_BYTE(inqPageBuffer, 11);
				g_ScanAttr.NoOfCameras =
				    GET_DWORD(inqPageBuffer, 16);
				g_ScanAttr.bufferSize =
				    GET_DWORD(inqPageBuffer, 24);
				g_ScanAttr.maxScanWidth =
				    GET_DWORD(inqPageBuffer, 28);
				g_ScanAttr.DSPModes =
				    GET_DWORD(inqPageBuffer, 40);
				g_ScanAttr.thresholdModeSupport =
				    GET_DWORD(inqPageBuffer, 51);
				g_ScanAttr.minSharpenSoften =
				    GET_CHAR(inqPageBuffer, 55);
				g_ScanAttr.maxSharpenSoften =
				    GET_BYTE(inqPageBuffer, 56);
				g_ScanAttr.maxDataRate =
				    GET_DWORD(inqPageBuffer, 57);
				g_ScanAttr.minScanWidth =
				    GET_DWORD(inqPageBuffer, 61);
				g_ScanAttr.maxSetWindowLength =
				    GET_BYTE(inqPageBuffer, 66);

				if (inqPageBuffer[3] >= 133 - 4) {
					// Byte 133 and 134 were added at the same time
					g_ScanAttr.colorBitWidthSupport =
					    GET_BYTE(inqPageBuffer, 133);
					g_ScanAttr.graytoneBitWidthSupport =
					    GET_BYTE(inqPageBuffer, 134);
				} else {
					// Old scanners does not have this information, i.e. only the lowest bit width
					g_ScanAttr.colorBitWidthSupport = 0x01;
					g_ScanAttr.graytoneBitWidthSupport =
					    0x01;
				}

				break;

			case SCAN_INQUIRY_PAGE_VAR_RESOLUTION:
				g_ScanAttr.minDpiX =
				    GET_DWORD(inqPageBuffer, 8);
				g_ScanAttr.maxDpiX =
				    GET_DWORD(inqPageBuffer, 12);
				break;

			case SCAN_INQUIRY_PAGE_FIXED_RESOLUTION:
				break;
			case SCAN_INQUIRY_PAGE_COLOR:
				g_ScanAttr.SizeOfGammaTable =
				    GET_DWORD(inqPageBuffer, 36);
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
				g_ScanAttr.ProductID.
				    assign((const char *)inqPageBuffer + 150,
					   32);
				break;
			case SCAN_INQUIRY_PAGE_DISPLAY_INFORMATION:
				break;
			default:
				fprintf(stderr, "Unknown page 0x%02x\n",
					(int)inqPagesSupBuffer[4 + listIndex]);
				break;
			}
		} else {
			fprintf(stderr, "Failed retrieving page 0x%02x\n",
				(int)inqPagesSupBuffer[4 + listIndex]);
			return FALSE;
		}
	}

	return rc;
}

void MakeGammaTable(BYTE * pTab, int nReqSize, bool bUSeSRGB)
{
	int i;
	// Return a linear gamma table
	for (i = 0; i < nReqSize; i++) {
		pTab[i] = 255 * i / (nReqSize - 1);
		if (bUSeSRGB)
			pTab[i] =
			    (int)(0.5 +
				  255.0 *
				  (((double)pTab[i] / 255.0 <=
				    0.0034) ? (12.92 * (double)pTab[i] /
					       255.0) : (1.055 *
							 pow((double)pTab[i] /
							     255.0,
							     1 / 2.4) -
							 0.055)));

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
	if (g_UnitReserved) {
		if (S_OK != (rc = scanReleaseUnit(hs)))
			fprintf(stderr,
				"Release Unit failed with return code %d\n",
				rc);
		g_UnitReserved = false;
	}
	//
	// Close the scanner
	//
	if (hs > 0) {
		if (S_OK != (rc = scanCloseScanner(hs)))
			fprintf(stderr,
				"Close Scanner failed with return code %d\n",
				rc);
	}
	//
	// Close the library
	//
	if (S_OK != (rc = scanCloseLib()))
		fprintf(stderr, "Close Library failed with return code %d\n",
			rc);

	//int ch = _getch();
	return 0;
}

int DisplayErrorAndExit(HSCANNER hs, int rc, char *Message)
{
	fprintf(stderr, "%s\n\n", Message);
	SetError(hs, rc);
	return CloseAndExit(hs);
}

void SetupColorScan(int width, int length, int dpi, bool bUseSRGB,
		    bool bCenterLoad, SETWINDOWPARAMS & swp)
{
	memset(&swp, 0, sizeof(swp));
	swp.m_ParameterListLength =
	    g_ScanAttr.maxSetWindowLength - WINDOW_DESC_OFFSET;
	swp.m_dpix = dpi;
	swp.m_dpiy = dpi;
	//swp.m_upperLeftX         = 0; // Assume side-loaded paper!!
	//swp.m_upperLeftY         = 0; // No vertical offset
	swp.m_upperLeftX = (long)MM_TO_INCHDIV1200(use_left);	// Assume side-loaded paper!!
	swp.m_upperLeftY = (long)MM_TO_INCHDIV1200(use_top);	// No vertical offset
	swp.m_width = (long)MM_TO_INCHDIV1200(width);
	fprintf(stderr, "width: %d; mm_to_inchdiv1200 width: %d\n", width,
		swp.m_width);
	swp.m_length = (long)MM_TO_INCHDIV1200(length);
	fprintf(stderr, "length: %d; mm_to_inchdiv1200 length: %d\n", length,
		swp.m_length);
	if (bCenterLoad)
		swp.m_upperLeftX =
		    g_ScanAttr.maxScanWidth / 2 - swp.m_width / 2;
	swp.m_threshold = 151;	// All above is black if BW mode used
	swp.m_imageComposition = 5;	// RGB mode
	swp.m_colorComposition = 4;	// 4 = RGB color
	swp.m_bitsPerPixel = 24;
	//swp.m_rif                = 0; // Not reversing BW data
	swp.m_compressionType = 0x0;	// 0x80; if BW
	//swp.m_dynamicThreshold   = 0;
	//swp.m_lineEnhancement    = 0;
	swp.m_lineLimit = 1000;
	//swp.m_scanDirection        = 0; // scan in the forward direction
	//   swp.m_bufferLimit          = 0x100000;//1MB
	//swp.m_colorComposition   = 0; only used for color image composition
	//swp.m_dspBackgroundLevel = 0;
	//swp.m_dspAdaptiveLevel   = 0;
	//swp.m_dspConstant        = 3;
	swp.m_scanSpeed = 100;	// Apparently scan speed in percentage
	//swp.m_useFeatureRam      = 0;
	swp.m_blur = 0;		// 0 -- 16
	swp.m_sharpening = 0;	// -1 -- 8
	// sRGB = 1 (but what sRGB exactly???)
	swp.m_ColorSpaceType = bUseSRGB ? 1 : 0;	// use sRGB color space
	swp.m_ColorSaturationLevel = 100;	// only use in color mode
	//
	// finally swap words from little endian to big endian before writing to the scanner
	//
	swp.Convert();
}

void SetupGraytoneScan(int width, int length, int dpi, bool bCenterLoad,
		       SETWINDOWPARAMS & swp)
{
	memset(&swp, 0, sizeof(swp));
	swp.m_ParameterListLength =
	    g_ScanAttr.maxSetWindowLength - WINDOW_DESC_OFFSET;
	swp.m_dpix = dpi;
	swp.m_dpiy = dpi;
	//swp.m_upperLeftX         = 0; // Assume side-loaded paper!!
	//swp.m_upperLeftY         = 0; // No vertical offset
	swp.m_width = (long)MM_TO_INCHDIV1200(width);
	swp.m_length = (long)MM_TO_INCHDIV1200(length);
	if (bCenterLoad)
		swp.m_upperLeftX =
		    g_ScanAttr.maxScanWidth / 2 - swp.m_width / 2;
	swp.m_threshold = 151;	// All above is black if BW mode used
	swp.m_imageComposition = 2;	// Gray tone mode
	swp.m_bitsPerPixel = 8;
	//swp.m_rif                = 0; // Not reversing BW data
	swp.m_compressionType = 0x0;	// 0x80; if BW
	//swp.m_dynamicThreshold   = 0;
	//swp.m_lineEnhancement    = 0;
	swp.m_lineLimit = 1000;
	//swp.m_scanDirection      = 0; // scan in the forward direction
	//swp.m_bufferLimit          = 0x100000;//1MB
	//swp.m_colorComposition   = 0; only used for color image composition
	//swp.m_dspBackgroundLevel = 0;
	//swp.m_dspAdaptiveLevel   = 0;
	//swp.m_dspConstant        = 3;
	swp.m_scanSpeed = 0;	//50;
	//swp.m_useFeatureRam      = 0;
	swp.m_blur = 0;		// 0 -- 16
	swp.m_sharpening = 0;	// -1 -- 8
	swp.m_ColorSpaceType = 0;	// use sRGB color space
	swp.m_ColorSaturationLevel = 100;	// only use in color mode
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
	int rc =
	    scanWriteBuffer(hs, buffer, 0x01, SCAN_BUFFER_ID_PAPER_HANDLING, 0,
			    1);
	if (rc == S_OK) {
		DWORD dwSpeed = (bOn ? 0x05 : 0xFFFF);
		dwSpeed *= 1200;	// Unit is: inch/1200/sec
		dwSpeed = MySwap(dwSpeed);
		rc = scanWriteBuffer(hs, (BYTE *) & dwSpeed, 0x01,
				     SCAN_BUFFER_ID_MAX_MOVE_SPEED, 0,
				     sizeof(dwSpeed));
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

	switch (ScanMode) {
	case 'G':
		PUT_WORD(buf, 0, 1);
		break;
	case 'C':
		PUT_WORD(buf, 0, 0);
		break;
	}
	buf[2] = Width;
	return scanWriteBuffer(hs, buf, 0x01, 0xEA, 0, sizeof(buf));
}

int DisplayScannerInfo(HSCANNER hs)
{
	fprintf(stderr, "The Scanner is a model : %.20s\n",
		(const char *)g_ScanAttr.scannerId.c_str());
	fprintf(stderr, "Produced by            : %.20s\n",
		(const char *)g_ScanAttr.vendorId.c_str());

	BYTE ScannerName[64];
	if (S_OK ==
	    scanGetScannerNameToDisplay(hs, ScannerName, sizeof(ScannerName)))
		fprintf(stderr, "Scanner name           : %.64s\n",
			ScannerName);
	else
		fprintf(stderr, "Scanner name           : %.64s\n",
			g_ScanAttr.ProductID.c_str());

	fprintf(stderr, "No of Cameras          : %2d\n",
		g_ScanAttr.NoOfCameras);

	fprintf(stderr, "Scanner supports       : ");
	switch (g_ScanAttr.colorSupport) {
	case 0:
		fprintf(stderr, "gray only\n");
		break;
	case 1:
		fprintf(stderr, "color and gray\n");
		break;
	case 2:
		fprintf(stderr, "color only\n");
		break;
	default:
		fprintf(stderr, "?");
		break;
	}

	{
		fprintf(stderr, "Color bit width support: ");
		int iBits = g_ScanAttr.colorBitWidthSupport;
		int iColorBitWidth[] = { 24, 48 };
		for (int idx = 0; idx < (sizeof(iColorBitWidth) / sizeof(int));
		     idx++) {
			if (iBits && 0x01)
				fprintf(stderr, "%2d  ", iColorBitWidth[idx]);
			iBits >>= 1;
		}
		fprintf(stderr, "\n");
	}

	{
		fprintf(stderr, "Gray bit width support : ");
		int iBits = g_ScanAttr.graytoneBitWidthSupport;
		int iGrayBitWidth[] = { 8, 16 };
		for (int idx = 0; idx < (sizeof(iGrayBitWidth) / sizeof(int));
		     idx++) {
			if (iBits && 0x01)
				fprintf(stderr, "%2d  ", iGrayBitWidth[idx]);
			iBits >>= 1;
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "Max scan width         : %2d\"\n\n",
		g_ScanAttr.maxScanWidth / 1200);
	return 1;
}

void parse_args(int argc, char *argv[])
{
	int c;
	char **endptr = NULL;

	/* mostly verbatim copy from https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example */

	while (1) {
		static struct option long_options[] = {
			{"save", no_argument, 0, 's'},
			{"dpi", required_argument, 0, 'd'},
			{"width", required_argument, 0, 'w'},
			{"height", required_argument, 0, 'h'},
			{"left", required_argument, 0, 'l'},
			{"top", required_argument, 0, 't'},
			{"file", required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "d:w:h:l:t:f:s",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			fprintf(stderr, "option %s",
				long_options[option_index].name);
			if (optarg)
				fprintf(stderr, " with arg %s", optarg);
			fprintf(stderr, "\n");
			break;

		case 'd':
			fprintf(stderr, "option -d with value `%s'\n", optarg);
			use_dpi = strtol(optarg, endptr, 10);
			break;

		case 'w':
			fprintf(stderr, "option -w with value `%s'\n", optarg);
			use_width = strtol(optarg, endptr, 10);
			break;

		case 'h':
			fprintf(stderr, "option -h with value `%s'\n", optarg);
			use_height = strtol(optarg, endptr, 10);
			break;

		case 'l':
			fprintf(stderr, "option -l with value `%s'\n", optarg);
			use_left = strtol(optarg, endptr, 10);
			break;

		case 't':
			fprintf(stderr, "option -t with value `%s'\n", optarg);
			use_top = strtol(optarg, endptr, 10);
			break;

		case 's':
			fprintf(stderr, "option -s passed\n");
			write_to_file = true;
			break;

		case 'f':
			fprintf(stderr, "option -f with value `%s'\n", optarg);
			file_to_save_to = strdup(optarg);
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			exit(1);
		}
	}
}

int main(int argc, char *argv[])
{
	int rc;
	//
	// 32 or 64 bit ?
	//
#ifdef _WIN64
	fprintf(stderr, "SimpleScan 64 bit version started\n");
#else
	fprintf(stderr, "SimpleScan 32 bit version started\n");
#endif

	parse_args(argc, argv);

	//
	//  We must open the library first
	//
	if (S_OK != (rc = scanOpenLib())) {
		fprintf(stderr, "Failed to open scanner library (rc=%d)\n", rc);
		//int ch = _getch();
		return 1;
	}

	HSCANNER hs = -1;	// HSCANNER is defined in ctx_scan_2000.h
	BOOL bIsOpen;		// Dummy var.
	//
	//  We locate and use the first available wide format scanner
	//
	if (S_OK != (rc = scanGetNextScanner(&hs, &bIsOpen, TRUE)))	// get first available CONTEX scanner
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
	if (S_OK != (rc = scanOpenScanner(hs)))
		return DisplayErrorAndExit(hs, rc, "Failed to open scanner");

	//
	// Read some attributes from the scanner
	//
	if (S_OK != (rc = ReadAttributes(hs)))
		return DisplayErrorAndExit(hs, rc,
					   "Failed to read scanner attributes");

	if (!DisplayScannerInfo(hs))
		return 3;

	//
	// Reserve Unit
	//
	if (S_OK != (rc = scanReserveUnit(hs))) {
		if (rc == SCSI_STATUS_RESERVATION_CONFLICT)
			return DisplayErrorAndExit(hs, rc,
						   "Scanner already reserved");
		else
			return DisplayErrorAndExit(hs, rc,
						   "Failed to reserve scanner");
	}
	g_UnitReserved = true;

	//
	// Load paper
	//
	if (S_OK != (rc = scanObjectPosition(hs, SCAN_OBJ_POS_LOAD, 0)))
		return DisplayErrorAndExit(hs, rc, "Failed to load media");

	//
	// wait for media to load
	//
	BYTE buf[128];
	bool bIsLoading = true;
	int nBytesReceived = 0;

	while (bIsLoading) {
		rc = scanRead(hs, buf, 2,
			      SCAN_READSEND_CODE_SCANNER_STATUS,
			      SCAN_READSEND_QUALIFIER_SCANNER_STATUS,
			      &nBytesReceived);

		if (rc == 0 && nBytesReceived == 2) {
			if (buf[1] == 0x30)	//Ready; original has just been loaded
				bIsLoading = false;
			else {
				bIsLoading = true;
				fprintf(stderr, "Loading media...\n");
				Sleep(200);
			}
		} else
			return DisplayErrorAndExit(hs, rc,
						   "Failed to read media status");
	}

	//
	//  Set scan window
	//
	SETWINDOWPARAMS swp;
	bool bUseSRGB = true;
	int dpi = use_dpi;
	char scanMode = SCANMODE;
	//int width      = 490;
	int width = use_width;
	//int width      = 482;
	int height = use_height;
	bool bCenterLoad = false;

	switch (g_ScanAttr.centerLoad) {
	case 0:
		bCenterLoad = false;	// Original can be placed at the first pixel
		break;
	case 1:
		bCenterLoad = true;	// Original can be placed at the center pixel
		break;
	case 2:
		bCenterLoad = true;	// Original can be placed at both the center and the first pixel
		{
			// Get Paper load position from Scanner Status
			rc = scanRead(hs, buf, 97,
				      SCAN_READSEND_CODE_SCANNER_STATUS,
				      SCAN_READSEND_QUALIFIER_SCANNER_STATUS,
				      &nBytesReceived);

			if (rc == 0 && nBytesReceived == 97) {
				bCenterLoad = (buf[96] == 0) ? true : false;
			} else
				bCenterLoad = false;
		}
		break;
	default:
		bCenterLoad = false;
		break;
	}

	fprintf(stderr, "Mode                   : %c\n", scanMode);
	fprintf(stderr, "File                   : %s\n",
		write_to_file ? "yes" : "no");
	fprintf(stderr, "Center load            : %s\n",
		bCenterLoad ? "yes" : "no");
	fprintf(stderr, "Dpi                    : %d (%d-%d)\n", dpi,
		g_ScanAttr.minDpiX, g_ScanAttr.maxDpiX);
	fprintf(stderr, "Width*Height           : %d * %d mm\n", width, height);

	switch (scanMode) {
	case 'G':		// Graytone
		SetupGraytoneScan(width, height, dpi, bCenterLoad, swp);
		break;
	case 'C':		// 24 bit color
		SetupColorScan(width, height, dpi, bUseSRGB, bCenterLoad, swp);
		break;
	default:
		DisplayErrorAndExit(hs, 0, "Invalid scan mode");
	}

	//
	// Is Post Scan Original Handling supported?
	//
	bool bPostScanHandlingSupported = false;
	if (g_ScanAttr.maxSetWindowLength >
	    offsetof(SETWINDOWPARAMS, m_PostScanOriginalHandling)) {
		bPostScanHandlingSupported = true;
		swp.m_PostScanOriginalHandling =
		    SCAN_OBJ_POST_POS_EJECT_ROLLERS;
	}
	//
	// Send the window to the scanner
	//
	{
		BYTE *pSetWindowBuffer =
		    new BYTE[g_ScanAttr.maxSetWindowLength];
		memset(pSetWindowBuffer, 0, g_ScanAttr.maxSetWindowLength);
		memcpy(pSetWindowBuffer, &swp,
		       std::min((int)sizeof(swp),
				g_ScanAttr.maxSetWindowLength));
		rc = scanSetWindow(hs, pSetWindowBuffer,
				   g_ScanAttr.maxSetWindowLength);
		delete[]pSetWindowBuffer;
		if (rc != S_OK)
			return DisplayErrorAndExit(hs, rc,
						   "Failed to set scan window");
	}

	//
	// Set neutral gammatable
	//
	BYTE *gammaBuf = new BYTE[3 * g_ScanAttr.SizeOfGammaTable];
	MakeGammaTable(gammaBuf, g_ScanAttr.SizeOfGammaTable, bUseSRGB);	// Red part
	MakeGammaTable(gammaBuf + g_ScanAttr.SizeOfGammaTable, g_ScanAttr.SizeOfGammaTable, bUseSRGB);	// Green part
	MakeGammaTable(gammaBuf + 2 * g_ScanAttr.SizeOfGammaTable, g_ScanAttr.SizeOfGammaTable, bUseSRGB);	// Blue part
	if (S_OK !=
	    (scanSend
	     (hs, gammaBuf, 3 * g_ScanAttr.SizeOfGammaTable,
	      SCAN_READSEND_CODE_GAMMA, SCAN_READSEND_QUALIFIER_GAMMA)))
		return DisplayErrorAndExit(hs, rc,
					   "Failed to send gamma table");
	delete[]gammaBuf;

	//
	// Set BW point
	//
	BYTE bwPointBuffer[24];
	memset(bwPointBuffer, 0, 24);
	int wp = (bUseSRGB) ? 275 : 255;
	*((DWORD *) (bwPointBuffer + 0)) = wp;
	*((DWORD *) (bwPointBuffer + 4)) = wp;
	*((DWORD *) (bwPointBuffer + 8)) = wp;

	*((DWORD *) (bwPointBuffer + 12)) = 0;
	*((DWORD *) (bwPointBuffer + 16)) = 0;
	*((DWORD *) (bwPointBuffer + 20)) = 0;

	if (S_OK !=
	    (scanSend
	     (hs, bwPointBuffer, 24, SCAN_READSEND_CODE_BWPOINT,
	      SCAN_READSEND_QUALIFIER_LINEARIZE_WORD)))
		return DisplayErrorAndExit(hs, rc, "Failed to set BW points");

	//
	//  Issue start scan command.
	//
	BYTE tmp = 0;
	if (S_OK != (rc = scanScan(hs, &tmp, 1)))
		return DisplayErrorAndExit(hs, rc,
					   "Failed to send scan command");

	//
	// After a scan command we can read the actual number
	// of bytes that will be read by each camera pr. line.
	// We use this info to calculate approximate amount of data.
	//
	DWORD dwActualBytesPerLine = 0;
	int nBytesRead;
	int iPixels;
	int iCameraBufSize = 2 * g_ScanAttr.NoOfCameras;
	BYTE *pbCameraBuf = new BYTE[iCameraBufSize];
	rc = scanRead(hs, pbCameraBuf, iCameraBufSize, 0xFF, 0x02, &nBytesRead);

	if (rc == 0 && nBytesRead == iCameraBufSize) {
		for (int iCamNo = 0; iCamNo < g_ScanAttr.NoOfCameras; iCamNo++)
			dwActualBytesPerLine +=
			    GET_WORD(pbCameraBuf, iCamNo * 2);

		iPixels =
		    (int)((double)dwActualBytesPerLine /
			  ((double)swp.m_bitsPerPixel / 8.0));
	}
	delete[]pbCameraBuf;

	/* TODO MW: If we're lazy, we can set this buffer size to some multiple of
	 * our rows */
	// Use max buffer size specified by the scanner to get highest performance
	int iBytesToRead = iPixels * 3 * 10;
	//int iBytesToRead = g_ScanAttr.bufferSize;
	int iBytesRead;
	BYTE *pBuffer = new BYTE[iBytesToRead];
	if (!pBuffer)
		return CloseAndExit(hs);
	fprintf(stderr, "Actual pixels per line : %d\n", iPixels);

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	//png_bytep row = NULL;

	png_ptr =
	    png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fprintf(stderr, "Cannot create png struct\n");
		return 1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fprintf(stderr, "Cannot create info ptr\n");
		return 1;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Cannot create png\n");
		return 1;
	}

	FILE *tempfile;

	if (write_to_file) {
		if (file_to_save_to != NULL) {
			tempfile = fopen((const char *)file_to_save_to, "wb");
			if (!tempfile) {
				fprintf(stderr, "Cannot open %s\n",
					file_to_save_to);
				return 1;
			}
			png_init_io(png_ptr, tempfile);
		} else {
			int res = _setmode(fileno(stdout), _O_BINARY);
			fprintf(stderr, "Changing mode: %d\n", res);
			png_init_io(png_ptr, stdout);
		}

		// Disable compression
		png_set_compression_level(png_ptr, 0);

		int width_px = int (ceil((width / 25.4) * dpi));
		int height_px = int (ceil((height / 25.4) * dpi));
		fprintf(stderr, "width_px: %d; height_px: %d\n", width_px,
			height_px);
		png_set_IHDR(png_ptr, info_ptr, width_px, height_px, 8,
			     PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			     PNG_COMPRESSION_TYPE_DEFAULT,
			     PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);
	}
	//  Enter loop to read data
	//
    fprintf(stderr, "Sleeping\n");
    sleep(2);
    fprintf(stderr, "Sleeping\n");
    sleep(2);
    fprintf(stderr, "Sleeping\n");
    sleep(2);
	do {
		//fprintf(stderr, "Going to read max %u bytes\n", iBytesToRead);
		rc = scanRead(hs,
			      pBuffer,
			      iBytesToRead,
			      SCAN_READSEND_CODE_IMAGE,
			      SCAN_READSEND_QUALIFIER_IMAGE, &iBytesRead);
		total_scanned_bytes += iBytesRead;
		//fprintf(stderr, "Read %u bytes\n", iBytesRead);
		switch (rc) {
		case SCSI_STATUS_GOOD:
			{
				if (write_to_file) {
					for (int i = 0;
					     i < (iBytesRead / (iPixels * 3));
					     i++) {
						png_write_row(png_ptr,
							      pBuffer +
							      (iPixels * 3 *
							       i));
					}
				}
				break;
			}
		case SCSI_STATUS_CHECK_CONDITION:
			{
				int err = scanGetLastAsc(hs);
				switch (err) {
				case ERROR_READ_PAST_END:
				case ERROR_READ_PAST_BEGINNING:
					// Correct end of scan reached
					break;

				default:
					{	// Error while reading data
						char msg[256];
						memset(msg, 0, sizeof(msg));
						scanGetErrorMessage(err, msg,
								    sizeof
								    (msg));
						printf
						    ("Scan ended with error %d: %s\n",
						     err, msg);
					}
					break;
				}
				break;
			}
		default:
			// Error reading data
			break;
		}
	} while (rc == S_OK);

	fprintf(stderr, "Read %ld bytes.\n", total_scanned_bytes);

	if (write_to_file) {
		png_write_end(png_ptr, NULL);

		if (file_to_save_to != NULL) {
			fflush(tempfile);
			fclose(tempfile);
		} else {
			fflush(stdout);
		}

		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
	}

	delete[]pBuffer;

	free(file_to_save_to);

	//
	// Eject paper to back rollers
	//
	if (!bPostScanHandlingSupported)
		if (S_OK !=
		    (rc = scanObjectPosition(hs, SCAN_OBJ_POS_UNLOAD, 0)))
			return DisplayErrorAndExit(hs, rc,
						   "Failed to load media");

	return CloseAndExit(hs);
}
