
// 
// This file contains a late binding interface to the ctx_scan_2000.dll file.
// The library file is designed for third party developers. 
//
// The library supports the API described in 
// SDK 13.0 "CONTEX SCANNER SOFTWARE DEVELOPMENT KIT (SDK) 
//
//

#include <wtypes.h>
#include <stdio.h>
#include "ctx_scan_2000.h"

// Global variables
#ifdef _WIN64
   static const char * const fulllibpath="CtxScan64.dll";
#else
   static const char * const fulllibpath="ctx_scan_2000.dll";
#endif

static BOOL			g_bLoaded = FALSE;
static HINSTANCE	g_hInstance  = NULL;
static DWORD		g_LoadError=0;

#define CHECK_DRIVER_LOADED { if(!g_bLoaded) return SCSI_ERROR_DLL_LOAD_FAILURE; }

// Driver system functions
int (*ptr_scanOpenLib)(void) = NULL; 
int (*ptr_scanCloseLib)(void) = NULL;
int (*ptr_scanRescanScsiBus)(void) = NULL;
// Scanner System functions
int (*ptr_scanGetNextScanner)(HSCANNER *hScanner, BOOL *isOpen, BOOL fRestart) = NULL;
int (*ptr_scanOpenScanner)(HSCANNER hScanner) = NULL;
int (*ptr_scanCloseScanner)(HSCANNER hScanner) = NULL;
int (*ptr_scanGetLastAsc)(HSCANNER hScanner) = NULL;
ctxConnectionType (*ptr_scanGetHardwareType)(HSCANNER hScanner) = NULL;
int (*ptr_scanGetHostIp)(HSCANNER hScanner, char *buffer, BYTE lenBuffer) = NULL;
int (*ptr_scanGetHostName)(HSCANNER hScanner, char *buffer, BYTE lenBuffer) = NULL;
// Scanner I/O functions
int (*ptr_scanTestUnitReady)(HSCANNER hScanner) = NULL;
int (*ptr_scanReserveUnit)(HSCANNER hScanner) = NULL;
int (*ptr_scanReleaseUnit)(HSCANNER hScanner) = NULL;
int (*ptr_scanInquiry)(HSCANNER hScanner, BYTE *buffer, BYTE length) = NULL;
int (*ptr_scanInquiryPage)(HSCANNER hScanner, BYTE *buffer, BYTE length, BYTE page) = NULL;
int (*ptr_scanScan)(HSCANNER hScanner, BYTE *buffer, BYTE length) = NULL;
int (*ptr_scanRead)(HSCANNER hScanner, BYTE *buffer, int iRequested, BYTE DataType, WORD DataTypeQualifier, int *iReceived) = NULL;
int (*ptr_scanSend)(HSCANNER hScanner, BYTE *buffer, int iLength, BYTE DataType, WORD DataTypeQualifier) = NULL;
int (*ptr_scanSetWindow)(HSCANNER hScanner, BYTE *buffer, WORD length) = NULL;
int (*ptr_scanObjectPosition)(HSCANNER hScanner, int function, long absolutePosition) = NULL;
int (*ptr_scanWriteBuffer)(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length) = NULL;
int (*ptr_scanReadBuffer)(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length) = NULL;
int (*ptr_scanReceiveDiagnostic)(HSCANNER hScanner, BYTE *buffer, WORD length) = NULL;
int (*ptr_scanSendDiagnostic)(HSCANNER hScanner, BYTE *buffer, WORD length) = NULL;
// Error Messages
int (*ptr_scanGetErrorMessage)(int iAscAscq, char *message, int szMessage) = NULL;
int (*ptr_scanGetDiagnosticsErrorMessage)(int errorCode, char *message, int szMessage) = NULL;
// Miscellaneous
int (*ptr_scanApplicationAccountInfo)(HSCANNER hScanner, BYTE* buffer) = NULL;
int (*ptr_scanGetScannerNameToDisplay)(HSCANNER hScanner, BYTE *buffer, BYTE length) = NULL;
#define unsigned char BYTE
int (*ptr_logApplicationAccountInfo)(BYTE* name, BYTE* buffer) = NULL;

BOOL LoadAll()
{
   //
   // First we load the library from the  
   //
   // printf("Loading %s\n",fulllibpath);
   if ((g_hInstance = LoadLibrary(fulllibpath)) == NULL)
   {
      g_LoadError = GetLastError();          // Must get error code right away
      return FALSE;
   }

   //
   // Now we load the published functions in ctx_scan_2000.dll
   //

   // Driver system functions
   ptr_scanOpenLib = (int (*)(void))GetProcAddress(g_hInstance, "scanOpenLib");
   ptr_scanCloseLib = (int (*)(void))GetProcAddress(g_hInstance, "scanCloseLib");  
   ptr_scanRescanScsiBus = (int (*)(void)) GetProcAddress(g_hInstance, "scanRescanScsiBus");
   
   // Scanner System functions
   ptr_scanGetNextScanner = (int (*)(HSCANNER *, BOOL *,BOOL )) GetProcAddress(g_hInstance, "scanGetNextScanner");
   ptr_scanOpenScanner = (int (*)(HSCANNER )) GetProcAddress(g_hInstance, "scanOpenScanner");
   ptr_scanCloseScanner = (int (*)(HSCANNER )) GetProcAddress(g_hInstance, "scanCloseScanner");
   ptr_scanGetLastAsc = (int (*)(HSCANNER )) GetProcAddress(g_hInstance, "scanGetLastAsc");
   ptr_scanGetHardwareType = (ctxConnectionType (*)(HSCANNER ))GetProcAddress(g_hInstance, "scanGetHardwareType");
   ptr_scanGetHostIp = (int (*)(HSCANNER , char *, BYTE ))GetProcAddress(g_hInstance, "scanGetHostIp");
   ptr_scanGetHostName = (int (*)(HSCANNER , char *, BYTE ))GetProcAddress(g_hInstance, "scanGetHostName");

   // Scanner I/O functions
   ptr_scanTestUnitReady = (int (*)(HSCANNER ))GetProcAddress(g_hInstance, "scanTestUnitReady");
   ptr_scanReserveUnit = (int (*)(HSCANNER ))GetProcAddress(g_hInstance, "scanReserveUnit");
   ptr_scanReleaseUnit = (int (*)(HSCANNER ))GetProcAddress(g_hInstance, "scanReleaseUnit");
   ptr_scanInquiry = (int (*)(HSCANNER , BYTE *, BYTE ))GetProcAddress(g_hInstance, "scanInquiry");
   ptr_scanInquiryPage = (int (*)(HSCANNER , BYTE *, BYTE , BYTE ))GetProcAddress(g_hInstance, "scanInquiryPage");
   ptr_scanScan = (int (*)(HSCANNER , BYTE *, BYTE ))GetProcAddress(g_hInstance, "scanScan");
   ptr_scanRead = (int (*)(HSCANNER , BYTE *, int , BYTE , WORD , int *))GetProcAddress(g_hInstance, "scanRead");
   ptr_scanSend = (int (*)(HSCANNER , BYTE *, int , BYTE , WORD ))GetProcAddress(g_hInstance, "scanSend");
   ptr_scanSetWindow = (int (*)(HSCANNER , BYTE *, WORD ))GetProcAddress(g_hInstance, "scanSetWindow");
   ptr_scanObjectPosition = (int (*)(HSCANNER , int , long ))GetProcAddress(g_hInstance, "scanObjectPosition");
   ptr_scanWriteBuffer = (int (*)(HSCANNER , BYTE *, BYTE , BYTE , DWORD , DWORD ))GetProcAddress(g_hInstance, "scanWriteBuffer");
   ptr_scanReadBuffer = (int (*)(HSCANNER , BYTE *, BYTE , BYTE , DWORD , DWORD ))GetProcAddress(g_hInstance, "scanReadBuffer");
   ptr_scanReceiveDiagnostic = (int (*)(HSCANNER , BYTE *, WORD ))GetProcAddress(g_hInstance, "scanReceiveDiagnostic");
   ptr_scanSendDiagnostic = (int (*)(HSCANNER , BYTE *, WORD ))GetProcAddress(g_hInstance, "scanSendDiagnostic");
   
   // Error Messages
   ptr_scanGetErrorMessage = (int (*)(int , char *, int ))GetProcAddress(g_hInstance, "scanGetErrorMessage");
   ptr_scanGetDiagnosticsErrorMessage = (int (*)(int , char *, int ))GetProcAddress(g_hInstance, "scanGetDiagnosticsErrorMessage");

   // Miscellaneous
   ptr_scanApplicationAccountInfo = (int (*)(HSCANNER, BYTE*))GetProcAddress(g_hInstance, "scanApplicationAccountInfo");
   ptr_scanGetScannerNameToDisplay = (int (*)(HSCANNER, BYTE*, BYTE))GetProcAddress(g_hInstance, "scanGetScannerNameToDisplay");
   ptr_logApplicationAccountInfo = (int (*)(BYTE*, BYTE*))GetProcAddress(g_hInstance, "logApplicationAccountInfo");
   g_bLoaded = true;
   return g_LoadError == 0;
}


DWORD IsDllLoaded(void)
{
	return g_LoadError;
}

//
// Driver system functions
//
int scanOpenLib(void)
{
   if (!g_bLoaded)
	   LoadAll();
   if ((ptr_scanOpenLib == NULL) )
      return SCSI_ERROR_DLL_LOAD_FAILURE;
   return ptr_scanOpenLib();
}


int scanCloseLib(void)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanCloseLib== NULL) )
      return SCSI_ERROR_DLL_LOAD_FAILURE;
   int rc = ptr_scanCloseLib();
   // Clean up
   FreeLibrary(g_hInstance);
   g_bLoaded = FALSE;
   g_hInstance  = NULL;
   g_LoadError=0;
   return rc;
}

int scanRescanScsiBus(void)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanRescanScsiBus== NULL) )
     return SCSI_ERROR_NO_SCANNER_FOUND;//SCSI_ERROR_DLL_LOAD_FAILURE;
   return ptr_scanRescanScsiBus();
}


//
// Scanner System functions
//
int scanGetNextScanner(HSCANNER *hScanner, BOOL *isOpen, BOOL fRestart)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetNextScanner== NULL) )
		return SCSI_ERROR_NO_SCANNER_FOUND;
	return ptr_scanGetNextScanner(hScanner, isOpen, fRestart);
}

int scanOpenScanner(HSCANNER hScanner)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanOpenScanner== NULL) )
      return SCSI_ERROR_DLL_LOAD_FAILURE;
   return ptr_scanOpenScanner(hScanner);
}

int scanCloseScanner(HSCANNER hScanner)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanCloseScanner== NULL) )
      return SCSI_ERROR_DLL_LOAD_FAILURE;
   return ptr_scanCloseScanner(hScanner);
}

int scanGetLastAsc(HSCANNER hScanner)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanGetLastAsc== NULL) )
      return SCSI_ERROR_DLL_LOAD_FAILURE;
   return ptr_scanGetLastAsc(hScanner);
}

ctxConnectionType scanGetHardwareType(HSCANNER hScanner)
{
	if ((ptr_scanGetHardwareType== NULL) )
		return CTX_CONNECTION_SCSI;
	return ptr_scanGetHardwareType(hScanner);
}

int scanGetHostIp(HSCANNER hScanner, char *buffer, BYTE lenBuffer)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetHostIp== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanGetHostIp(hScanner, buffer, lenBuffer);
}

int scanGetHostName(HSCANNER hScanner, char *buffer, BYTE lenBuffer)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetHostName== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanGetHostName(hScanner, buffer, lenBuffer);
}

//
// Scanner I/O functions
//
int scanTestUnitReady(HSCANNER hScanner)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanTestUnitReady== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanTestUnitReady(hScanner);
}

int scanReserveUnit(HSCANNER hScanner)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanReserveUnit== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanReserveUnit(hScanner);
}

int scanReleaseUnit(HSCANNER hScanner)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanReleaseUnit== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanReleaseUnit(hScanner);
}

int scanInquiry(HSCANNER hScanner, BYTE *buffer, BYTE length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanInquiry== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanInquiry(hScanner, buffer, length);
}

int scanInquiryPage(HSCANNER hScanner, BYTE *buffer, BYTE length, BYTE page)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanInquiryPage== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanInquiryPage(hScanner, buffer, length, page);
}

int scanScan(HSCANNER hScanner, BYTE *buffer, BYTE length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanScan== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanScan(hScanner, buffer, length);
}

int scanRead(HSCANNER hScanner, BYTE *buffer, int iRequested, BYTE DataType, WORD DataTypeQualifier, int *iReceived)
{
	// CHECK_DRIVER_LOADED;   
	if ((ptr_scanRead== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanRead(hScanner, buffer, iRequested, DataType, DataTypeQualifier, iReceived);
}

int scanSend(HSCANNER hScanner, BYTE *buffer, int iLength, BYTE DataType, WORD DataTypeQualifier)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanSend== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanSend(hScanner, buffer, iLength, DataType, DataTypeQualifier);
}

int scanSetWindow(HSCANNER hScanner, BYTE *buffer, WORD length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanSetWindow== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanSetWindow(hScanner, buffer, length);
}

int scanObjectPosition(HSCANNER hScanner, int function, long absolutePosition)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanObjectPosition== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanObjectPosition(hScanner, function, absolutePosition);
}

int scanWriteBuffer(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanWriteBuffer== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanWriteBuffer(hScanner, buffer, mode, bufferId, off, length);
}

int scanReadBuffer(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanReadBuffer== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanReadBuffer(hScanner, buffer, mode, bufferId, off, length);
}

int scanReceiveDiagnostic(HSCANNER hScanner, BYTE *buffer, WORD length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanReceiveDiagnostic== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanReceiveDiagnostic(hScanner, buffer, length);
}

int scanSendDiagnostic(HSCANNER hScanner, BYTE *buffer, WORD length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanSendDiagnostic== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanSendDiagnostic(hScanner, buffer, length);
}

//
// Error Messages
//
int scanGetErrorMessage(int iAscAscq, char *message, int szMessage)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetErrorMessage== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanGetErrorMessage(iAscAscq, message, szMessage);
}

int scanGetDiagnosticsErrorMessage(int errorCode, char *message, int szMessage)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetDiagnosticsErrorMessage== NULL) )
		return SCSI_ERROR_DLL_LOAD_FAILURE;
	return ptr_scanGetDiagnosticsErrorMessage(errorCode, message, szMessage);
}

//
// Miscellaneous
//
void SetName(BYTE* target, BYTE * source, int iLength)
{
	int i = iLength - 1;
	for (; i > 0; i--)		// Remove trailing blanks
	{
		if (source[i] != ' ' && source[i] != '\0')
			break;
	}
	memcpy(target, source, i+1);
	target[i+1] = '\0';	
}

int scanGetScannerNameToDisplay(HSCANNER hScanner, BYTE *buffer, BYTE length)
{
	CHECK_DRIVER_LOADED;   
	if ((ptr_scanGetScannerNameToDisplay== NULL) )
	{
		return ERROR_NOT_SUPPORTED;
	}
	else
		return ptr_scanGetScannerNameToDisplay(hScanner, buffer, length);
}

int logApplicationAccountInfo(BYTE* name, BYTE* buffer)
{
   CHECK_DRIVER_LOADED;    
   if ((logApplicationAccountInfo== NULL) )
      return 0;
   return ptr_logApplicationAccountInfo(name, buffer);
}

int scanApplicationAccountInfo(HSCANNER hScanner, BYTE* buffer)
{
   CHECK_DRIVER_LOADED;   
   if ((ptr_scanApplicationAccountInfo== NULL) )
      return S_OK;
   return ptr_scanApplicationAccountInfo(hScanner, buffer);
}
