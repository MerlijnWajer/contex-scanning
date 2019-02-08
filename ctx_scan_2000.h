#ifndef _CTX_SCAN_2000_H
#define _CTX_SCAN_2000_H

#ifndef _WINDOWS_
#ifndef CTX_SCAN_NOTYPEDEF
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
#endif
#endif

typedef int    HSCANNER;

/* Device driver interface types */
typedef enum{
   CTX_INTERFACE_UNKNOWN=0, // No interface selected
   CTX_INTERFACE_ASPI32,    // use aspi32 access
   CTX_INTERFACE_STI        // Use STI access to scanners
}ctxInterfaceType;

/* Device driver hardware connection types */
typedef enum{
   CTX_CONNECTION_UNKNOWN=0,  // No interface selected
   CTX_CONNECTION_USB,        // uses USB access
   CTX_CONNECTION_SCSI,       // Uses SCSI access to scanners
   CTX_CONNECTION_1394,       // Uses 1394 access to scanners
   CTX_CONNECTION_ETHERNET,   // Uses Ethernet access to scanners
   CTX_CONNECTION_USBCY       // uses Cypress USB access
}ctxConnectionType;

typedef	enum
{
   CTX_NO_EVENT    = 0x00, // No button has been pressed   (0)
   CTX_COPY_EVENT  = 0x01, // The Copy button was pressed  (1)
   CTX_SCAN_EVENT  = 0x02, // The Scan button was pressed  (2)
   CTX_EMAIL_EVENT = 0x03, // The Mail button was pressed  (3)
   CTX_START_EVENT = 0x04, // The Start button was pressed (4)
   CTX_BUTTON1_EVENT = CTX_EMAIL_EVENT,
   CTX_BUTTON2_EVENT = CTX_COPY_EVENT,
   CTX_BUTTON3_EVENT = CTX_SCAN_EVENT,
   CTX_BUTTON4_EVENT = CTX_START_EVENT
} ctxWsEventType;

/* Object Position functions */
#define SCAN_OBJ_POS_UNLOAD                         0
#define SCAN_OBJ_POS_LOAD                           1
#define SCAN_OBJ_POS_ABSOLUTE                       2
#define SCAN_OBJ_POS_RELATIVE                       3

/* Eject Object Count constants */
#define SCAN_EJECT_OBJ_FRONT                        0xFFFFFE
#define SCAN_EJECT_OBJ_BACK                         0xFFFFFF

#define SCAN_OBJ_POST_POS_NOTHING                   0
#define SCAN_OBJ_POST_POS_EJECT_ROLLERS             1
#define SCAN_OBJ_POST_POS_EJECT_BACK                2
#define SCAN_OBJ_POST_POS_LOAD                      3
#define SCAN_OBJ_POST_POS_UNLOAD                    4

/* Modes for scanWriteBuffer & scanReadBuffer */
#define SCAN_BUFFER_VENDOR_SPECIFIC                 0x01
#define SCAN_BUFFER_DATA                            0x02
#define SCAN_BUFFER_DOWNLOAD                        0x04
#define SCAN_BUFFER_DOWNLOAD_SAVE                   0x05

/* Buffers accessed with scanBufferRead/Write */
#define SCAN_BUFFER_ID_VERTICAL_ADJUST_FACTOR       0x00
#define SCAN_BUFFER_ID_HORIZONTAL_ADJUST_FACTOR     0x03
#define SCAN_BUFFER_ID_STITCH_VALUES                0x01
#define SCAN_BUFFER_ID_PAPER_AUTO_LOAD              0x02
#define SCAN_BUFFER_ID_XY_AXIS_OFFSET               0x03
#define SCAN_BUFFER_ID_FLASH_COLOR_SPACE_MATRIX     0x04
#define SCAN_BUFFER_ID_COLOR_CALIBRATION_VALUES     0x05
#define SCAN_BUFFER_ID_WHITE_REF_VALUES             0x06
#define SCAN_BUFFER_ID_RAM_COLOR_SPACE_MATRIX       0x07
#define SCAN_BUFFER_ID_FSX_OFFSETS                  0x08
#define SCAN_BUFFER_ID_PAPER_HANDLING               0x09
#define SCAN_BUFFER_ID_DATA_RATE                    0x0A
#define SCAN_BUFFER_ID_MAX_MOVE_SPEED               0x0B
#define SCAN_BUFFER_ID_DOWNLOAD_MICRO_CODE          0x10
#define SCAN_BUFFER_ID_WRITE_DATA_IOBUFFER          0x13
#define SCAN_BUFFER_ID_NOFLASH_FSX_OFFSETS          0x48

/* Data type codes and data type qualifiers used with the READ and SEND commands */
#define SCAN_READSEND_CODE_IMAGE                    0x00
#define SCAN_READSEND_CODE_PALETTE                  0x01
#define SCAN_READSEND_CODE_FEATURE                  0x01
#define SCAN_READSEND_CODE_HOLLERITH                0x01
#define SCAN_READSEND_CODE_GAMMA                    0x03
#define SCAN_READSEND_CODE_STITCHMODE               0x06
#define SCAN_READSEND_CODE_SCANNER_STATUS           0x80
#define SCAN_READSEND_CODE_TIMER_STATUS             0x81
#define SCAN_READSEND_CODE_SCANNER_STATISTICS       0x82
#define SCAN_READSEND_CODE_CURRENT_SCANLINE         0xFC
#define SCAN_READSEND_CODE_CALIBRATION              0xFD
#define SCAN_READSEND_CODE_WHITE_BACKGROUND         0xFE
#define SCAN_READSEND_CODE_ADJUST                   0xFF
#define SCAN_READSEND_CODE_STITCH_LIMITS            0xFF
#define SCAN_READSEND_CODE_BWPOINT                  0xFA
#define SCAN_READSEND_CODE_QUALITY_MODE             0x86
#define SCAN_READSEND_CODE_CURRENT_WEEK_TIME        0x81
#define SCAN_READSEND_CODE_TIMER_SETTINGS           0x81
#define SCAN_READSEND_CODE_POWER_DOWN_TIME          0x81
#define SCAN_READSEND_CODE_SCANNER_TIME             0x81
#define SCAN_READSEND_CODE_VALIDATION_TIME          0x87
#define SCAN_READSEND_CODE_GOLF_SETTINGS            0x87
#define SCAN_READSEND_CODE_ONETOUCH_SUBSCRIBTION    0x88
#define SCAN_READSEND_CODE_STORAGE_INFO             0x93
#define SCAN_READSEND_CODE_ERROR_LIST               0x92
#define SCAN_READSEND_CODE_POWER_DOWN               0x81

#define SCAN_READSEND_QUALIFIER_SCANNER_STATUS      0x00
#define SCAN_READSEND_QUALIFIER_STOP_SCANNING       0x01
#define SCAN_READSEND_QUALIFIER_RESET_SCAN_SYSTEM   0x02
#define SCAN_READSEND_QUALIFIER_POWER_STATUS        0x03
#define SCAN_READSEND_QUALIFIER_ETHERNET_STATUS     0x04
#define SCAN_READSEND_QUALIFIER_LINEARIZE           0x00
#define SCAN_READSEND_QUALIFIER_STITCHMODE          0x00
#define SCAN_READSEND_QUALIFIER_STITCHCUSTOM        0x03
#define SCAN_READSEND_QUALIFIER_LINEARIZE_WORD      0x02
#define SCAN_READSEND_QUALIFIER_IMAGE               0x00
#define SCAN_READSEND_QUALIFIER_PALETTE             0x10
#define SCAN_READSEND_QUALIFIER_FEATURE             0x20
#define SCAN_READSEND_QUALIFIER_HOLLERITH           0x80
#define SCAN_READSEND_QUALIFIER_HOLLERITH_6G        0x81
#define SCAN_READSEND_QUALIFIER_GAMMA               0x00
#define SCAN_READSEND_QUALIFIER_CUR_LINE_PHYS       0x00
#define SCAN_READSEND_QUALIFIER_CUR_LINE_DOUB       0x01
#define SCAN_READSEND_QUALIFIER_CALIBRATION         0x00
#define SCAN_READSEND_QUALIFIER_WHITE_PHYS          0x00
#define SCAN_READSEND_QUALIFIER_WHITE_DOUB          0x01
#define SCAN_READSEND_QUALIFIER_ADJUST              0x00
#define SCAN_READSEND_QUALIFIER_STITCH_LIMITS       0x01
#define SCAN_READSEND_QUALIFIER_SCANNER_STATISTICS  0x00
#define SCAN_READSEND_QUALIFIER_QUALITY_MODE        0x00
#define SCAN_READSEND_QUALIFIER_CURRENT_WEEK_TIME   0x01
#define SCAN_READSEND_QUALIFIER_TIMER_SETTINGS      0x02
#define SCAN_READSEND_QUALIFIER_POWERDOWN           0x03
#define SCAN_READSEND_QUALIFIER_POWER_DOWN_TIME     0x04
#define SCAN_READSEND_QUALIFIER_VALIDATION_TIME     0x00
#define SCAN_READSEND_QUALIFIER_GOLF_SETTINGS       0x01
#define SCAN_READSEND_QUALIFIER_ONETOUCH_SUBSCRIBTION 0x00
#define SCAN_READSEND_QUALIFIER_ONETOUCH_TARGET     0x01
#define SCAN_READSEND_QUALIFIER_ONETOUCH_BITMAP     0x02
#define SCAN_READSEND_QUALIFIER_ERROR_LIST          0x00
#define SCAN_READSEND_QUALIFIER_SCANNER_TIME        0x06
#define SCAN_READSEND_QUALIFIER_STORAGE_INFO        0x00
#define SCAN_READSEND_QUALIFIER_LAMP_ON             0x31
#define SCAN_READSEND_QUALIFIER_LAMP_OFF            0x32

#define GET_BUFFER_STATUS_LENGTH                    0x11

/* SET_WINDOW definitions */
#define SCAN_SETWINDOW_BW                           0x00
#define SCAN_SETWINDOW_GRAY                         0x01
#define SCAN_SETWINDOW_RGB                          0x05

/* Page codes for INQUIRY  */
#define SCAN_INQUIRY_PAGE_PAGES_SUPPORTED           0x00
#define SCAN_INQUIRY_PAGE_REVISION                  0xC0
#define SCAN_INQUIRY_PAGE_AUTO_CONFIG               0xC1
#define SCAN_INQUIRY_PAGE_VAR_RESOLUTION            0xC2
#define SCAN_INQUIRY_PAGE_FIXED_RESOLUTION          0xC3
#define SCAN_INQUIRY_PAGE_COLOR                     0xC4
#define SCAN_INQUIRY_PAGE_CAMERA_POSITION           0xC5
#define SCAN_INQUIRY_PAGE_CALIBRATION_SUPPORT       0xC6
#define SCAN_INQUIRY_PAGE_DEVICE_INFORMATION        0xC7
#define SCAN_INQUIRY_PAGE_DISPLAY_INFORMATION       0xC8
#define SCAN_INQUIRY_PAGE_BOOT                      0xFF

/******************* SCSI Status codes ************************************/
#define SCSI_STATUS_GOOD                            0x00
#define SCSI_STATUS_CHECK_CONDITION                 0x02
#define SCSI_STATUS_CONDITION_MET                   0x04
#define SCSI_STATUS_BUSY                            0x08
#define SCSI_STATUS_INTERMIDIATE                    0x10
#define SCSI_STATUS_INTERMIDIATE_CON_MET            0x14
#define SCSI_STATUS_RESERVATION_CONFLICT            0x22
#define SCSI_STATUS_QUEUE_FULL                      0x28

/******************* SCSI Error codes *************************************/
#define SCSI_ERROR_DLL_LOAD_FAILURE                 -99         /* Failed to load ctx_scan_2000.dll */

#define SCSI_ERROR_PARM_ERROR                       -100        /* Parameter error in CTX_SCAN call*/
#define SCSI_ERROR_NO_BOARD                         -110        /* CTX_SCAN could not find a SCSI interface board */
#define SCSI_ERROR_NO_DRIVER                        -111        /* CTX_SCAN could not load a SCSI driver (like WIA) */
#define SCSI_ERROR_PROTOCOL                         -112        /* A SCSI protocol error was detected */
#define SCSI_ERROR_SYSTEM                           -113        /* An operation system error was detected */
#define SCSI_ERROR_IO_NOT_COMPLETE                  -114        /* The IO command has not yet completed */
#define SCSI_ERROR_CTX_DRIVER                       -115        /* An internal CTX_xxx driver error occured */
#define SCSI_ERROR_DRIVER_TO_OLD                    -116        /* The driver specified was too old */
#define SCSI_ERROR_NO_SCANNER_FOUND                 -117        /* No Contex scanner was found. */
#define SCSI_ERROR_UNABLE_TO_OPEN_DRIVER            -118        /* It was not possible to open the driver */
#define SCSI_ERROR_SCANNERS_STILL_OPEN              -119        /* It was not possible to close the library, because a scanner is not closed*/
#define SCSI_ERROR_UDP_TIMEOUT                      -120        /* Timeout on Ethernet connection to scanner */
#define SCSI_ERROR_INTERNET_ERROR                   -121        /* Error occurred during internet activation */ 

#define SCSI_ERROR_SCANNER_ALREADY_OPEN             -150        /* The calling program tried to open an already open scanner */
#define SCSI_ERROR_SCANNER_NOT_OPEN                 -151        /* The calling program tried to do I/O onn closed scanner */
#define SCSI_ERROR_SCANNER_ALREADY_CLOSED           -152        /* The calling program tried to close an already closed scanner */
#define SCSI_ERROR_UNKNOWN_HANDLE                   -153        /* The calling program tried to open an unknown handle */

#define ERROR_NO_ADDITIONAL                         0x0000
#define ERROR_COMMAND_PHASE                         0x4A00
#define ERROR_OVERLAPPED_COMMAND_PHASE              0x4E00
#define ERROR_INVALID_COMMAND_OPERATION_CODE        0x2000
#define ERROR_INVALID_VALUE_IN_CDB_2	             0x2400
#define ERROR_SCANNER_NEEDS_ACTIVATION              0x2610
#define ERROR_COMMAND_SEQ                           0x2C00
#define ERROR_DATAPHASE                             0x4B00
#define ERROR_EJECT_FAILED                          0x5300
#define ERROR_MEDIA_NOT_PRESENT                     0x3A00

#define ERROR_MEDIA_NOT_LOADED                      0x3A80
#define ERROR_MEDIA_LOAD_ABORT                      0x3A81
#define ERROR_MEDIA_LOAD_PROGRESS                   0x3A82
#define ERROR_OBJECT_POSITION_PROGRESS              0x3A83
#define ERROR_OBJ_POS_STOPPED_PAPER_JAM             0x3A84
#define ERROR_MEDIA_MUST_MANUALLY_BE_RELOADED       0x3A85
#define ERROR_BASIC_CALIBRATION_PROGRESS            0x3A86
#define ERROR_SCANNER_LID_IS_OPEN                   0x3A88

#define ERROR_MESSAGE                               0x4300
#define ERROR_PARMLISTLENGTH                        0x1A00
#define ERROR_INVALID_FIELD_IN_PARM_LIST            0x2600
#define ERROR_PARNNOTSUPP                           0x2601
#define ERROR_INVALID_PARM                          0x2602
#define ERROR_READ_PAST_BEGINNING                   0x3B0A
#define ERROR_READ_PAST_END                         0x3B09
#define ERROR_SELFTEST                              0x4200
#define ERROR_SCSIPARITY                            0x4700
#define ERROR_SELECT                                0x4500
#define ERROR_TO_MANY_WINDOWS                       0x2C01
#define ERROR_WARMING_UP                            0x2900

#define ERROR_UNKNOWN                               0x9001
#define ERROR_WHITE_CAM_A                           0x9101
#define ERROR_WHITE_CAM_B                           0x9102
#define ERROR_WHITE_CAM_C                           0x9103
#define ERROR_WHITE_CAM_D                           0x9104
#define ERROR_WHITE_CAM_E                           0x9105
#define ERROR_WHITE_CAM_F                           0x9106
#define ERROR_BLACK_CAM_A                           0x9111
#define ERROR_BLACK_CAM_B                           0x9112
#define ERROR_BLACK_CAM_C                           0x9113
#define ERROR_BLACK_CAM_D                           0x9114
#define ERROR_BLACK_CAM_E                           0x9115
#define ERROR_BLACK_CAM_F                           0x9116
#define ERROR_STITCH_AB                             0x9121
#define ERROR_STITCH_BC                             0x9122
#define ERROR_STITCH_CD                             0x9123
#define ERROR_STITCH_DE                             0x9124
#define ERROR_STITCH_EF                             0x9125
#define ERROR_CALIB                                 0x9130
#define ERROR_CALIB_A_R                             0x9131
#define ERROR_CALIB_A_G                             0x9132
#define ERROR_CALIB_A_B                             0x9133
#define ERROR_CALIB_B_R                             0x9134
#define ERROR_CALIB_B_G                             0x9135
#define ERROR_CALIB_B_B                             0x9136
#define ERROR_CALIB_C_R                             0x9137
#define ERROR_CALIB_C_G                             0x9138
#define ERROR_CALIB_C_B                             0x9139
#define ERROR_CALIB_D_R                             0x913A
#define ERROR_CALIB_D_G                             0x913B
#define ERROR_CALIB_D_B                             0x913C
#define ERROR_CALIB_E_R                             0x913D
#define ERROR_CALIB_E_G                             0x913E
#define ERROR_CALIB_E_B                             0x913F
#define ERROR_CALIB_F_R                             0x9140
#define ERROR_CALIB_F_G                             0x9141
#define ERROR_CALIB_F_B                             0x9142
#define ERROR_CALIB_UNABLE_TO_MATCH_BLACK_LEVELS    0x9145
#define ERROR_FLASH                                 0x9150
#define ERROR_CAMERA_A_ADJUST_FAILED                0x91F1
#define ERROR_CAMERA_B_ADJUST_FAILED                0x91F2
#define ERROR_CAMERA_C_ADJUST_FAILED                0x91F3
#define ERROR_CAMERA_D_ADJUST_FAILED                0x91F4
#define ERROR_CAMERA_E_ADJUST_FAILED                0x91F5
#define ERROR_CAMERA_F_ADJUST_FAILED                0x91F6
#define ERROR_CONTROLLER_DITHER_RAM_RW              0x9201
#define ERROR_CONTROLLER_AMP_RAM_RW                 0x9202
#define ERROR_EXT_INT                               0x9301
#define ERROR_SERIAL_INT                            0x9302
#define ERROR_SOFTWARE_TIMER_INT                    0x9303
#define ERROR_HSO_INT                               0x9304
#define ERROR_HSI_INT                               0x9305
#define ERROR_AD_INT                                0x9306
#define ERROR_TIMER_INT                             0x9307
#define ERROR_SHIFT_INT                             0x9308
#define ERROR_CTRL_PRG                              0x9309
#define ERROR_STATE                                 0x930A

#define ERROR_NO_IN_TEST_MODE                       0xA000
#define ERROR_IN_TEST_MODE                          0xA001
#define ERROR_END_OF_TILE                           0xA002
#define ERROR_TIME_OUT_ON_REQUEST                   0xA003
#define ERROR_SOEM_TIMEOUT                          0xA004
#define ERROR_NOT_PCEPC                             0xA005
#define ERROR_PCEPC_ERR                             0xA006
#define ERROR_FATAL_OS_ERROR                        0xA010
#define ERROR_FATAL_SCSI_SYSTEM                     0xA011
#define ERROR_FATAL_IN_FIRMWARE                     0xA012
#define ERROR_FATAL_NO_SMARTCARD                    0xA013
#define ERROR_FATAL_INVALID_SMARTCARD               0xA014
#define ERROR_IN_POWER_DOWN_MODE                    0xA015
#define ERROR_KBD_SWITCH                            0xB000
#define ERROR_SCB                                   0xB001
#define ERROR_CCB                                   0xB002
#define ERROR_EEPROM                                0xB003
#define ERROR_FLASH_VPP_LOW                         0xB004
#define ERROR_FLASH_UNABLE_TO_ERASE                 0xB005
#define ERROR_FLASH_UNABLE_TO_PROGRAM               0xB006
#define ERROR_SCU                                   0xB007
#define ERROR_CCF                                   0xB008
#define ERROR_PPU                                   0xB009
#define ERROR_SFD                                   0xB00A
#define ERROR_UNUSABLE_DSP                          0xC000
#define ERROR_FATALDSP                              0xC001
#define ERROR_HOLERITH_DATA_NOT_READY               0xD000
#define ERROR_ABORT                                 0xD002
#define ERROR_COVER_OPEN                            0xD001

/******************* Software  scanner related Error codes *************************************/
#define ERROR_SWS                                   0xE000
#define ERROR_PIXELS_LAYOUT                         0xE004
#define ERROR_TIMEOUT_EMPTY_BUFFERS                 0xE005

/******************* Gigabit Ethernet scanner related Error codes ******************************/
#define ERROR_MAX_SCANNER_CONNECTIONS               0xE003

#define ERROR_IN_BLACK_REFERENCE                    0xE009
#define ERROR_CABLE_OR_CONNECTION                   0xE00A
#define ERROR_NO_SCANNER_POWER                      0xE00B

/******************* STI Error codes *************************************/
#define STI_ERROR_LOCK_FAILED                   -1001        /* The calling program failed to lock device*/
#define STI_ERROR_UNLOCK_FAILED                 -1002        /* The calling program failed to unlock device*/
#define STI_ERROR_THREAD_COMMAND_TIMEOUT        -1003        /* The STI thread failed to return within 30 secs  */
#define STI_ERROR_THREAD_COMMAND_FAILED         -1004        /* The STI thread failed  */
#define STI_ERROR_USB_CRC_ERRORCODE23           -1005        /* The connection was lost or failed  */
#define STI_ERROR_SCANNER_NOT_OPEN               SCSI_ERROR_SCANNER_NOT_OPEN

#ifdef __cplusplus
extern "C" {
#endif


int scanOpenLib();
int scanCloseLib(void);
int scanRescanScsiBus(void);

int scanGetNextScanner(HSCANNER *hScanner, BOOL *isOpen, BOOL fRestart);
int scanOpenScanner(HSCANNER hScanner);
int scanCloseScanner(HSCANNER hScanner);
int scanGetLastAsc(HSCANNER hScanner);
ctxConnectionType scanGetHardwareType(HSCANNER hScanner);
int scanGetHostIp(HSCANNER hScanner, char *buffer, BYTE lenBuffer);
int scanGetHostName(HSCANNER hScanner, char *buffer, BYTE lenBuffer);

int scanTestUnitReady(HSCANNER hScanner);
int scanReserveUnit(HSCANNER hScanner);
int scanReleaseUnit(HSCANNER hScanner);
int scanInquiry(HSCANNER hScanner, BYTE *buffer, BYTE length);
int scanInquiryPage(HSCANNER hScanner, BYTE *buffer, BYTE length, BYTE page);
int scanScan(HSCANNER hScanner, BYTE *buffer, BYTE length);
int scanRead(HSCANNER hScanner, BYTE *buffer, int iRequested, BYTE DataType, WORD DataTypeQualifier, int *iReceived);
int scanSend(HSCANNER hScanner, BYTE *buffer, int iLength, BYTE DataType, WORD DataTypeQualifier);
int scanSetWindow(HSCANNER hScanner, BYTE *buffer, WORD length);
int scanObjectPosition(HSCANNER hScanner, int function, long absolutePosition);
int scanWriteBuffer(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length);
int scanReadBuffer(HSCANNER hScanner, BYTE *buffer, BYTE mode, BYTE bufferId, DWORD off, DWORD length);
int scanReceiveDiagnostic(HSCANNER hScanner, BYTE *buffer, WORD length);
int scanSendDiagnostic(HSCANNER hScanner, BYTE *buffer, WORD length);

int scanGetErrorMessage(int iAscAscq, char *message, int szMessage);
int scanGetDiagnosticsErrorMessage(int errorCode, char *message, int szMessage);

int scanGetScannerNameToDisplay(HSCANNER hScanner, BYTE *buffer, BYTE length);
int scanApplicationAccountInfo(HSCANNER hScanner, BYTE *buffer);
int logApplicationAccountInfo(BYTE* name, BYTE* buffer);

#ifdef __cplusplus
}
#endif

#endif // _CTX_SCAN_2000_H
