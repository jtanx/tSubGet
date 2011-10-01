#include "streamDump.h"

/////////////////////////////Main Data Callback/////////////////////////////
HRESULT dataCallback(IMediaSample *pMediaSample, __int64 *start, __int64 *end){
	static StreamInfo *si = NULL;
	
	if (!pMediaSample && !end){
		si = (StreamInfo*)start;
	} else{
		HRESULT hr;
		BYTE *pbDat;
		long dataLength;

		if (!si->fp) return S_FALSE;
		hr = pMediaSample->lpVtbl->GetPointer(pMediaSample,&pbDat);
		if (FAILED(hr)) return hr;
		dataLength = pMediaSample->lpVtbl->GetActualDataLength(pMediaSample);
		if (dataLength <= 0) return S_FALSE;

		if (si->outStart)
			fprintf(si->fp,"%lld",*start);
		if (si->outStart && si->outEnd)
			fprintf(si->fp," --> ");
		if (si->outEnd)
			fprintf(si->fp,"%lld",*end);
		if (si->outStart || si->outEnd)
			fprintf(si->fp,"\r\n");
		fwrite(pbDat,dataLength,1,si->fp);
		if (si->outStart || si->outEnd)
			fprintf(si->fp,"\r\n\xAA\xFF\xAA\xFF\r\n");
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

	dsg->pNullGrabber->lpVtbl->SetAcceptedMediaType(dsg->pNullGrabber, &dsg->am);
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

int parseFile(StreamInfo *si){
	
	//Initialise data callback
	dataCallback(NULL,(void*)si,NULL);
	si->dsg.callbackRoutine = dataCallback;

	if (!initGraph(&si->dsg) || !addFilters(&si->dsg,si->inFile)){
		cleanup(&si->dsg);
		return FALSE;
	}
	if (!connectPins(&si->dsg) || !runGraph(&si->dsg)){
		cleanup(&si->dsg);
		return FALSE;
	}
	cleanup(&si->dsg);
	return TRUE;
}

