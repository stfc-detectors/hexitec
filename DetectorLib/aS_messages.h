//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x100
#define FACILITY_NI_SYSTEM               0x300
#define FACILITY_NI_IMAQ                 0x200
#define FACILITY_AS_FG                   0x1
#define FACILITY_AS_CONVERT              0x2
#define FACILITY_AS                      0x0


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: AS_NO_ERROR
//
// MessageText:
//
// No error occurred (%1!d!).
//
#define AS_NO_ERROR                      ((DWORD)0x00000000L)

//
// MessageId: AS_MUTEX_NOT_LONGER_VALID
//
// MessageText:
//
// MUTEX Object is not longer valid (%1!d!). 
//
#define AS_MUTEX_NOT_LONGER_VALID        ((DWORD)0xC0000001L)

//
// MessageId: AS_WAIT_TIMEOUT
//
// MessageText:
//
// Timeout occurred at wait for Object (%1!d!).
//
#define AS_WAIT_TIMEOUT                  ((DWORD)0xC0000002L)

//
// MessageId: AS_INVALID_MUTEX
//
// MessageText:
//
// MUTEX Object is not valid (%1!d!). 
//
#define AS_INVALID_MUTEX                 ((DWORD)0xC0000003L)

//
// MessageId: AS_INSTANCE_BUSY
//
// MessageText:
//
// Instance already in use (%1!d!). 
//
#define AS_INSTANCE_BUSY                 ((DWORD)0xC0000004L)

//
// MessageId: AS_INSTANCE_READY
//
// MessageText:
//
// Instance not in use (%1!d!). 
//
#define AS_INSTANCE_READY                ((DWORD)0xC0000005L)

//
// MessageId: AS_UNKNOWN_INSTANCE
//
// MessageText:
//
// Unknown instance specified (%1!d!). 
//
#define AS_UNKNOWN_INSTANCE              ((DWORD)0xC0000006L)

//
// MessageId: AS_FG_CREATE_EVENT_ERR
//
// MessageText:
//
// Event-handle could not be created (FrameGrabberLib: %1!d!). 
//
#define AS_FG_CREATE_EVENT_ERR           ((DWORD)0xC0010001L)

//
// MessageId: AS_FG_INTERFACE_OPEN_ERR
//
// MessageText:
//
// Framegrabber-Interface could not be opened (%1!d!). 
//
#define AS_FG_INTERFACE_OPEN_ERR         ((DWORD)0xC2000001L)

//
// MessageId: AS_NI_SYS_INVALID_MSG_BUFFER
//
// MessageText:
//
// Message buffer for NI error Message too small (min. 256 bytes)(%1!d!). 
//
#define AS_NI_SYS_INVALID_MSG_BUFFER     ((DWORD)0xC3000001L)

//
// MessageId: AS_FG_CREATE_SEMAPHORE_ERR
//
// MessageText:
//
// Semaphore could not be created (FrameGrabberLib: %1!d!). 
//
#define AS_FG_CREATE_SEMAPHORE_ERR       ((DWORD)0xC0010003L)

//
// MessageId: AS_FG_CREATE_THREAD_ERR
//
// MessageText:
//
// Thread could not be created (FrameGrabberLib: %1!d!). 
//
#define AS_FG_CREATE_THREAD_ERR          ((DWORD)0xC0010004L)

//
// MessageId: AS_FG_WAIT_OBJECT_ERR
//
// MessageText:
//
// Wait for object failed (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WAIT_OBJECT_ERR            ((DWORD)0xC0010005L)

//
// MessageId: AS_FG_INVALID_THREAD_CNT
//
// MessageText:
//
// The parameter "threadCnt" is out of Range (FrameGrabberLib: %1!d!). 
//
#define AS_FG_INVALID_THREAD_CNT         ((DWORD)0xC0010006L)

//
// MessageId: AS_FG_INVALID_INDEX
//
// MessageText:
//
// The parameter "idx" is greater than the array size of the parameter "fgIfNames" (FrameGrabberLib: %1!d!). 
//
#define AS_FG_INVALID_INDEX              ((DWORD)0xC0010007L)

//
// MessageId: AS_FG_INVALID_FGIFNAMES_SIZE
//
// MessageText:
//
// The array "fgIfNames" have to be greater than 0 (FrameGrabberLib: %1!d!). 
//
#define AS_FG_INVALID_FGIFNAMES_SIZE     ((DWORD)0xC0010008L)

//
// MessageId: AS_CONVERT_TO_STRING_ERR
//
// MessageText:
//
// Number to string convertion failed (%1!d!). 
//
#define AS_CONVERT_TO_STRING_ERR         ((DWORD)0xC0020001L)

//
// MessageId: AS_FG_SESSION_OPEN_ERR
//
// MessageText:
//
// Framegrabber-Session could not be opened (%1!d!). 
//
#define AS_FG_SESSION_OPEN_ERR           ((DWORD)0xC2000002L)

//
// MessageId: AS_FG_SESSION_FIT_ROI_ERR
//
// MessageText:
//
// ROI could not be properly aligned (%1!d!). 
//
#define AS_FG_SESSION_FIT_ROI_ERR        ((DWORD)0xC2000003L)

//
// MessageId: AS_FG_SESSION_CONFIGURE_ROI_ERR
//
// MessageText:
//
// ROI could not be set (%1!d!). 
//
#define AS_FG_SESSION_CONFIGURE_ROI_ERR  ((DWORD)0xC2000004L)

//
// MessageId: AS_FG_GET_BUFFER_SIZE_ERR
//
// MessageText:
//
// Needed buffer size could not determined (%1!d!). 
//
#define AS_FG_GET_BUFFER_SIZE_ERR        ((DWORD)0xC2000005L)

//
// MessageId: AS_FG_CREATE_BUFFER_LIST_ERR
//
// MessageText:
//
// Buffer List could not be created (%1!d!). 
//
#define AS_FG_CREATE_BUFFER_LIST_ERR     ((DWORD)0xC2000006L)

//
// MessageId: AS_FG_CREATE_BUFFER_ERR
//
// MessageText:
//
// Buffer could not be created (%1!d!). 
//
#define AS_FG_CREATE_BUFFER_ERR          ((DWORD)0xC2000007L)

//
// MessageId: AS_FG_SET_BUFFER_ADDRESS_ERR
//
// MessageText:
//
// Buffer address could not be set (%1!d!). 
//
#define AS_FG_SET_BUFFER_ADDRESS_ERR     ((DWORD)0xC2000008L)

//
// MessageId: AS_FG_SET_BUFFER_SIZE_ERR
//
// MessageText:
//
// Buffer size could not be set (%1!d!). 
//
#define AS_FG_SET_BUFFER_SIZE_ERR        ((DWORD)0xC2000009L)

//
// MessageId: AS_FG_SET_BUFFER_COMMAND_ERR
//
// MessageText:
//
// Buffer command could not be set (%1!d!). 
//
#define AS_FG_SET_BUFFER_COMMAND_ERR     ((DWORD)0xC200000AL)

//
// MessageId: AS_FG_SESSION_CONFIGURE_ERR
//
// MessageText:
//
// Session could not be configured (%1!d!). 
//
#define AS_FG_SESSION_CONFIGURE_ERR      ((DWORD)0xC200000BL)

//
// MessageId: AS_NULL_POINTER
//
// MessageText:
//
// One parameter is a NULL pointer which should not be (%1!d!). 
//
#define AS_NULL_POINTER                  ((DWORD)0xC0000007L)

//
// MessageId: AS_FG_NOT_INITIALIZED
//
// MessageText:
//
// The framegrabber instance is not initialized (FrameGrabberLib: %1!d!). 
//
#define AS_FG_NOT_INITIALIZED            ((DWORD)0xC0010009L)

//
// MessageId: AS_FG_DISPOSE_BUFFER_LIST_ERR
//
// MessageText:
//
// Dispose buffer list failed (%1!d!). 
//
#define AS_FG_DISPOSE_BUFFER_LIST_ERR    ((DWORD)0xC200000CL)

//
// MessageId: AS_FG_GET_ROI_ERR
//
// MessageText:
//
// Get ROI failed (%1!d!). 
//
#define AS_FG_GET_ROI_ERR                ((DWORD)0xC200000DL)

//
// MessageId: AS_FG_SESSION_NOT_CONFIGURED
//
// MessageText:
//
// Session is not configured (FrameGrabberLib: %1!d!). 
//
#define AS_FG_SESSION_NOT_CONFIGURED     ((DWORD)0xC001000AL)

//
// MessageId: AS_FG_WRONG_DEST_IMG_PARAMETER
//
// MessageText:
//
// Invalid properties of destination image (image size | image type)(FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_DEST_IMG_PARAMETER   ((DWORD)0xC001000BL)

//
// MessageId: AS_FG_WRONG_CORRECT_IMG_PARAMETER
//
// MessageText:
//
// Invalid properties of correction image (image size | image type)(FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_CORRECT_IMG_PARAMETER ((DWORD)0xC001000CL)

//
// MessageId: AS_FG_CREATE_PULSE
//
// MessageText:
//
// Pulse creation failed (%1!d!). 
//
#define AS_FG_CREATE_PULSE               ((DWORD)0xC200000EL)

//
// MessageId: AS_FG_TRIGGER_CONFIGURE
//
// MessageText:
//
// Trigger configuration failed (%1!d!). 
//
#define AS_FG_TRIGGER_CONFIGURE          ((DWORD)0xC200000FL)

//
// MessageId: AS_FG_TRIGGER_DRIVE_ERR
//
// MessageText:
//
// Drive trigger failed (%1!d!). 
//
#define AS_FG_TRIGGER_DRIVE_ERR          ((DWORD)0xC2000010L)

//
// MessageId: AS_FG_PULSE_DISPOSE_ERR
//
// MessageText:
//
// Dispose pulse failed (%1!d!). 
//
#define AS_FG_PULSE_DISPOSE_ERR          ((DWORD)0xC2000011L)

//
// MessageId: AS_FG_EXT_TRIGGER_OUT_ERR
//
// MessageText:
//
// External trigger out not configured (FrameGrabberLib: %1!d!). 
//
#define AS_FG_EXT_TRIGGER_OUT_ERR        ((DWORD)0xC001000DL)

//
// MessageId: AS_FG_SESSION_START_ACQ_ERR
//
// MessageText:
//
// Acquisition start failed (%1!d!). 
//
#define AS_FG_SESSION_START_ACQ_ERR      ((DWORD)0xC2000012L)

//
// MessageId: AS_FG_PULSE_START_ERR
//
// MessageText:
//
// Pulse start failed (%1!d!). 
//
#define AS_FG_PULSE_START_ERR            ((DWORD)0xC2000013L)

//
// MessageId: AS_FG_PULSE_STOP_ERR
//
// MessageText:
//
// Pulse stop failed (%1!d!). 
//
#define AS_FG_PULSE_STOP_ERR             ((DWORD)0xC2000014L)

//
// MessageId: AS_RELEASE_SEMAPHORE_ERR
//
// MessageText:
//
// Release Semaphore failed (%1!d!). 
//
#define AS_RELEASE_SEMAPHORE_ERR         ((DWORD)0xC1000001L)

//
// MessageId: AS_FG_WRONG_FUNCTION_NUMBER
//
// MessageText:
//
// Function number is out of Range (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_FUNCTION_NUMBER      ((DWORD)0xC001000EL)

//
// MessageId: AS_FG_EXAMINE_BUFFER_ERR
//
// MessageText:
//
// Examine buffer failed (%1!d!). 
//
#define AS_FG_EXAMINE_BUFFER_ERR         ((DWORD)0xC2000015L)

//
// MessageId: AS_FG_RELEASE_BUFFER_ERR
//
// MessageText:
//
// Release buffer failed (%1!d!). 
//
#define AS_FG_RELEASE_BUFFER_ERR         ((DWORD)0xC2000016L)

//
// MessageId: AS_FG_GET_ATTRIBUTE_ERR
//
// MessageText:
//
// Get attribute failed (%1!d!). 
//
#define AS_FG_GET_ATTRIBUTE_ERR          ((DWORD)0xC2000017L)

//
// MessageId: AS_FG_RETURNED_BUFFER_ERR
//
// MessageText:
//
// Returned buffer does not match the requested buffer (FrameGrabberLib: %1!d!). 
//
#define AS_FG_RETURNED_BUFFER_ERR        ((DWORD)0xC001000FL)

//
// MessageId: AS_FG_LOST_FRAMES_OCCURED
//
// MessageText:
//
// Lost frames occured (FrameGrabberLib: %1!d!). 
//
#define AS_FG_LOST_FRAMES_OCCURED        ((DWORD)0xC0010010L)

//
// MessageId: AS_FG_DIV_EQUAL_0
//
// MessageText:
//
// Divide by zero not possible (FrameGrabberLib: %1!d!). 
//
#define AS_FG_DIV_EQUAL_0                ((DWORD)0xC0010011L)

//
// MessageId: AS_FG_WRONG_DEST_BUFFER_PARAMETER
//
// MessageText:
//
// Buffer pointer NULL or buffer to small (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_DEST_BUFFER_PARAMETER ((DWORD)0xC0010012L)

//
// MessageId: AS_FG_SERIAL_FLUSH_ERR
//
// MessageText:
//
// SerialFlush over framegrabber error (%1!d!). 
//
#define AS_FG_SERIAL_FLUSH_ERR           ((DWORD)0xC2000018L)

//
// MessageId: AS_FG_SERIAL_WRITE_ERR
//
// MessageText:
//
// SerialWrite over framegrabber failed (%1!d!). 
//
#define AS_FG_SERIAL_WRITE_ERR           ((DWORD)0xC2000019L)

//
// MessageId: AS_FG_SERIAL_READ_ERR
//
// MessageText:
//
// SerialRead over framegrabber failed (%1!d!). 
//
#define AS_FG_SERIAL_READ_ERR            ((DWORD)0xC200001AL)

//
// MessageId: AS_FG_WRONG_MASK_LEN
//
// MessageText:
//
// Invalid length of mask array (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_MASK_LEN             ((DWORD)0xC0010013L)

//
// MessageId: AS_FG_WAIT_SIGNAL_ASYNC_ERR
//
// MessageText:
//
// Configure wait for signal failed (%1!d!). 
//
#define AS_FG_WAIT_SIGNAL_ASYNC_ERR      ((DWORD)0xC200001BL)

//
// MessageId: AS_OPENFILE_ERR
//
// MessageText:
//
// File open failed (%1!d!). 
//
#define AS_OPENFILE_ERR                  ((DWORD)0xC1000002L)

//
// MessageId: AS_GET_FILE_SIZE_ERR
//
// MessageText:
//
// GetFileSize failed (%1!d!). 
//
#define AS_GET_FILE_SIZE_ERR             ((DWORD)0xC1000003L)

//
// MessageId: AS_READ_FILE_ERR
//
// MessageText:
//
// ReadFile failed (%1!d!). 
//
#define AS_READ_FILE_ERR                 ((DWORD)0xC1000004L)

//
// MessageId: AS_FG_WRONG_DARK_IMG_PARAMETER
//
// MessageText:
//
// Invalid properties of dark correction image (image size | image type)(FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_DARK_IMG_PARAMETER   ((DWORD)0xC0010014L)

//
// MessageId: AS_SYS_CREATE_EVENT_ERR
//
// MessageText:
//
// Event-handle could not be created (%1!d!). 
//
#define AS_SYS_CREATE_EVENT_ERR          ((DWORD)0xC1000005L)

//
// MessageId: AS_SYS_RESET_EVENT_ERR
//
// MessageText:
//
// Event-Reset failed (%1!d!). 
//
#define AS_SYS_RESET_EVENT_ERR           ((DWORD)0xC1000006L)

//
// MessageId: AS_SYS_SET_EVENT_ERR
//
// MessageText:
//
// Event-Set failed (%1!d!). 
//
#define AS_SYS_SET_EVENT_ERR             ((DWORD)0xC1000007L)

//
// MessageId: AS_SYS_CREATE_DIR_ERR
//
// MessageText:
//
// Create directory failed (%1!d!). 
//
#define AS_SYS_CREATE_DIR_ERR            ((DWORD)0xC1000008L)

//
// MessageId: AS_CREATEFILE_ERR
//
// MessageText:
//
// Create file failed (%1!d!). 
//
#define AS_CREATEFILE_ERR                ((DWORD)0xC1000009L)

//
// MessageId: AS_WRITEFILE_ERR
//
// MessageText:
//
// Write file failed (%1!d!). 
//
#define AS_WRITEFILE_ERR                 ((DWORD)0xC100000AL)

//
// MessageId: AS_CLOSEFILE_ERR
//
// MessageText:
//
// Close file failed (%1!d!). 
//
#define AS_CLOSEFILE_ERR                 ((DWORD)0xC100000BL)

//
// MessageId: AS_FINDFILE_ERR
//
// MessageText:
//
// Find file failed (%1!d!). 
//
#define AS_FINDFILE_ERR                  ((DWORD)0xC100000CL)

//
// MessageId: AS_FG_WRONG_IMG_CNT
//
// MessageText:
//
// Invalid image count (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_IMG_CNT              ((DWORD)0xC0010015L)

//
// MessageId: AS_FG_WRONG_ROI_PARAMETER
//
// MessageText:
//
// Invalid properties of region of interest (ROI Start | ROI Size | Read Out Start)(FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_ROI_PARAMETER        ((DWORD)0xC0010016L)

//
// MessageId: AS_FG_WRONG_SENSOR_TYPE
//
// MessageText:
//
// Invalid sensor type (FrameGrabberLib: %1!d!). 
//
#define AS_FG_WRONG_SENSOR_TYPE          ((DWORD)0xC0010017L)

//
// MessageId: AS_FG_SET_ATTRIBUTE_ERR
//
// MessageText:
//
// Set attribute failed (%1!d!). 
//
#define AS_FG_SET_ATTRIBUTE_ERR          ((DWORD)0xC200001CL)

//
// MessageId: AS_FG_SET_CAMERAFILE_ERR
//
// MessageText:
//
// Change camera file failed (%1!d!). 
//
#define AS_FG_SET_CAMERAFILE_ERR         ((DWORD)0xC200001DL)

//
// MessageId: AS_FG_STOP_ACQ_ERR
//
// MessageText:
//
// Stop acquisition failed (%1!d!). 
//
#define AS_FG_STOP_ACQ_ERR               ((DWORD)0xC200001EL)

