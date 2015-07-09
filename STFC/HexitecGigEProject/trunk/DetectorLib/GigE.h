// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GIGE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GIGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GIGE_EXPORTS
#define GIGE_API __declspec(dllexport)
#else
#define GIGE_API __declspec(dllimport)
#endif

// function typedef for CallBack function to signalize a filled transfer buffer to a third party application.
typedef void (__cdecl *p_bufferCallBack)(															// todo add ImageInfo (Size...)
												PUCHAR				aTransferBuffer,				// pointer to current transfer buffer
												ULONG				aImageCount );					// number of valid frames within the transfer buffer

// create device instance
EXTERN_C	GIGE_API	LONG	InitDevice(
												HANDLE				*deviceHdl,						// handle for the device
												CONST LPSTR			aDeviceDescriptor);				// empty for first available Pleora GigE Device, IP-Address or MAC-Address

// close device instance
EXTERN_C	GIGE_API	LONG	ExitDevice(
												HANDLE				deviceHdl);						// handle for the device

// open serial communication port
EXTERN_C	GIGE_API	LONG	OpenSerialPort(
												HANDLE				deviceHdl,						// handle for the device
												LONG				serialPort,						// reserved; always set to 2
												ULONG				rxBufferSize,					// internal Pleora API receive buffer size; default 2048
												UCHAR				useTermChar,					// reserved; always set to 1
												UCHAR				termChar);						// reserved; always set to 0x0d

// open serial communication port
EXTERN_C	GIGE_API	LONG	CloseSerialPort(
												HANDLE				deviceHdl);						// handle for the device

// writes and read from the serial communication port
EXTERN_C	GIGE_API	LONG	SerialPortWriteRead(
												HANDLE				deviceHdl,						// handle for the device
												CONST PUCHAR		txBuffer,						// transmit buffer
												ULONG				txBufferSize,					// transmit buffer size/bytes to send
												PULONG				bytesWritten,					// returns the number of bytes written
												PUCHAR				rxBuffer,						// receive buffer
												ULONG				rxBufferSize,					// receive buffer size
												PULONG				bytesRead,						// returns the number of bytes read
												ULONG				timeOut);						// time out

// open an image acquisition stream
EXTERN_C	GIGE_API	LONG	OpenStream(
												HANDLE				deviceHdl);						// handle for the device

// close an image acquisition stream
EXTERN_C	GIGE_API	LONG	CloseStream(
												HANDLE				deviceHdl);						// handle for the device

// set the application image format to the Pleora GigE device (fixed for this application except the resolution)
EXTERN_C	GIGE_API	LONG	SetImageFormatControl(
												HANDLE				deviceHdl,						// handle for the device
												CONST LPSTR			pixelFormat,					// reserved; always set to "Mono14"
												ULONGLONG			width,							// x-resolution 4..16376, increment 4
												ULONGLONG			height,							// y-resolution 1..16383, increment 1
												ULONGLONG			offsetX,						// reserved; always set to 0
												ULONGLONG			offsetY,						// reserved; always set to 0
												CONST LPSTR			sensorTaps,						// reserved; always set to "One"
												CONST LPSTR			testPattern);					// reserved; always set to "Off"

// create Pleora API image acquisition pipeline
EXTERN_C	GIGE_API	LONG	CreatePipeline(
												HANDLE				deviceHdl,						// handle for the device
												ULONG				bufferCount,					// internal Pleora API buffer count (single frame; full FPS); default 512 (increase to avoid frame skip; decrease to save memory)
												ULONG				transferBufferCount,			// buffer count for data exchange between aSpect DLL and third party application; default 100 (increase to avoid frame skip; decrease to save memory)
												ULONG				transferBufferFrameCount);		// number of frame within one transfer buffer; default 100 (increase to avoid frame skip; decrease to save memory)

// close Pleora API image acquisition pipeline
EXTERN_C	GIGE_API	LONG	ClosePipeline(
												HANDLE				deviceHdl);						// handle for the device

// acquire images to the transfer buffers. Once one transfer buffer is filled the registered callback function (RegisterTransferBufferReadyCallBack) is called to transmit the pointer to a third party application. Once the third party application is ready working on this buffer the has to be returned to the pool by invoking ReturnBuffer. Once the specified frames are acquired or a StopAcquisition call or an error occurs the function will return.
EXTERN_C	GIGE_API	LONG	AcquireImages(														// todo add TimeOut, Synchronous/Asynchronous,
												HANDLE				deviceHdl,						// handle for the device
												ULONG				ImageCount,						// number of frames to acquire; set to 0 for continuous acquisition
												PULONGLONG			ImagesAcquired);				// number of frames acquired

// get the last occurred Pleora API error code 												
EXTERN_C	GIGE_API	ULONG	GetLastResult(
												HANDLE				deviceHdl);						// handle for the device

// register a callback function to signalize a third party application a filled transfer buffer. The function should transmit the pointer to the application and should return in a short term to avoid skipping frames. Once the application is ready working on the buffer the pointer has to be returned to the pool by invoking ReturnBuffer.
EXTERN_C	GIGE_API	void	RegisterTransferBufferReadyCallBack(
												HANDLE				deviceHdl,						// handle for the device
												p_bufferCallBack	transferBufferReadyCallBack );	// pointer to the callback function

// returns a buffer/point back to the transfer buffer pool. The order returning the pointers doesn't matter. Currently, if all buffer are handed over to the third party application and no buffer returns the application will hang until a buffer is returned by using this function.
EXTERN_C	GIGE_API	void	ReturnBuffer(
												HANDLE				deviceHdl,						// handle for the device
												PUCHAR				transferBuffer );				// pointer to the transfer buffer to push back into the pool

// stop an running acquisition
EXTERN_C	GIGE_API	void	StopAcquisition(
												HANDLE				deviceHdl);						// handle for the device