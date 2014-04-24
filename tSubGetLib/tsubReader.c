#include "tsubInternal.h"

static int initGraph(FileReader *fr);
static int addFilters(FileReader *fr, wchar_t *fileIn);
static int findPin(IEnumPins *pEnum, IPin **pin, int direction);
static int connectPins(FileReader *fr);

int tsgProcess(CaptionsParser *p){
	HRESULT hr;
	long state;

	p->fr.pControl->lpVtbl->Run(p->fr.pControl);
	do{
		if (IsEventActive(p->hAbort)){
			p->fr.pControl->lpVtbl->Stop(p->fr.pControl);
			return PARSER_E_ABORT;
		}

		hr = p->fr.pEvent->lpVtbl->WaitForCompletion(p->fr.pEvent,100,&state);
		if (SUCCEEDED(hr)){
			if (state == EC_COMPLETE){ 
				ccEnd(p->cc, p->fr.duration, FALSE);
				p->fr.currentPos = p->fr.duration;

				//IMPORTANT: Not stopping graph will lead to mem leaks
				p->fr.pControl->lpVtbl->Stop(p->fr.pControl);
				return PARSER_OK;
			}
			else return PARSER_E_IN;
		}

		p->fr.pControl->lpVtbl->GetState(p->fr.pControl,100,&state);
	} while (state == State_Running);

	return p->fr.state;
}

int tsgGetProgress(CaptionsParser *p){
	if (p && p->fr.duration > 0)
		return (int)(p->fr.currentPos*100/p->fr.duration);
	return 0;
}

int tsgGetPositionStr(CaptionsParser *p, wchar_t *buf, size_t bufSize){
	RTime total = {0}, current = {0};

	if (!buf) return FALSE;
	if (p){
		convertMsToRTime(p->fr.duration / 10000, &total);
		convertMsToRTime(p->fr.currentPos / 10000, &current);
	}
	_snwprintf_s(buf, bufSize, _TRUNCATE, L"%.2lld:%.2lld:%.2lld / %.2lld:%.2lld:%.2lld",
					current.h, current.m, current.s, total.h, total.m, total.s);
	return TRUE;			
}

int readerInit(CaptionsParser *p){
	int ret;
	if (!p)
		return PARSER_E_PARAMS;

	ngCallbackInit(p);
	
	ret = initGraph(&p->fr);
	if (ret != PARSER_OK){
		readerClose(p);
		return ret;
	}

	ret = addFilters(&p->fr,p->po.fileIn);
	if (ret != PARSER_OK){
		readerClose(p);
		return ret;
	}

	ret = connectPins(&p->fr);
	p->fr.state = PARSER_OK;

	if (ret != PARSER_OK)
		readerClose(p);
	
	return ret;
}

void readerClose(CaptionsParser *p){
	int ret = 0;
	if (p->fr.pNullGrabber)
		ret = p->fr.pNullGrabber->lpVtbl->Release(p->fr.pNullGrabber);
	if (p->fr.pNullGrabberF)
		ret = p->fr.pNullGrabberF->lpVtbl->Release(p->fr.pNullGrabberF);
	if (p->fr.pSourceF)
		ret = p->fr.pSourceF->lpVtbl->Release(p->fr.pSourceF);
	if (p->fr.pEvent)
		ret = p->fr.pEvent->lpVtbl->Release(p->fr.pEvent);
	if (p->fr.pControl)
		ret = p->fr.pControl->lpVtbl->Release(p->fr.pControl);
	if (p->fr.pStatus)
		ret = p->fr.pStatus->lpVtbl->Release(p->fr.pStatus);
	if (p->fr.pGraph)
		ret = p->fr.pGraph->lpVtbl->Release(p->fr.pGraph);
	ZeroMemory(&p->fr, sizeof(FileReader));	
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

	hr = CoCreateInstance(&CLSID_StreamBufferSource, NULL, CLSCTX_INPROC_SERVER,
							&IID_IBaseFilter, &fr->pSourceF);
	if (FAILED(hr)) {
		fr->pSourceF = NULL;
	} else {
		IFileSourceFilter *f;
		hr = fr->pSourceF->lpVtbl->QueryInterface(fr->pSourceF, &IID_IFileSourceFilter, &f);
		if (FAILED(hr)) {
			fr->pSourceF->lpVtbl->Release(fr->pSourceF);
			fr->pSourceF = NULL;
		}

		hr = f->lpVtbl->Load(f, fileIn, NULL);
		f->lpVtbl->Release(f);
		if (FAILED(hr)) {
			fr->pSourceF->lpVtbl->Release(fr->pSourceF);
			fr->pSourceF = NULL;
		} else {
			hr = fr->pGraph->lpVtbl->AddFilter(fr->pGraph, fr->pSourceF, L"Source");
			if (FAILED(hr)) {
				fr->pSourceF->lpVtbl->Release(fr->pSourceF);
				fr->pSourceF = NULL;
			}
		}
	}

	if (fr->pSourceF == NULL) {
		hr = fr->pGraph->lpVtbl->AddSourceFilter(fr->pGraph,fileIn,
													L"Source",&fr->pSourceF);
		if (FAILED(hr)) {
			return PARSER_E_IN;
		}
	}
	hr = fr->pGraph->lpVtbl->AddFilter(fr->pGraph,fr->pNullGrabberF,L"Null Grabber");
	if (FAILED(hr))
		return PARSER_E_COM;
	hr = fr->pNullGrabberF->lpVtbl->QueryInterface(fr->pNullGrabberF,
											&IID_INullGrabber,&fr->pNullGrabber);
	if (FAILED(hr))
		return PARSER_E_COM;
	fr->pNullGrabber->lpVtbl->SetAcceptedMediaType(fr->pNullGrabber,&am);
	fr->pNullGrabber->lpVtbl->SetCallback(fr->pNullGrabber,
											(INullGrabberCB*) &fr->NullGrabberCB);
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
		hr = fr->pGraph->lpVtbl->ConnectDirect(fr->pGraph, pinOut, pinIn, NULL);
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