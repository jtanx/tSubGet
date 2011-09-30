#include "tsubGet.h"

/////////////////////////////Main Data Callback/////////////////////////////
HRESULT dataCallback(IMediaSample *pMediaSample, __int64 *start, __int64 *end){
	static Details *uDets = NULL;
	
	if (!pMediaSample && !end){
		uDets = (Details*)start;
		return S_OK;
	} else if (WaitForSingleObject(uDets->gui.hEvent,0) == WAIT_OBJECT_0){
		uDets->po.g.pControl->lpVtbl->Pause(uDets->po.g.pControl);
		return S_FALSE;
	} else{
		HRESULT hr;
		LONGLONG cPos = *start;

		if (uDets->po.g.currentPos > cPos){
			uDets->po.g.currentPos = cPos;
		}
		else if (cPos - uDets->po.g.currentPos > 100){
			uDets->po.g.currentPos = *start;
			cPos = (LONGLONG)((double)cPos/uDets->po.g.duration*100);
			SendMessage(uDets->gui.hwndProgress,PBM_SETPOS,(WPARAM)cPos,0);
		}
		hr = pMediaSample->lpVtbl->GetPointer(pMediaSample,&(uDets->po.d.smp.buffer));
		if (FAILED(hr)) return hr;

		uDets->po.d.smp.bufLen = pMediaSample->lpVtbl->GetActualDataLength(pMediaSample);
		uDets->po.d.smp.sTime = (double)(*start)/10000.0;
		if (!decodeSample(&uDets->po.d)){
			uDets->po.g.pControl->lpVtbl->Pause(uDets->po.g.pControl);
			return S_FALSE;
		}
	}
	return S_OK;
}

/////////////////////////////Direct Show Stuff/////////////////////////////
static int initGraph(DShowGraph *dsg){
	HRESULT hr;
	IMediaFilter *pMedia;

	hr = CoCreateInstance(&CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		&IID_IGraphBuilder,&(dsg->pGraph));
	if (FAILED(hr))
		return FALSE;

	dsg->pGraph->lpVtbl->QueryInterface(dsg->pGraph,&IID_IMediaControl,&(dsg->pControl));
	dsg->pGraph->lpVtbl->QueryInterface(dsg->pGraph,&IID_IMediaEventEx,&(dsg->pEvent));
	dsg->pGraph->lpVtbl->QueryInterface(dsg->pGraph,&IID_IMediaSeeking,&(dsg->pStatus));
	dsg->pGraph->lpVtbl->QueryInterface(dsg->pGraph,&IID_IMediaFilter,&pMedia);
	//This part is important: It makes DS run as fast as possible, instead of at playback speed.
	pMedia->lpVtbl->SetSyncSource(pMedia,NULL);
	pMedia->lpVtbl->Release(pMedia);

	return TRUE;
}

static int addFilters(DShowGraph *dsg, wchar_t *filename){
	HRESULT hr;
	AM_MEDIA_TYPE am = {0};
	am.majortype = MEDIATYPE_MSTVCaption;
	//am.subtype = MEDIASUBTYPE_ETDTFilter_Tagged;//MEDIASUBTYPE_TELETEXT;

	hr = CoCreateInstance(&CLSID_NullGrabber,NULL,
							CLSCTX_INPROC_SERVER,&IID_IBaseFilter,
							&(dsg->pNullGrabberF));
	if (FAILED(hr))
		return FALSE;
	
	hr = dsg->pGraph->lpVtbl->AddSourceFilter(dsg->pGraph, filename, 
										L"Source", &(dsg->pSourceF));
	if (FAILED(hr))
		return FALSE;

	hr = dsg->pGraph->lpVtbl->AddFilter(dsg->pGraph, dsg->pNullGrabberF,
										L"Null Grabber");
	if (FAILED(hr))
		return FALSE;
	dsg->pNullGrabberF->lpVtbl->QueryInterface(dsg->pNullGrabberF,
								&IID_INullGrabber,&(dsg->pNullGrabber));

	dsg->pNullGrabber->lpVtbl->SetAcceptedMediaType(dsg->pNullGrabber, &am);
	hr = dsg->pNullGrabber->lpVtbl->SetCallback(dsg->pNullGrabber, 
												dsg->callbackRoutine);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

static int findPin(IEnumPins *pEnum, IPin **pin, int direction){
	PIN_INFO pinfo;

	*pin = NULL;
	while(pEnum->lpVtbl->Next(pEnum,1,pin,NULL) == S_OK){
		(*pin)->lpVtbl->QueryPinInfo(*pin,&pinfo);
		pinfo.pFilter->lpVtbl->Release(pinfo.pFilter);
		
		if(pinfo.dir == direction)
			break;
		(*pin)->lpVtbl->Release(*pin);
		*pin = NULL;
	}
	if (*pin)
		return TRUE;
	return FALSE;
}

static int connectPins(DShowGraph *dsg){
	HRESULT hr;
	IEnumPins *pEnum;
	IPin *pinIn,*pinOut;

	//Get the input pin of the null grabber filter
	dsg->pNullGrabberF->lpVtbl->EnumPins(dsg->pNullGrabberF, &pEnum);
	pEnum->lpVtbl->Reset(pEnum);
	hr = findPin(pEnum,&pinIn,PINDIR_INPUT);
	if (!hr) return FALSE;
	pEnum->lpVtbl->Release(pEnum);

	//Find the right output pin of the source filter and connect.
	dsg->pSourceF->lpVtbl->EnumPins(dsg->pSourceF, &pEnum);
	pEnum->lpVtbl->Reset(pEnum);
	while (findPin(pEnum,&pinOut,PINDIR_OUTPUT)){
		hr = dsg->pGraph->lpVtbl->Connect(dsg->pGraph,pinOut,pinIn);
		if (hr == S_OK)
			break;
		pinOut->lpVtbl->Release(pinOut);
		pinOut = NULL;
	}
	pinIn->lpVtbl->Release(pinIn);
	if (!pinOut)
		return FALSE;

	dsg->pStatus->lpVtbl->SetTimeFormat(dsg->pStatus,&TIME_FORMAT_MEDIA_TIME);
	dsg->pStatus->lpVtbl->GetDuration(dsg->pStatus,&dsg->duration);
	return TRUE;
}

static int runGraph(DShowGraph *dsg){
	HRESULT hr;
	long state;

	dsg->pControl->lpVtbl->Run(dsg->pControl);
	do{
		hr = dsg->pEvent->lpVtbl->WaitForCompletion(dsg->pEvent,100,&state);
		if (!FAILED(hr)){
			if (state == EC_COMPLETE) return TRUE;
			return FALSE;
		}

		hr = dsg->pControl->lpVtbl->GetState(dsg->pControl,10,&state);
		if (FAILED(hr) || state == State_Paused)
			return FALSE;
	} while (state == State_Running);

	return FALSE;
}

static void cleanup(DShowGraph *dsg){
	int ret;
	if (dsg->pNullGrabber) 
		ret = dsg->pNullGrabber->lpVtbl->Release(dsg->pNullGrabber);
	if (dsg->pNullGrabberF) 
		ret = dsg->pNullGrabberF->lpVtbl->Release(dsg->pNullGrabberF);
	if (dsg->pSourceF) 
		ret = dsg->pSourceF->lpVtbl->Release(dsg->pSourceF);
	if (dsg->pEvent) 
		ret = dsg->pEvent->lpVtbl->Release(dsg->pEvent);
	if (dsg->pControl) 
		ret = dsg->pControl->lpVtbl->Release(dsg->pControl);
	if (dsg->pStatus) 
		ret = dsg->pStatus->lpVtbl->Release(dsg->pStatus);
	if (dsg->pGraph) 
		ret = dsg->pGraph->lpVtbl->Release(dsg->pGraph);
}

int parseFile(Details *uDets){
	memset(&uDets->po.g,0,sizeof(DShowGraph));
	dataCallback(NULL,(void*)uDets,NULL);
	uDets->po.g.callbackRoutine = dataCallback;

	if (!initGraph(&uDets->po.g) || !addFilters(&uDets->po.g,uDets->pi.filePath)){
		cleanup(&uDets->po.g);
		return FALSE;
	}
	if (!connectPins(&uDets->po.g) || !runGraph(&uDets->po.g)){
		cleanup(&uDets->po.g);
		return FALSE;
	}
	cleanup(&uDets->po.g);
	return TRUE;
}

