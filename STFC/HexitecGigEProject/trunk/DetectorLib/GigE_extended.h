#ifndef GIGE_EXTENDED_H
#define GIGE_EXTENDED_H
#define EXTERN_C extern "C"

#define PVOID void *
#define VOID void
#define CONST const

#define LONG long
#define PLONG long *
#define ULONG unsigned long
#define PULONG unsigned long *
#define PLONGLONG long long *
#define ULONGLONG unsigned long long
#define PULONGLONG unsigned long long *
#define CHAR char
#define UCHAR unsigned char
#define PUCHAR unsigned char *
#define LPSTR char *
#define HANDLE void *

namespace GigE 
{
   #include "GigE.h"
   HANDLE detectorHandle;
}

#endif // GIGE_EXTENDED_H
