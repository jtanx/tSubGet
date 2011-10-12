#include "tsubParser.h"
#include <Dshow.h>
#include "..\\NullGrabberIIDs\\NullGrabber_h.h"

struct _FileReader{
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent *pEvent;
	IMediaSeeking *pStatus;
	IBaseFilter *pSourceF;
	IBaseFilter *pNullGrabberF;
	INullGrabber *pNullGrabber;
	NGCALLBACK callbackRoutine;
	__int64 duration;
	__int64 currentPos;
	int state;
};

int parseSample(CaptionsParser *p, Sample smp);