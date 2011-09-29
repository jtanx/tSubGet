/* Header file for tsubInput.c & tsubOutput.c */

#include <Dshow.h>
#include "..\\NullGrabberIIDs\\NullGrabber_h.h"

#define SUB_EXT		L".srt"
#define SUB_EXT_SZ	5

typedef struct _rTime{
	long h;
	long m;
	long s;
	long ms;
} rTime;

typedef struct _DShowGraph {
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEventEx *pEvent;
	IBaseFilter *pSourceF;
	IBaseFilter *pNullGrabberF;
	INullGrabber *pNullGrabber;
	NGCALLBACK callbackRoutine;
} DShowGraph;

static const wchar_t *colours[] = {L"black",L"red",L"green",L"yellow",
									L"blue",L"magenta",L"cyan",L"white"};
