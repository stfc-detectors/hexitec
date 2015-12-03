/*
 * hxtRawDataTypes.h
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#ifndef HXTRAWDATATYPES_H_
#define HXTRAWDATATYPES_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
	u8 byte[3];
} hxtRawLine;

typedef struct {
	u8 frameByte[6];
} hxtFrameCounter;

typedef struct {
	u8 pad;
	u8 rowLabel;
	u8 rowIdx;
} hxtRowMarker;

typedef struct {
	u16 pixel;
	u8  colIdx;
} hxtColumnData;

typedef struct {
	u32 nRow;
	u32 nCol;
} hxtPixel;

typedef enum {
	framePreamble,
	frameHeaderStart,
	frameHeaderEnd,
	rowMarker,
	colData
} hxtParserState;

// Motor positions struct - Passing as one unit within GUI/HxtProcessing
typedef struct {
    // New motor names:
    int mSSX;
    int mSSY;
    int mSSZ;
    int mSSROT;
    int mTimer;
    int mGALX;
    int mGALY;
    int mGALZ;
    int mGALROT;
} motorPositions;

// Used to track whether one/several members of the above struct has changed
typedef enum {
    conditionNoneMet=1,
    conditionMotorAnyStep=2,
    conditionMotorPositionStep=4,
    conditionMotorTimeStep=8,
    conditionEveryNewFile=16
} processingCondition;

// Used to interrogate motorPositions struct special cases
//  i.e. is a given motor value unavailable/uninitialised or real?
typedef enum {
    positionUnavailable   = 1111111,           // Used to be -77777
    positionUninitialised = 2222222            // Used to be -65536
} motorSpecialValues;

const u8 kHxtFramePreambleLabel = 255;
const u8 kHxtRowMarkerLabel     = 192;

#endif /* HXTRAWDATATYPES_H_ */
