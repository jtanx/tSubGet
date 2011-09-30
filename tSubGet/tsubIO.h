/* Header file for tsubInput.c & tsubOutput.c */

#include <Dshow.h>
#include "..\\NullGrabberIIDs\\NullGrabber_h.h"

typedef struct _DShowGraph {
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEventEx *pEvent;
	IMediaSeeking *pStatus;
	IBaseFilter *pSourceF;
	IBaseFilter *pNullGrabberF;
	INullGrabber *pNullGrabber;
	NGCALLBACK callbackRoutine;
	LONGLONG duration;
	LONGLONG currentPos;
} DShowGraph;

//Decoder stuff
#define CAP_LENGTH		51

enum _colours {
	BLACK,RED,GREEN,YELLOW,BLUE,
	MAGENTA,CYAN,WHITE
};

static const wchar_t *colours[] = {L"black",L"red",L"green",L"yellow",
									L"blue",L"magenta",L"cyan",L"white"};

typedef struct _Sample {
	long double sTime;
	BYTE *buffer;
	DWORD cBPos;
	DWORD bufLen;	
} Sample;

typedef struct _Caption {
	unsigned lang[2];
	unsigned tsIndex;
	unsigned colour[2];
	unsigned pos[2];
	unsigned noBreak;
	wchar_t text[CAP_LENGTH];
} Caption;

typedef struct _Tstamp {
	long double startTime;
	long double endTime;
} Tstamp;

typedef struct _DecoderMeta {
	unsigned state;
	unsigned curLang[2];
	unsigned forceLang[2];
	unsigned forceLangFlag;
	unsigned capIdx;
	unsigned capTextIdx;
	unsigned tsIdx;
	unsigned capSize;
	unsigned tsSize;
	unsigned hasActiveTs;
} DecoderMeta;

typedef struct _Decoder {
	Sample smp;
	DecoderMeta meta;
	Caption *caps;
	Tstamp *ts;
	unsigned pageNumber;
} Decoder;

int initialiseDecoder(Decoder *d, unsigned pageNumber, unsigned forceLang[2]);
void finaliseDecoder(Decoder *d);
void freeDecoder(Decoder *d);
int decodeSample(Decoder *d);