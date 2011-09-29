#include "tsubGet.h"
#include "tsubIO.h"

/////////////////////////////Main Data Callback/////////////////////////////
HRESULT dataCallback(IMediaSample *pMediaSample, __int64 *start, __int64 *end){
	static Decoder *d = NULL;
	
	if (!pMediaSample && !end){
		d = (Decoder*)start;
		return S_OK;
	} else{
		HRESULT hr;

		hr = pMediaSample->lpVtbl->GetPointer(pMediaSample,&(d->smp.buffer));
		if (FAILED(hr)) return hr;

		d->smp.bufLen = pMediaSample->lpVtbl->GetActualDataLength(pMediaSample);
		d->smp.sTime = (double)(*start)/10000.0;
		if (!decodeSample(d))
			return S_FALSE;
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
	return TRUE;
}

static int runGraph(DShowGraph *dsg){
	HRESULT hr;
	long eventCode;

	//You can set up event handling here AFAIK...
	hr = dsg->pControl->lpVtbl->Run(dsg->pControl);
	hr = dsg->pEvent->lpVtbl->WaitForCompletion(dsg->pEvent,INFINITE,&eventCode);
	if (FAILED(hr))
		return FALSE;
	return TRUE;
}

static void cleanup(DShowGraph *dsg){
	if (dsg->pNullGrabber) dsg->pNullGrabber->lpVtbl->Release(dsg->pNullGrabber);
	if (dsg->pNullGrabberF) dsg->pNullGrabberF->lpVtbl->Release(dsg->pNullGrabberF);
	if (dsg->pSourceF) dsg->pSourceF->lpVtbl->Release(dsg->pSourceF);
	if (dsg->pEvent) dsg->pEvent->lpVtbl->Release(dsg->pEvent);
	if (dsg->pControl) dsg->pControl->lpVtbl->Release(dsg->pControl);
	if (dsg->pGraph) dsg->pGraph->lpVtbl->Release(dsg->pGraph);
}

int parseFile(Decoder *d, wchar_t *filename){
	DShowGraph dsg = {0};
	
	//Initialise data callback
	dataCallback(NULL,(void*)d,NULL);
	dsg.callbackRoutine = dataCallback;

	if (!initGraph(&dsg) || !addFilters(&dsg,filename)){
		cleanup(&dsg);
		return FALSE;
	}
	if (!connectPins(&dsg) || !runGraph(&dsg)){
		cleanup(&dsg);
		return FALSE;
	}
	//Hack to finalise the last timestamp 
	if (d->meta.hasActiveTs){
		d->ts[d->meta.tsIdx].endTime = d->smp.sTime;
		d->meta.hasActiveTs = FALSE;
	}
	cleanup(&dsg);
	return TRUE;
}

