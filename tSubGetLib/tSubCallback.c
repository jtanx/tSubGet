#include <initguid.h> //DO NOT REMOVE - to initialise the custom GUIDs used by NullGrabber.
#include "..\\NullGrabber\\NullGrabberInterfaces.h"
#include "tSubInternal.h"
#define IsEventActive(hEvent) (WaitForSingleObject((hEvent),0) == WAIT_OBJECT_0)

HRESULT STDMETHODCALLTYPE QueryInterface(INullGrabberCB *this, REFIID riid, void **ppvObject){
	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE AddRef(INullGrabberCB *this){
	return 2;
}

HRESULT STDMETHODCALLTYPE Release(INullGrabberCB *this){
	return 1;
}

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

void ngCallbackInit(CaptionsParser *p){
	p->fr.NullGrabberCB.lpVtbl = &INullGrabberCB_Vtbl;
	p->fr.NullGrabberCB.pParent = p;
}