#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <Dshow.h>
#include <initguid.h>
#include "..\\NullGrabberIIDs\\NullGrabber_h.h"
#pragma comment (lib,"strmiids.lib")
#pragma comment (lib,"rpcrt4.lib") 

typedef struct _DShowGraph {
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEventEx *pEvent;
	IBaseFilter *pSourceF;
	IBaseFilter *pNullGrabberF;
	INullGrabber *pNullGrabber;
	NGCALLBACK callbackRoutine;
	AM_MEDIA_TYPE am;
} DShowGraph;

typedef struct _StreamInfo {
	int outStart;
	int outEnd;
	FILE *fp;
	DShowGraph dsg;
	wchar_t inFile[MAX_PATH];
} StreamInfo;

int parseFile(StreamInfo *si);