/* Header file for tsubReader.c */

#include <wmsdk.h>
#pragma comment (lib,"Wmvcore.lib")

#define SUB_EXT		L".srt"
#define SUB_EXT_SZ	5

//Major type: MEDIATYPE_MSTVCAPTION
//Subtype: MEDIASUBTYPE_ETDTFilter_Tagged
static const GUID majorReq = {0xB88B8A89, 0xB049, 0x4C80,
						{0xAD, 0xCF, 0x58, 0x98, 0x98, 0x5E, 0x22, 0xC1}};
static const GUID subReq = {0xC4C4C4D0, 0x0049, 0x4E2B, 
						{0x98, 0xFB, 0x95, 0x37, 0xF6, 0xCE, 0x51, 0x6D}};

typedef struct _rTime{
	long h;
	long m;
	long s;
	long ms;
} rTime;

static const wchar_t *colours[] = {L"black",L"red",L"green",L"yellow",
									L"blue",L"magenta",L"cyan",L"white"};

