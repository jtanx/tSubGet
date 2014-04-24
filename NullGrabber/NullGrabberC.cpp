#include <streams.h>
#include <Windows.h>
#include <stdarg.h>
#include <wchar.h>
#include <new>
#include "NullGrabber.h"

#define IsEventActive(hEvent) (WaitForSingleObject((hEvent), 0) == WAIT_OBJECT_0)

NGC_Interface::NGC_Interface(wchar_t *filepath, const CMediaType *pmtAccept) : 
	pControl(NULL), pEvent(NULL), pGraph(NULL), 
	pNullGrabber(NULL), pSourceF(NULL), pStatus(NULL),
	callback(NULL), progress(NULL), progressUserData(NULL),
	callbackUserData(NULL), hrCallback(S_OK)
{
	HRESULT hr;

	if (FAILED((hr = initGraphFilters(filepath, pmtAccept)))) {
		closeGraph();
		throw hr;
	} else if (FAILED((hr = connectFilters()))) {
		closeGraph();
		throw hr;
	}

	//Create the abort event.
	abortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	runEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

NGC_Interface::~NGC_Interface() {
	closeGraph();

	CloseHandle(abortEvent);
	CloseHandle(runEvent);
}

void NGC_Interface::SetSampleCallback(NGC_Callback callback, void *userData) {
	CAutoLock lock(&m_Lock);
	this->callback = callback;
	this->callbackUserData = userData;
}

void NGC_Interface::SetProgressCallback(NGC_Progress progress, void *userData) {
	CAutoLock lock(&m_Lock);
	this->progress = progress;
	this->progressUserData = userData;
}

HRESULT NGC_Interface::Run() {
	HRESULT hr;
	long state;

	//Prevent multiple calls to run!
	if (IsEventActive(this->runEvent)) {
		return E_ABORT; //Not technically right but...
	} 

	SetEvent(this->runEvent);
	this->pControl->Run();
	do{
		if (IsEventActive(this->abortEvent)){
			this->pControl->Stop();
			if (FAILED(this->hrCallback))
				return this->hrCallback;
			return E_ABORT;
		} 

		hr = this->pEvent->WaitForCompletion(100, &state);
		if (SUCCEEDED(hr)){
			if (state == EC_COMPLETE){ 
				//Set the callback/progress to 100%. Signal a finish!
				if (this->progress) {
					this->progress(this->duration, this->duration, this->progressUserData);
				}
				if (this->callback) {
					//Special signal: NULL buffer/0 bytes long to indicate finish
					this->callback(NULL, 0, duration, this->callbackUserData);
				}
				
				//IMPORTANT: Stop graph to prevent memory leaks.
				this->pControl->Stop();
				return S_OK;
			}
			else {
				//Graph stopped, but abort event is caught before this is reached
				return NGC_UNEXPECTED_RUN_ERROR; 
			}
		}

		this->pControl->GetState(100, &state);
	} while (state == State_Running);

	//Graph should always be running - abort events are caught while it's still running
	return NGC_UNEXPECTED_RUN_ERROR;
}

void NGC_Interface::Abort() {
	SetEvent(abortEvent);
}

//INullGrabberCB implementations.
STDMETHODIMP NGC_Interface::QueryInterface(REFIID riid, void **ppvObject) {
	return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE NGC_Interface::AddRef() {
	return 2;
}
ULONG STDMETHODCALLTYPE NGC_Interface::Release() {
	return 1;
}
STDMETHODIMP NGC_Interface::SampleCB(IMediaSample *pSample,
	REFERENCE_TIME *StartTime, REFERENCE_TIME *EndTime) {
	unsigned char *buffer;
	HRESULT hr = S_OK;
	CAutoLock lock(&m_Lock);

	if (this->progress) {
		this->progress(*StartTime, this->duration, this->progressUserData);
	}

	if (this->callback && !IsEventActive(abortEvent)) {
		hr = pSample->GetPointer(&buffer);
		if (SUCCEEDED(hr) && buffer != NULL) {
			if (FAILED(hr = this->callback(buffer, pSample->GetActualDataLength(), 
				*StartTime, this->callbackUserData))) 
			{
				SetEvent(abortEvent); //Stop!
				this->hrCallback = hr;
				return hr;
			}
		}
	}

	return hr;
}

/////////////////////////////////Private methods///////////////////////////////
//Graph initialization and creation
HRESULT NGC_Interface::initGraphFilters(wchar_t *filepath, const CMediaType *pmtAccept){
	HRESULT hr;
	hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
							IID_IGraphBuilder, (void**) &pGraph);
	if (SUCCEEDED(hr)) {
		IMediaFilter *pMedia;

		pGraph->QueryInterface(IID_IMediaControl, (void**) &pControl);
		pGraph->QueryInterface(IID_IMediaEvent, (void**) &pEvent);
		pGraph->QueryInterface(IID_IMediaSeeking, (void**) &pStatus);
		pGraph->QueryInterface(IID_IMediaFilter, (void**) &pMedia);
		
		//Ensure correct time format.
		pStatus->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);

		//This part is important: It makes DS run as fast as possible, instead of at playback speed.
		pMedia->SetSyncSource(NULL);
		pMedia->Release();

		//Try to use StreamBufferSource first, if possible.
		hr = CoCreateInstance(CLSID_StreamBufferSource, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter, (void**) &pSourceF);
		if (FAILED(hr)) {
			pSourceF = NULL;
		} else {
			IFileSourceFilter *f;
			hr = pSourceF->QueryInterface(IID_IFileSourceFilter, (void**) &f);
			if (FAILED(hr)) {
				pSourceF->Release();
				pSourceF = NULL;
			} else {
				hr = f->Load(filepath, NULL);
				f->Release();
				if (FAILED(hr)) {
					pSourceF->Release();
					pSourceF = NULL;
				} else {
					hr = pGraph->AddFilter(pSourceF, L"Source");
					if (FAILED(hr)) {
						pSourceF->Release();
						pSourceF = NULL;
					}
				}
			}
		}

		if (pSourceF == NULL) {
			hr = pGraph->AddSourceFilter(filepath, L"Source", &pSourceF);
			if (FAILED(hr))
				return hr;
		}
		
		//Manual instantiation - correct??
		pNullGrabber = (CNullGrabber*) CNullGrabber::CreateInstance(NULL, &hr);
		if (FAILED(hr))
			return hr;
		pNullGrabber->AddRef();

		hr = pGraph->AddFilter(pNullGrabber, L"Null Grabber");
		if (FAILED(hr))
			return hr;

		pNullGrabber->SetAcceptedMediaType(pmtAccept);
		pNullGrabber->SetCallback(this);
	}

	return hr;
}

bool NGC_Interface::findPin(IEnumPins *pEnum, IPin **pin, int direction){
	PIN_INFO pinfo;

	*pin = NULL;
	while(pEnum->Next(1, pin, NULL) == S_OK){
		(*pin)->QueryPinInfo(&pinfo);
		if (pinfo.pFilter)
			pinfo.pFilter->Release();
		
		if(pinfo.dir == direction)
			break;
		(*pin)->Release();
		*pin = NULL;
	}

	return (*pin) != 0;
}

HRESULT NGC_Interface::connectFilters(){
	HRESULT hr;
	IEnumPins *pEnum;
	IPin *pinIn, *pinOut;

	//Get the input pin of the null grabber filter
	pNullGrabber->EnumPins(&pEnum);
	pEnum->Reset();
	if (!findPin(pEnum, &pinIn, PINDIR_INPUT)) {
		return NGC_UNKNOWN_COM_ERROR;
	}
	pEnum->Release();

	//Find the right output pin of the source filter and connect.
	pSourceF->EnumPins(&pEnum);
	pEnum->Reset();
	while (findPin(pEnum, &pinOut, PINDIR_OUTPUT)){
		hr = pGraph->ConnectDirect(pinOut, pinIn, NULL);
		if (hr == S_OK)
			break;
		pinOut->Release();
		pinOut = NULL;
	}
	pEnum->Release();
	pinIn->Release();

	if (!pinOut) {
		return NGC_NO_INPUT_STREAM;
	}

	//Get the duration, now that everything's connected.
	this->pStatus->GetDuration(&duration);
	pinOut->Release();
	return S_OK;
}
	
#define RELEASE_AND_ZERO(item) if (item) { item->Release(); item = NULL; }
void NGC_Interface::closeGraph(){
	RELEASE_AND_ZERO(pNullGrabber);
	RELEASE_AND_ZERO(pSourceF);
	RELEASE_AND_ZERO(pEvent);
	RELEASE_AND_ZERO(pControl);
	RELEASE_AND_ZERO(pStatus);
	RELEASE_AND_ZERO(pGraph);
}

///////////////////////////////C API method stubs//////////////////////////////

NGC_API_DECLARE(HRESULT) NGC_Init(wchar_t *filepath, AM_MEDIA_TYPE *pmtAccept, NGC_Info **pRet) {
	NGC_Interface *iface = NULL;

	if (!filepath || !pRet) {
		return E_INVALIDARG;
	}

	try {
		iface = new NGC_Interface(filepath, (CMediaType *) pmtAccept);
	} catch (HRESULT ret) {
		(*pRet) = NULL;
		return ret;
	} catch (std::bad_alloc&) { //Gotta catch 'em all
		(*pRet) = NULL;
		return E_OUTOFMEMORY;
	}

	(*pRet) = (NGC_Info *) iface;
	return S_OK;
}

NGC_API_DECLARE(void) NGC_SetSampleCallback(NGC_Info *info, NGC_Callback callback, void *userData) {
	if (info) {
		((NGC_Interface *) info)->SetSampleCallback(callback, userData);
	}
}

NGC_API_DECLARE(void) NGC_SetProgressCallback(NGC_Info *info, NGC_Progress progress, void *userData) {
	if (info) {
		((NGC_Interface *) info)->SetProgressCallback(progress, userData);
	}
}

NGC_API_DECLARE(HRESULT) NGC_Run(NGC_Info *info) {
	if (info) {
		return ((NGC_Interface *)info)->Run();
	}
	return false;
}

NGC_API_DECLARE(void) NGC_Abort(NGC_Info *info) {
	if (info) {
		((NGC_Interface *)info)->Abort();
	}
}

NGC_API_DECLARE(void) NGC_Free(NGC_Info **info) {
	if (info && *info) {
		delete ((NGC_Interface *) *info);
		*info = NULL;
	}
}