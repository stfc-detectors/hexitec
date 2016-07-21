#ifndef HEXITEC_H
#define HEXITEC_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HEXITEC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HEXITEC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HEXITEC_EXPORTS
#define HEXITEC_API __declspec(dllexport)
#else
#define HEXITEC_API __declspec(dllimport)
#endif

#define EXTERN_C extern "C"

#define AHANDLE void *
#define PVOID void *
#define VOID void
#define CONST const

#define LONG long
#define PLONG long *
#define ULONG unsigned long
#define PULONG unsigned long *
#define CHAR char
#define UCHAR unsigned char
#define PUCHAR unsigned char *
#define LPSTR char *

typedef enum _ImageType {
	AS_IMAGE_U8              = 0,           //The image type is 8-bit unsigned integer grayscale.
	AS_IMAGE_U16             = 7,           //The image type is 16-bit unsigned integer grayscale.
	AS_IMAGE_I16             = 1,           //The image type is 16-bit signed integer grayscale.
	AS_IMAGE_SGL             = 2,           //The image type is 32-bit floating-point grayscale.
	AS_IMAGE_COMPLEX         = 3,           //The image type is complex.
	AS_IMAGE_RGB             = 4,           //The image type is RGB color.
	AS_IMAGE_HSL             = 5,           //The image type is HSL color.
	AS_IMAGE_RGB_U64         = 6,           //The image type is 64-bit unsigned RGB color.
	AS_IMAGE_TYPE_SIZE_GUARD = 0xFFFFFFFF   
	} ImageType;

typedef enum _PixelSize {
	AS_PIXELSIZE_U8              = 1,           //The image type is 8-bit unsigned integer grayscale.
	AS_PIXELSIZE_U16             = 2,           //The image type is 16-bit unsigned integer grayscale.
	AS_PIXELSIZE_I16             = 2,           //The image type is 16-bit signed integer grayscale.
	AS_PIXELSIZE_SGL             = 4,           //The image type is 32-bit floating-point grayscale.
	AS_PIXELSIZE_COMPLEX         = 8,           //The image type is complex.
	AS_PIXELSIZE_RGB             = 4,           //The image type is RGB color.
	AS_PIXELSIZE_HSL             = 4,           //The image type is HSL color.
	AS_PIXELSIZE_RGB_U64         = 8,           //The image type is 64-bit unsigned RGB color.
	AS_PIXELSIZE_TYPE_SIZE_GUARD = 0xFFFFFFFF   
	} PixelSize;

typedef LONG	(__cdecl *p_newDataFileFunction)(CONST LPSTR path);

#pragma pack(push)
#pragma pack(1)

typedef struct _ImageInfo {
	ImageType	imageType;
	LONG		xRes;
	LONG		yRes;
	LONG		border;
	LONG		pixelsPerLine;
	PixelSize	pixelSize;
	} ImageInfo, *ImageInfoPtr, **ImageInfoHdl;

#pragma pack(pop)

EXTERN_C HEXITEC_API LONG	InitFrameGrabber(
					AHANDLE		*fg,
					CONST LPSTR	ifName,
					CONST LPSTR	ifCamFile,
					PLONG		asError,
					PLONG		niError,
					PLONG		sysError);

EXTERN_C HEXITEC_API LONG	ExitFrameGrabber(
					AHANDLE		fg);

EXTERN_C HEXITEC_API LONG	ConfigureFrameGrabber(
					AHANDLE		fg,
					ULONG		triggerPolarity,
					PVOID		imgDest,
					ImageInfoPtr	imgDestInfo,
					LPSTR		imgCorrectPath,
					LPSTR		imgDarkPath,
					UCHAR		doCorrectImg, 
					UCHAR		doDarkImg,
					ULONG		xResGrab,
					ULONG		yResGrab,
					ULONG		xStartROI,
					ULONG		yStartROI,
					ULONG		xSizeROI,
					ULONG		ySizeROI,
					ULONG		frameUpdate,
					UCHAR		sensorType,
					ULONG		imagesPerFile,
					ULONG		timeOut,
					ULONG		maxRingBufCnt,
					PULONG		imgCntMaxFrameBuffer);

EXTERN_C HEXITEC_API LONG	GetImages(
					AHANDLE		fg,
					PVOID	 	imgDest,
					ImageInfoPtr	imgDestInfo,
					ULONG		useTrigger,
					UCHAR		streamEn,
					ULONG		imgCntAverage,
					ULONG		imgCntPreTrg,
					LONG		imgCntPostTrg,
					LPSTR		path,
					UCHAR		remote,
					ULONG		timeOut);

EXTERN_C HEXITEC_API LONG	SerialWrite(
					AHANDLE		fg,
					UCHAR		idx, 
					CONST PUCHAR	buffer, 
					PULONG		bufferLen, 
					ULONG		timeout);

EXTERN_C HEXITEC_API LONG	SerialRead(
					AHANDLE		fg,
					UCHAR		idx, 
					PUCHAR		buffer, 
					PULONG		bufferLen, 
					ULONG		timeout);

EXTERN_C HEXITEC_API LONG	SetParameter(
					AHANDLE		fg,
					UCHAR		doCorrectImg, 
					UCHAR		doDarkImg);

EXTERN_C HEXITEC_API LONG	GetLastErrorMsg(	
					AHANDLE		fg,	
					PLONG		asError, 
					LPSTR		asErrorMsg, 
					PLONG		niError, 
					LPSTR		niErrorMsg, 
					PLONG		sysError, 
					LPSTR		sysErrorMsg, 
					ULONG		length);

EXTERN_C HEXITEC_API LONG	GetResult(	
					AHANDLE		fg,	
					PLONG		asError, 
					LPSTR		asErrorMsg, 
					PLONG		niError, 
					LPSTR		niErrorMsg, 
					PLONG		sysError, 
					LPSTR		sysErrorMsg, 
					ULONG		length,
					ULONG		timeOut);

EXTERN_C HEXITEC_API VOID	GetLastErrorStrA(
					PLONG		asError,
					LPSTR		asErrorMsg,
					PLONG		niError,
					LPSTR		niErrorMsg,
					PLONG		sysError,
					LPSTR		sysErrorMsg,
					ULONG		length);

EXTERN_C HEXITEC_API LONG	RegisterNewDataFileCallBack(	
					AHANDLE			fg,
					p_newDataFileFunction	functionPtr);

EXTERN_C HEXITEC_API LONG	UnRegisterNewDataFileCallBack(
					AHANDLE			fg);

EXTERN_C HEXITEC_API LONG	NewDataFileCallBackTest(	
					HANDLE			fg);
	
HEXITEC_API LONG		NewDataFileCallBack(
					CONST LPSTR		path);
#endif // HEXITEC_H
