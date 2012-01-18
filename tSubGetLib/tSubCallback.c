#include <initguid.h> //DO NOT REMOVE - to initialise the custom GUIDs used by NullGrabber.
#include "..\\NullGrabber\\NullGrabberInterfaces.h"
#include "tSubInternal.h"
#define IsEventActive(hEvent) (WaitForSingleObject((hEvent),0) == WAIT_OBJECT_0)

/* QueryInterface, AddRef and Release are all function stubs used to
   implement INullGrabberCB (pseudo-COM interface)
*/
HRESULT STDMETHODCALLTYPE QueryInterface(INullGrabberCB *this, REFIID riid, void **ppvObject){
	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE AddRef(INullGrabberCB *this){
	return 2;
}

HRESULT STDMETHODCALLTYPE Release(INullGrabberCB *this){
	return 1;
}

/*
   SampleCB(this, pSample, StartTime, EndTime) is the main callback
   function for samples received from the Null Grabber. Generally
   speaking, EndTime is of no use. It is at this point in which
   the abort condition is checked and executed.
*/
HRESULT STDMETHODCALLTYPE SampleCB(INullGrabberCB *this, IMediaSample *pSample, REFERENCE_TIME *StartTime, REFERENCE_TIME *EndTime){
	CaptionsParser *p = ((NGCallback*) this)->pParent;

	if (IsEventActive(p->hAbort)){
		p->fr.state = PARSER_E_ABORT;
		p->fr.pControl->lpVtbl->Pause(p->fr.pControl);
		return S_FALSE;
	} else{
		Sample smp;
		p->fr.currentPos = *StartTime;

		smp.bufSize = pSample->lpVtbl->GetActualDataLength(pSample);
		pSample->lpVtbl->GetPointer(pSample,&smp.pBuf);
		smp.time = *StartTime;
		p->fr.state = parseSample(p,smp);
		if (p->fr.state != PARSER_OK){
			p->fr.pControl->lpVtbl->Pause(p->fr.pControl);
			return S_FALSE;
		}
	}
	return S_OK;
}

static const INullGrabberCBVtbl INullGrabberCB_Vtbl = {
	QueryInterface,
	AddRef,
	Release,
	SampleCB
};

/*
   ngCallbackinit(p) 'initialises' INullGrabberCB/NGCallback
   with the required settings, and enables the callback function
   SampleCB to work.
*/
void ngCallbackInit(CaptionsParser *p){
	p->fr.NullGrabberCB.lpVtbl = &INullGrabberCB_Vtbl;
	p->fr.NullGrabberCB.pParent = p;
}