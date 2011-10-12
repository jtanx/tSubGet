#include "tsubReader.h"
static HRESULT sampleCallback(IMediaSample *pMediaSample, __int64 *start, __int64 *end);
static int initGraph(FileReader *fr);
static int addFilters(FileReader *fr, wchar_t *fileIn);
static int findPin(IEnumPins *pEnum, IPin **pin, int direction);
static int connectPins(FileReader *fr);

int readerInit(CaptionsParser *p){
	int ret;

	p->fr = calloc(1,sizeof(FileReader));
	if (!p->fr)
		return PARSER_E_MEM;
	sampleCallback(NULL,(void*)p,NULL);
	p->fr->callbackRoutine = sampleCallback;
	
	ret = initGraph(p->fr);
	if (ret != PARSER_OK){
		readerClose(p);
		return ret;
	}

	ret = addFilters(p->fr,p->fileIn);
	if (ret != PARSER_OK){
		readerClose(p);
		return ret;
	}

	ret = connectPins(p->fr);
	p->fr->state = PARSER_OK;

	if (ret != PARSER_OK)
		readerClose(p);
	
	return ret;
}

int parserReadFile(CaptionsParser *p){
	HRESULT hr;
	long state;

	p->fr->pControl->lpVtbl->Run(p->fr->pControl);
	do{
		hr = p->fr->pEvent->lpVtbl->WaitForCompletion(p->fr->pEvent,100,&state);
		if (!FAILED(hr)){
			if (state == EC_COMPLETE){ 
				ccEnd(p->cc,&p->ccI,p->fr->duration);
				p->fr->currentPos = p->fr->duration;

				//IMPORTANT: Not stopping graph will lead to mem leaks
				p->fr->pControl->lpVtbl->Stop(p->fr->pControl);
				return PARSER_OK;
			}
			else return PARSER_E_IN;
		}
		hr = p->fr->pControl->lpVtbl->GetState(p->fr->pControl,100,&state);
		if (FAILED(hr) || state == State_Paused){
			p->fr->pControl->lpVtbl->Stop(p->fr->pControl);
			if (p->fr->state == PARSER_OK) p->fr->state = PARSER_E_IN;
			
			return p->fr->state;
		}
	} while (state == State_Running);

	return PARSER_E_IN;
}

int parserGetProgress(CaptionsParser *p){
	if (p->fr && p->fr->duration > 0)
		return (int)(p->fr->currentPos*100/p->fr->duration);
	return 0;
}

void readerClose(CaptionsParser *p){
	if (p->fr){
		if (p->fr->pNullGrabber)
			p->fr->pNullGrabber->lpVtbl->Release(p->fr->pNullGrabber);
		if (p->fr->pNullGrabberF)
			p->fr->pNullGrabberF->lpVtbl->Release(p->fr->pNullGrabberF);
		if (p->fr->pSourceF)
			p->fr->pSourceF->lpVtbl->Release(p->fr->pSourceF);
		if (p->fr->pEvent)
			p->fr->pEvent->lpVtbl->Release(p->fr->pEvent);
		if (p->fr->pControl)
			p->fr->pControl->lpVtbl->Release(p->fr->pControl);
		if (p->fr->pStatus)
			p->fr->pStatus->lpVtbl->Release(p->fr->pStatus);
		if (p->fr->pGraph)
			p->fr->pGraph->lpVtbl->Release(p->fr->pGraph);
		free(p->fr);
		p->fr = NULL;
	}
}

static HRESULT sampleCallback(IMediaSample *pMediaSample, __int64 *start, __int64 *end){
	static CaptionsParser *p = NULL;

	if (!pMediaSample && !end)
		p = (CaptionsParser*)start;
	else if (IsEventActive(p->hAbort)){
		p->fr->state = PARSER_E_ABORT;
		p->fr->pControl->lpVtbl->Pause(p->fr->pControl);
		return S_FALSE;
	} else{
		Sample smp;
		p->fr->currentPos = *start;

		smp.bufSize = pMediaSample->lpVtbl->GetActualDataLength(pMediaSample);
		pMediaSample->lpVtbl->GetPointer(pMediaSample,&smp.pBuf);
		smp.time = *start;
		p->fr->state = parseSample(p,smp);
		if (p->fr->state != PARSER_OK){
			p->fr->pControl->lpVtbl->Pause(p->fr->pControl);
			return S_FALSE;
		}
	}
	return S_OK;
}


static int initGraph(FileReader *fr){
	HRESULT hr;
	IMediaFilter *pMedia;

	if (!fr) return PARSER_E_PARAMS;

	hr = CoCreateInstance(&CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
							&IID_IGraphBuilder,&fr->pGraph);
	if (FAILED(hr))
		return PARSER_E_COM;
	fr->pGraph->lpVtbl->QueryInterface(fr->pGraph,&IID_IMediaControl,&(fr->pControl));
	fr->pGraph->lpVtbl->QueryInterface(fr->pGraph,&IID_IMediaEvent,&(fr->pEvent));
	fr->pGraph->lpVtbl->QueryInterface(fr->pGraph,&IID_IMediaSeeking,&(fr->pStatus));
	fr->pGraph->lpVtbl->QueryInterface(fr->pGraph,&IID_IMediaFilter,&pMedia);
	//This part is important: It makes DS run as fast as possible, instead of at playback speed.
	pMedia->lpVtbl->SetSyncSource(pMedia,NULL);
	pMedia->lpVtbl->Release(pMedia);

	return PARSER_OK;
}

static int addFilters(FileReader *fr, wchar_t *fileIn){
	HRESULT hr;
	AM_MEDIA_TYPE am = {0};
	am.majortype = MEDIATYPE_MSTVCaption;

	hr = CoCreateInstance(&CLSID_NullGrabber,NULL,CLSCTX_INPROC_SERVER,
							&IID_IBaseFilter,&fr->pNullGrabberF);
	if (FAILED(hr))
		return PARSER_E_COM;
	hr = fr->pGraph->lpVtbl->AddSourceFilter(fr->pGraph,fileIn,
												L"Source",&fr->pSourceF);
	if (FAILED(hr))
		return PARSER_E_IN;
	hr = fr->pGraph->lpVtbl->AddFilter(fr->pGraph,fr->pNullGrabberF,L"Null Grabber");
	if (FAILED(hr))
		return PARSER_E_COM;
	hr = fr->pNullGrabberF->lpVtbl->QueryInterface(fr->pNullGrabberF,
											&IID_INullGrabber,&fr->pNullGrabber);
	if (FAILED(hr))
		return PARSER_E_COM;
	fr->pNullGrabber->lpVtbl->SetAcceptedMediaType(fr->pNullGrabber,&am);
	fr->pNullGrabber->lpVtbl->SetCallback(fr->pNullGrabber,fr->callbackRoutine);
	return PARSER_OK;	
}

static int findPin(IEnumPins *pEnum, IPin **pin, int direction){
	PIN_INFO pinfo;

	*pin = NULL;
	while(pEnum->lpVtbl->Next(pEnum,1,pin,NULL) == S_OK){
		(*pin)->lpVtbl->QueryPinInfo(*pin,&pinfo);
		if (pinfo.pFilter)
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

static int connectPins(FileReader *fr){
	HRESULT hr;
	IEnumPins *pEnum;
	IPin *pinIn,*pinOut;

	//Get the input pin of the null grabber filter
	fr->pNullGrabberF->lpVtbl->EnumPins(fr->pNullGrabberF,&pEnum);
	pEnum->lpVtbl->Reset(pEnum);
	hr = findPin(pEnum,&pinIn,PINDIR_INPUT);
	if (!hr) return PARSER_E_COM;
	pEnum->lpVtbl->Release(pEnum);

	//Find the right output pin of the source filter and connect.
	fr->pSourceF->lpVtbl->EnumPins(fr->pSourceF,&pEnum);
	pEnum->lpVtbl->Reset(pEnum);
	while (findPin(pEnum,&pinOut,PINDIR_OUTPUT)){
		hr = fr->pGraph->lpVtbl->Connect(fr->pGraph,pinOut,pinIn);
		if (hr == S_OK)
			break;
		pinOut->lpVtbl->Release(pinOut);
		pinOut = NULL;
	}
	pEnum->lpVtbl->Release(pEnum);
	pinIn->lpVtbl->Release(pinIn);

	if (!pinOut)
		return PARSER_E_IN;
	pinOut->lpVtbl->Release(pinOut);

	fr->pStatus->lpVtbl->SetTimeFormat(fr->pStatus,&TIME_FORMAT_MEDIA_TIME);
	fr->pStatus->lpVtbl->GetDuration(fr->pStatus,&fr->duration);
	
	return PARSER_OK;
}